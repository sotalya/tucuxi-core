/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef DRUGDEFINITIONS_H
#define DRUGDEFINITIONS_H

#include <utility>

#include "tucucore/definitions.h"
#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {


///
/// \brief The AnalyteId class
/// This class is simply a std::string.
/// The rationale is that it makes it mandatory to use ActiveMoietyId wherever
/// needed, and as such not to mix things with AnalyteGroupId or AnalyteId.
/// Therefore, the code is safer, and more readable.
///
class ActiveMoietyId
{
public:
    explicit ActiveMoietyId(std::string _s) : m_s(std::move(_s)) {}
    explicit ActiveMoietyId(const char* _s) : m_s(_s) {}
    size_t size() const
    {
        return m_s.size();
    }
    bool empty() const
    {
        return m_s.empty();
    }

    std::string toString() const
    {
        return m_s;
    }

    inline bool operator==(const ActiveMoietyId& _other) const
    {
        return this->m_s == _other.m_s;
    }
    inline bool operator<(const ActiveMoietyId& _other) const
    {
        return this->m_s < _other.m_s;
    }

protected:
    std::string m_s;
};

// inline bool operator==(const ActiveMoietyId& _lhs, const ActiveMoietyId& _rhs){ return _lhs.toString() == _rhs.toString(); }



/// \brief Default value for a drug (given either as a numerical value, or as an operation to obtain it).
/// A PopulationValue represents the value of a parameter needed for computing a drug's dosage, and is the default value
/// adopted when the corresponding patient information is missing.
class PopulationValue
{
public:
    /// \brief Create a population value from its components.
    /// \param _id Identifier of the value.
    /// \param _value Default value.
    /// \param _operation Operation associated with the parameter.
    /// The operation has a default value of nullptr
    PopulationValue(std::string _id, const Value _value, std::unique_ptr<Operation> _operation = nullptr)
        : m_id(std::move(_id)), m_value(_value), m_operation(std::move(_operation))
    {
    }

    /// \brief Create a population value with a fixed value.
    /// \param _value Default value.
    /// The value is not operable
    PopulationValue(const Value _value) : m_value(_value), m_operation(nullptr) {}

    virtual ~PopulationValue() {}

    /// \brief Return the identifier of the value.
    /// \return Value's identifier.
    std::string getId() const
    {
        return m_id;
    }

    /// \brief Get the operation associated with the value.
    /// \return Reference to the operation associated with the value.
    virtual Operation& getOperation() const
    {
        return *m_operation;
    }

    /// \brief Get the value.
    /// \return Returns the value.
    Value getValue() const
    {
        return m_value;
    }

    /// \brief Set the value.
    /// \param _value Value to set.
    void setValue(const Value _value)
    {
        m_value = _value;
    }

    /// \brief Get if the value is computed via an operation or is a fixed value.
    /// \return True if the value is a result of an operation, false otherwise.
    bool isComputed() const
    {
        return m_operation != nullptr;
    }


protected:
    std::string m_id;
    Value m_value;
    std::unique_ptr<Operation> m_operation;
};

template<typename DefinitionClass>
class IndividualValue
{
public:
    IndividualValue(const DefinitionClass& _definition) : m_definition(_definition) {}

    virtual ~IndividualValue() {}

    const DefinitionClass& getDefinition() const
    {
        return m_definition;
    }

protected:
    const DefinitionClass& m_definition;

    /// \brief Get the operation associated with the value.
    /// \return Reference to the operation associated with the value.
    virtual Operation& getOperation() const
    {
        return m_definition.getOperation();
    }
};

} // namespace Core
} // namespace Tucuxi

#endif // DRUGDEFINITIONS_H
