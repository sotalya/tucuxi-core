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


#ifndef DRUGMODELDOMAIN_H
#define DRUGMODELDOMAIN_H

#include "tucucommon/translatablestring.h"

#include "tucucore/invariants.h"
#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {

class TranslatedText
{
};

enum class ConstraintType
{
    HARD,
    SOFT,
    MANDATORYHARD,
    UNDEFINED
};

class Constraint
{
public:
    Constraint(){};
    virtual ~Constraint(){};

    void addRequiredCovariateId(std::string _id)
    {
        m_requiredCovariateIds.push_back(std::move(_id));
    };
    const std::vector<std::string>& getRequiredCovariateIds() const
    {
        return m_requiredCovariateIds;
    }

    void setCheckOperation(std::unique_ptr<Operation> _operation)
    {
        m_checkOperation = std::move(_operation);
    }
    const Operation& getCheckOperation() const
    {
        return *m_checkOperation;
    }
    Operation* getCheckOperationPointer() const
    {
        return m_checkOperation.get();
    }

    void setType(ConstraintType _type)
    {
        m_type = _type;
    }

    ConstraintType getType() const
    {
        return m_type;
    }

    void setDescription(Tucuxi::Common::TranslatableString _description)
    {
        m_description = std::move(_description);
    }

    void setErrorMessage(Tucuxi::Common::TranslatableString _errorMessage)
    {
        m_errorMessage = std::move(_errorMessage);
    }

    Tucuxi::Common::TranslatableString getErrorMessage() const
    {
        return m_errorMessage;
    }

    INVARIANTS(
            INVARIANT(Invariants::INV_CONSTRAINT_0001, (m_checkOperation != nullptr), "A constraint has no operation");
            INVARIANT(
                    Invariants::INV_CONSTRAINT_0002,
                    (!m_requiredCovariateIds.empty()),
                    "A constraint has no required covariate Ids");
            INVARIANT(
                    Invariants::INV_CONSTRAINT_0003,
                    (m_type != ConstraintType::UNDEFINED),
                    "A constraint type should not be undefined");
            LAMBDA_INVARIANT(
                    Invariants::INV_CONSTRAINT_0004,
                    {
                        bool ok = true;
                        for (const auto& requiredCovariateId : m_requiredCovariateIds) {
                            ok &= !requiredCovariateId.empty();
                        }
                        return ok;
                    },
                    "There is an empty covariate Id in the required covariates list of a constraint");)

protected:
    ConstraintType m_type{ConstraintType::UNDEFINED};
    Tucuxi::Common::TranslatableString m_description;
    std::vector<std::string> m_requiredCovariateIds;
    std::unique_ptr<Operation> m_checkOperation;
    Tucuxi::Common::TranslatableString m_errorMessage;
};

class DrugModelDomain
{
public:
    DrugModelDomain(std::unique_ptr<Constraint> _constraint = nullptr);
    virtual ~DrugModelDomain();

    void addConstraint(std::unique_ptr<Constraint> _constraint)
    {
        m_constraints.push_back(std::move(_constraint));
    }
    const std::vector<std::unique_ptr<Constraint> >& getConstraints() const
    {
        return m_constraints;
    }

    void setDescription(Tucuxi::Common::TranslatableString _description)
    {
        m_description = std::move(_description);
    }

    const Tucuxi::Common::TranslatableString& getDescription() const
    {
        return m_description;
    }


    INVARIANTS(LAMBDA_INVARIANT(
            Invariants::INV_DRUGMODELDOMAIN_0001,
            {
                bool ok = true;
                for (const auto& constraint : m_constraints) {
                    ok &= constraint->checkInvariants();
                }
                return ok;
            },
            "A constraint of a drug domain has an error"))

protected:
    std::vector<std::unique_ptr<Constraint> > m_constraints;

    Tucuxi::Common::TranslatableString m_description;
};


} // namespace Core
} // namespace Tucuxi


#endif // DRUGMODELDOMAIN_H
