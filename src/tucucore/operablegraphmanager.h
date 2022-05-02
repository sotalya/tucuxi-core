//@@license@@

#ifndef OPERABLEGRAPHMANAGER_H
#define OPERABLEGRAPHMANAGER_H

#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {

class IOperableInput;
class IOperable;
typedef int IOperable_ID;

/// \brief Class responsible for creating and maintaining a graph of dependencies between operables, and to perform the
///        operations in the correct order.
class OperableGraphManager
{
public:
    /// \brief Explicitly initialize the next ID value and 'computed' flag.
    OperableGraphManager();

    /// \brief Compute all the retrievable values (that is, all those that have an associated name).
    /// \return True if the graph is valid and all the desired computations could be performed, false otherwise.
    bool evaluate();

    /// \brief Return the current value of a stored input or operation.
    /// \param _name Name of the sought value.
    /// \param _value Value of the sought value.
    /// \return True if the value could be retrieved, false otherwise.
    bool getValue(const std::string& _name, double& _value) const;

    /// \brief Check if a given input is present in the Operable Graph.
    /// \param _name Name of the sought value.
    /// \return True if the input is present, false otherwise.
    bool isInputPresent(const std::string& _name) const;

    /// \brief Record an input to an operable in the operable graph.
    /// \param _input Shared pointer to the Operable input.
    /// \param _scriptVarName Input name.
    /// \param _isComputed Flag marking the pointed element as the result of an operable node that has to be
    ///        computed. [default value = false --> "simple" input]
    /// \param _id Identifier of the node in question, if the _isComputed flag marks it as an Operable node.
    ///        [default value = -1 --> "simple" input]
    /// \return True if the insertion was successful, false otherwise.
    /// \post if (m_operableInputs.find(_scriptVarName) == false && (_isComputed == false || _id >= 0)) { m_operableInputs += _input && [RETURN] == true }
    ///       else { m_operableInputs == PREV(m_operableInputs) && m_operables == PREV(m_operables) && [RETURN] == false };
    bool registerInput(
            const std::shared_ptr<IOperableInput>& _input,
            const std::string& _scriptVarName,
            bool _isComputed = false,
            IOperable_ID _id = -1);

    /// \brief Record an operable in the operable graph.
    /// \param _operable Operable to insert.
    /// \param _scriptVarName Optional name for the produced output -- required if it has to be used as input to another
    ///        operable.
    /// \return True if the insertion was successful, false otherwise.
    /// \post if (_operable != nullptr && (_scriptVarName == "" || m_operableInputs.find(_scriptVarName) == false))
    ///           { m_operables += _operable && (if (_scriptVarName != "") { m_operableInputs.find(_scriptVarName) == true }) && [RETURN] == true }
    ///       else { m_operableInputs == PREV(m_operableInputs) && m_operables == PREV(m_operables) && [RETURN] == false };
    bool registerOperable(const std::shared_ptr<IOperable>& _operable, const std::string& _scriptVarName = "");


    /// \brief Class' output operator.
    /// \param _output Output stream.
    /// \param _dt Self reference to the OperableGraphManager to print.
    /// \return Output stream given as input (for output chaining).
    friend std::ostream& operator<<(std::ostream& _output, const OperableGraphManager& _ogm)
    {
        _output << "-- OperableGraphManager INPUTS --\n";
        for (const auto& i : _ogm.m_operableInputs) {
            _output << "\t" << i.first << "\n"
                    << "\t\tValue = " << i.second.getValue() << "\n"
                    << "\t\tID = " << i.second.getOperableID() << "\n"
                    << "\t\tisComputed = " << i.second.isComputed() << "\n";
        }
        _output << "-- OperableGraphManager m_operables --\n";
        for (const auto& i : _ogm.m_operableInputs) {
            _output << "\t" << i.first << "\n";
        }
        return _output;
    }


private:
    /// \brief Recursively evaluate an Operable node.
    /// \param _id ID of the node that has to be evaluated.
    /// \param _alreadyComputed Map containing the evaluation state of the graph's nodes.
    /// \return True if the evaluation was successful, false otherwise.
    bool evaluateOperableNode(IOperable_ID _id, std::map<IOperable_ID, bool>& _alreadyComputed);

    /// \brief Helper function that checks if a subgraph starting at the given node gets back to a previously visited
    ///        node.
    /// \param _cur Current node to explore.
    /// \param _visited Map marking nodes as visited or not.
    /// \param _gotBack Map used to check whether we are getting back to a previously-visited node.
    /// \return True if there is a cycle in the graph, false otherwise.
    bool isCyclic(const std::string& _cur, std::map<std::string, bool>& _visited, std::map<std::string, bool>& _gotBack)
            const;

    /// \brief Check if a graph is valid --- that is, if does not contain loops.
    /// \return Returns true if the graph is valid, false otherwise.
    bool isValid();


    /// \brief Input node in the Operable graph.
    class OperableInputNode
    {
    public:
        /// \brief Build an Operable Input graph's node.
        /// \param _ptr Shared pointer to the actual input.
        /// \param _isComputed Flag marking the pointed element as the result of an operable node that has to be
        ///        computed.
        /// \param _id Identifier of the operable node in question.
        /// If we are considering a "simple" input, then we just have to record the shared ptr, while if it has to be
        /// computed then it has to be in the m_operables map too, and therefore it has an id too.
        /// \pre _isComputed == false || (_isComputed == true && _id >= 0 && m_operables.find(_id) != false)
        /// \pre _ptr != nullptr
        OperableInputNode(std::shared_ptr<IOperableInput> _ptr, bool _isComputed = false, IOperable_ID _id = -1);

        /// \brief If the node has to be computed, return the ID of the linked operable.
        /// \return ID of the linked operable if the node has to be computed, -1 otherwise.
        IOperable_ID getOperableID() const;

        /// \brief Retrieve the value of the input.
        /// \return Value of the input represented by the node.
        /// If the node represents a "simple" input, then just retrieve the value via getValue(), otherwise perform the
        /// evaluation and return the computed value. This gets chained, performing automagically all the operations
        /// needed to get the required value.
        double getValue() const;

        /// \brief Return whether the node has to be computed or not.
        /// \return True if the node has to be computed from a series of other inputs, false if it is a "simple" node.
        bool isComputed() const;


    private:
        /// \brief Shared pointer to the actual input.
        std::shared_ptr<IOperableInput> m_sptr;

        /// \brief Flag marking the node as to be computed or not (that is, linked with a "simple" input or an Operable)
        bool m_isComputed;

        /// \brief If the node has to the computed, then this is the ID of the linked operable.
        IOperable_ID m_id;
    };

    /// \brief Operable node in the graph (node linked with an operation, as opposed to a node that just stores a value)
    class OperableComputeNode
    {
    public:
        /// \brief Build an Operable node starting from its shared pointer.
        /// \param _ptr Shared pointer to the computation node.
        /// \note The dependencies that are needed for the operations of this node are retrieved via getInputs().
        /// \warning Adding an Operable node might leave the system in an *inconsistent* state -- that is, a state when
        ///          some of the required inputs are absent. This is normal, as there is no imposed order in which to
        ///          insert the Operables and Inputs. However, this inconsistency has to be solved before performing the
        ///          evaluation.
        /// \pre _ptr != nullptr
        OperableComputeNode(std::shared_ptr<IOperable> _ptr);

        /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
        ///        OperableGraphManager and computing the desired output.
        /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
        /// \param _value Computed result.
        /// \return True if the evaluation could be performed, false in case of errors.
        virtual bool evaluate(const OperableGraphManager& _graphMgr);

        /// \brief Retrieve the list of dependencies of the Operable node.
        /// \return Vector containing the dependencies of the node.
        std::vector<std::string> getDependencies() const;

        /// \brief Perform the evaluation and return the computed value.
        /// \return Value computed by the node.
        /// This gets chained, performing automagically all the operations needed to get the required value.
        double getValue() const;


    private:
        /// \brief Shared pointer to the operable.
        std::shared_ptr<IOperable> m_sptr;

        /// \brief Set of dependencies, espressed as a vector of strings in the m_operableInputs vector (each Operable
        ///        whose result has to be reused must associate a name to it!).
        std::vector<std::string> m_deps;
    };

    /// \brief Operable inputs, either user-provided or computed via other operables.
    std::map<std::string, OperableInputNode> m_operableInputs;

    /// \brief Actual dependencies between operables and inputs.
    /// This map represents the real graph of dependencies between the operables and their inputs. It is updated in real
    /// time as a new input or operable is added.
    std::map<IOperable_ID, OperableComputeNode> m_operables;

    /// \brief ID available for uniquely identify an Operable in the m_operables map.
    IOperable_ID m_nextAvailableID;

    /// \brief Mark a graph as already checked for validity.
    bool m_validGraph;
};


/// \brief Abstract class representing an input node.
class IOperableInput
{
public:
    /// \brief Default virtual destructor, required for proper object's destruction.
    virtual ~IOperableInput() = default;

    /// \brief Retrieve the value stored in the input node.
    /// \return Value of the input node (evaluated, if the real node behind is an Operable one).
    virtual double getValue() const = 0;
};


/// \brief Abstract class representing a computation node.
class IOperable : public IOperableInput
{
public:
    /// \brief Default virtual destructor, required for proper object's destruction.
    ~IOperable() override = default;

    /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
    ///        OperableGraphManager.
    /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
    /// \return True if the evaluation could be performed, false in case of errors.
    virtual bool evaluate(const OperableGraphManager& _graphMgr) = 0;

    /// \brief Return the list of required input operands.
    /// \return Vector containing a list of the operands required for the operation.
    /// This list can be filled by the caller to have all the values ready for evaluation.
    virtual OperationInputList getInputs() const = 0;

    /// \brief Return the value computed by the node.
    /// \return Value computed by the node.
    double getValue() const override = 0;
};


/// \brief Operable node, which is either subclassed for testing purposes, or subclassed by a CovariateDefinition to get
///        Operable-like capabilities.
class Operable : public IOperable
{
public:
    /// \brief Create an operable from a fixed value.
    /// \param _value Value of the operable.
    Operable(const double& _value);

    /// \brief Default virtual destructor, required for proper object's destruction.
    ~Operable() override = default;

    /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
    ///        OperableGraphManager.
    /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
    /// \return True if the evaluation could be performed, false in case of errors.
    bool evaluate(const OperableGraphManager& _graphMgr) override;

    /// \brief Return the list of required input operands.
    /// \return Vector containing a list of the operands required for the operation.
    /// This list can be filled by the caller to have all the values ready for evaluation.
    OperationInputList getInputs() const override;

    /// \brief Get the associated operation.
    /// \return Reference to the associated operation.
    virtual Operation& getOperation() const = 0;

    /// \brief Return the latest value computed by the node.
    /// \return Value computed by the node.
    double getValue() const override;

    /// \brief Manually set an Operable's value.
    /// \param _value Value to set.
    virtual void setValue(double _value);


protected:
    /// \brief Latest computed value.
    double m_value;
};


/// \brief Actual sample implementation of an Operable node, useful for testing purposes.
class OperableImpl : public Operable
{
public:
    /// \brief Create an operable from a fixed value.
    /// \param _value Value of the operable.
    OperableImpl(const double& _value) : Operable(_value) {}

    /// \brief Create an operable from a shared pointer to an Operation.
    /// \param _ptr Shared pointer to an Operation object.
    OperableImpl(const std::shared_ptr<Operation>& _ptr) : Operable(0), m_sptr{_ptr} {}

    /// \brief Default virtual destructor, required for proper object's destruction.
    ~OperableImpl() override = default;

    /// \brief Get the associated operation.
    /// \return Reference to the associated operation.
    Operation& getOperation() const override
    {
        return *m_sptr;
    }


protected:
    /// \brief Pointer to the operation.
    std::shared_ptr<Operation> m_sptr;
};


} // namespace Core
} // namespace Tucuxi

#endif // OPERABLEGRAPHMANAGER_H
