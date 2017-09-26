/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/operablegraphmanager.h"

namespace Tucuxi {
namespace Core {

OperableGraphManager::OperableGraphManager()
    : m_nextAvailableID{0}, m_validGraph{true}
{

}


bool
OperableGraphManager::evaluate()
{
    // Ensure that the graph is valid, before wasting time doing computations (or, even worse, get stuck in an infinite
    // loop). To save computations, the result is cached and won't change unless the graph's structure changes.
    if (!m_validGraph) {
        if (!isValid()) {
            return false;
        }
    }

    // Algorithm: travel sequentially over the map of Operables, and for each element if it is computable then
    //            request its evaluation -- eventually recursing if needed. Keep a parallel map with boolean values to
    //            keep track of which elements still require to be computed.
    std::map<IOperable_ID, bool> alreadyComputed;

    // Initialize the map to false since all the nodes still have to be computed.
    for (const auto &it : m_operables) {
        alreadyComputed.insert(std::pair<IOperable_ID, bool>(it.first, false));
    }

    // Now, for practicality, iterate directly on the alreadyComputed map.
    for (auto const &current : alreadyComputed) {
        bool rc = evaluateOperableNode(current.first, alreadyComputed);
        if (!rc) {
            return false;
        }
    }
    return true;
}


bool
OperableGraphManager::getValue(const std::string &_name, double &_value) const
{
    if (m_operableInputs.find(_name) != m_operableInputs.end()) {
        _value = m_operableInputs.at(_name).getValue();
        return true;
    }
    return false;
}


bool
OperableGraphManager::registerInput(std::shared_ptr<IOperableInput> _input, const std::string &_scriptVarName,
                                    const bool _isComputed, const IOperable_ID _id)
{
    if (_input == nullptr || (_isComputed && _id < 0)) {
        return false;
    }
    // Create a OperableInputNode object on the fly with the required fields filled, and push it into the map.
    std::pair<std::map<std::string, OperableInputNode>::iterator, bool> ret;
    if (_isComputed) {
        ret = m_operableInputs.insert(std::pair<std::string, OperableInputNode>(_scriptVarName,
                                                                                OperableInputNode(_input, true, _id)));
    } else {
        ret = m_operableInputs.insert(std::pair<std::string, OperableInputNode>(_scriptVarName,
                                                                                OperableInputNode(_input)));
    }
    // ret.second is false if the name was already taken.
    if (!ret.second) {
        return false;
    }

    m_validGraph = false;
    return true;
}


bool
OperableGraphManager::registerOperable(std::shared_ptr<IOperable> _operable, const std::string &_scriptVarName)
{
    if (_operable == nullptr) {
        return false;
    }

    m_operables.insert(std::pair<IOperable_ID, OperableComputeNode>(m_nextAvailableID,
                                                                    OperableComputeNode(_operable)));

    // Take advantage of the registerInputs function to insert the value of the current Operable in the list of
    // available inputs, if it is expected to be reused (that is, a variable name has been specified).
    if (_scriptVarName.length() > 0) {
        return registerInput(_operable, _scriptVarName, true, m_nextAvailableID++);
    }
    m_nextAvailableID++;
    m_validGraph = false;
    return true;
}


bool
OperableGraphManager::evaluateOperableNode(const IOperable_ID _id, std::map<IOperable_ID, bool> &_alreadyComputed)
{
    if (_alreadyComputed.at(_id)) {
        // Operable already evaluated, so return success.
        return true;
    }

    // Retrieve the list of dependencies.
    std::vector<std::string> deps = m_operables.at(_id).getDependencies();
    // Iterate over the dependencies and check each of them:
    // If the m_operableInputs map marks it as a "simple" input, then just skip to the subsequent one. Otherwise check
    // in the _alreadyComputed map if it has already been computed. If this is not the case, recursively evaluate it.
    for (auto const &it : deps) {
        if (m_operableInputs.at(it).isComputed()) {
            IOperable_ID opNodeId;
            opNodeId = m_operableInputs.at(it).getOperableID();
            if (!_alreadyComputed.at(opNodeId)) {
                bool rc = evaluateOperableNode(opNodeId, _alreadyComputed);
                if (!rc) {
                    return false;
                }
            }
        }
    }
    // At this point all dependencies have been satisfied, so we can safely perform our evaluation.
    _alreadyComputed.at(_id) = m_operables.at(_id).evaluate(*this);
    return _alreadyComputed.at(_id);
}


bool
OperableGraphManager::isValid()
{
    // Check that no loops are present by checking if any back-edge is present.
    std::map<std::string, bool> visited;
    for (auto it : m_operableInputs) {
        visited.insert(std::pair<std::string, bool> (it.first, false));
    }

    std::map<std::string, bool> gotBack = visited;

    for (auto it : m_operableInputs) {
        if (isCyclic(it.first, visited, gotBack)) {
            return false;
        }
    }

    m_validGraph = true;
    return true;
}


bool
OperableGraphManager::isCyclic(const std::string &_cur,
                               std::map<std::string, bool> &_visited,
                               std::map<std::string, bool> &_gotBack) const
{
    if (_visited.at(_cur) == false) {
        _visited.at(_cur) = true;
        _gotBack.at(_cur) = true;

        // Get deps.
        IOperable_ID nodeID = m_operableInputs.at(_cur).getOperableID();
        if (nodeID >= 0) {
            // Just need to check computed nodes!
            std::vector<std::string> deps = m_operables.at(nodeID).getDependencies();

            for (auto depIt : deps) {
                if (!_visited.at(depIt) && isCyclic(depIt, _visited, _gotBack)) {
                    return true;
                } else if (_gotBack.at(depIt)) {
                    return true;
                }
            }
        }
    }
    _gotBack.at(_cur) = false;
    return false;
}


OperableGraphManager::OperableInputNode::OperableInputNode(std::shared_ptr<IOperableInput> _ptr,
                                                           const bool _isComputed, const IOperable_ID _id)
    : m_sptr{_ptr}, m_isComputed{_isComputed}, m_id{_id}
{

}


IOperable_ID
OperableGraphManager::OperableInputNode::getOperableID() const
{
    if (m_isComputed) {
        return m_id;
    }
    return -1;
}


double
OperableGraphManager::OperableInputNode::getValue() const
{
    return m_sptr->getValue();
}


bool
OperableGraphManager::OperableInputNode::isComputed() const
{
    return m_isComputed;
}


OperableGraphManager::OperableComputeNode::OperableComputeNode(std::shared_ptr<IOperable> _ptr)
    : m_sptr{_ptr}
{
    OperationInputList inList = m_sptr->getInputs();

    for (const auto &it : inList) {
        m_deps.push_back(it.getName());
    }
}

bool
OperableGraphManager::OperableComputeNode::evaluate(const OperableGraphManager &_graphMgr)
{
    return m_sptr->evaluate(_graphMgr);
}


std::vector<std::string>
OperableGraphManager::OperableComputeNode::getDependencies() const
{
    return m_deps;
}


double
OperableGraphManager::OperableComputeNode::getValue() const
{
    return m_sptr->getValue();
}


Operable::Operable(const double &_value)
    : m_sptr{nullptr}, m_value{_value}
{

}


Operable::Operable(const std::shared_ptr<Operation> &_ptr)
    : m_sptr{_ptr}, m_value{0.0}
{

}


Operable::Operable(const Operation &_op)
    : m_value{0.0}
{
    m_sptr = std::make_shared<Operation>(_op);
}


bool
Operable::evaluate(const OperableGraphManager &_graphMgr)
{
    if (m_sptr == nullptr) {
        return true;
    }

    // Collect inputs
    OperationInputList inputs = getInputs();

    for (auto &input : inputs) {
        double val;
        bool rc;
        rc = _graphMgr.getValue(input.getName(), val);
        if (!rc) {
            return false;
        }
        input.setValue(val);
    }

    return m_sptr->evaluate(inputs, m_value);
}


OperationInputList
Operable::getInputs() const
{
    if (m_sptr == nullptr) {
        return { };
    }
    return m_sptr->getInputs();
}


double
Operable::getValue() const
{
    return m_value;
}


void
Operable::setValue(const double _value)
{
    m_value = _value;
}

}
}
