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


#ifndef HARDCODEDOPERATION_H
#define HARDCODEDOPERATION_H

#include <memory>

#include "tucucore/operation.h"


namespace Tucuxi {
namespace Core {

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,bugprone-macro-parentheses)
#define HARDCODED_OPERATION(NAME)                                                       \
    class NAME : public HardcodedOperation                                              \
    {                                                                                   \
    public:                                                                             \
        NAME()                                                                          \
        {                                                                               \
            NAME::fillRequiredInputs();                                                 \
            m_filledInputs = true;                                                      \
        }                                                                               \
        virtual std::unique_ptr<Operation> clone() const                                \
        {                                                                               \
            return std::make_unique<NAME>(*this);                                       \
        }                                                                               \
                                                                                        \
    protected:                                                                          \
        virtual bool compute(const OperationInputList& _inputs, double& _result) const; \
        virtual void fillRequiredInputs();                                              \
    };

/// \ingroup TucuCore
/// \brief Ideal Body Weight computation.
HARDCODED_OPERATION(IdealBodyWeight);

/// \ingroup TucuCore
/// \brief Body Surface Area computation.
HARDCODED_OPERATION(BodySurfaceArea);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault general equation.
HARDCODED_OPERATION(eGFR_CockcroftGaultGeneral);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault general equation.
HARDCODED_OPERATION(OperationEGFRCockcroftGaultGeneral);



/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault equation based on IBW.
HARDCODED_OPERATION(eGFR_CockcroftGaultIBW);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Cockcroft-Gault equation based on the adjusted body weight.
HARDCODED_OPERATION(eGFR_CockcroftGaultAdjIBW);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the MDRD approach.
HARDCODED_OPERATION(GFR_MDRD);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the Chronic Kidney Disease Epidemiology Collaboration method.
HARDCODED_OPERATION(GFR_CKD_EPI);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Schwartz (pediatric) method.
HARDCODED_OPERATION(eGFR_Schwartz);

/// \ingroup TucuCore
/// \brief Compute the **GFR** value using the Jelliffe method.
HARDCODED_OPERATION(GFR_Jelliffe);

/// \ingroup TucuCore
/// \brief Compute the eGFR value using the Salazar-Corcoran method (obese patients).
HARDCODED_OPERATION(eGFR_SalazarCorcoran);

/// \ingroup TucuCore
/// \brief Compute an active moiety from a single analyte, so no calculation required.
HARDCODED_OPERATION(direct);

/// \ingroup TucuCore
/// \brief Compute an active moiety from two analytes by summing them.
HARDCODED_OPERATION(sum2);


/// \brief Collection of operations -- groups hard coded operations so that they can be easily retrieved.
class OperationCollection
{
public:
    /// \brief Initialize an empty Operation collection.
    OperationCollection() = default;

    /// \brief Add an operation to the collection.
    /// \param _operation A pointer to an Operation to add to the collection.
    /// \param _operationId Id of the operation, used to retrieve it later on.
    /// This function is typically used at startup to populate the collection with the existing hardcoded operations.
    bool addOperation(std::shared_ptr<Operation> _operation, const std::string& _operationId);

    /// \brief Given an Id, returns the operation with the corresponding identifier.
    /// \param _operation ID of the PkModel asked by the caller.
    /// \return A PkModel corresponding to the ID if available, nullptr if no PkModel is found for the specified ID.
    std::shared_ptr<Operation> getOperationFromId(const std::string& _operationId) const;

    /// \brief Adds all available hardcoded operations to a collection.
    /// \param _collection The collection to populate.
    /// \return True if the collection has been successfully populated, false otherwise.
    /// This method shall be called at the beginning of the execution.
    virtual bool populate();

protected:
    std::map<std::string, std::shared_ptr<Operation>> m_collection;
};

} // namespace Core
} // namespace Tucuxi

#endif // HARDCODEDOPERATION_H
