/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef DRUGMODELDOMAIN_H
#define DRUGMODELDOMAIN_H

#include "tucucommon/translatablestring.h"

#include "tucucore/operation.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class TranslatedText
{

};

enum class ConstraintType {HARD, SOFT};

class Constraint
{
public:
    Constraint(){};
    virtual ~Constraint(){};

    void addRequiredCovariateId(std::string _id) {m_requiredCovariateIds.push_back(_id);};
    const std::vector<std::string>& getRequiredCovariateIds() const {return m_requiredCovariateIds;}

    void setCheckOperation(std::unique_ptr<Operation> _operation) {m_checkOperation = std::move(_operation);}
    const Operation& getCheckOperation() const { return *m_checkOperation;}

    void setType(ConstraintType _type) {m_type = _type;}

    ConstraintType getType() const { return m_type;}

    void setDescription(Tucuxi::Common::TranslatableString _description) {m_description = _description;}

    INVARIANTS(
            INVARIANT(Invariants::INV_CONSTRAINT_0001, (m_checkOperation != nullptr))
            INVARIANT(Invariants::INV_CONSTRAINT_0001, (m_requiredCovariateIds.size() > 0))
            LAMBDA_INVARIANT(Invariants::INV_CONSTRAINT_0003, {bool ok = true; for(size_t i = 0; i < m_requiredCovariateIds.size(); i++) {ok &= m_requiredCovariateIds.at(i).size() > 0;} return ok;})
            )

protected:

    ConstraintType m_type;
    Tucuxi::Common::TranslatableString m_description;
    std::vector<std::string> m_requiredCovariateIds;
    std::unique_ptr<Operation> m_checkOperation;
};

class DrugModelDomain
{
public:

    DrugModelDomain(std::unique_ptr<Constraint> _constraint = nullptr);
    virtual ~DrugModelDomain();

    void addConstraint(std::unique_ptr<Constraint> _constraint) { m_constraints.push_back(std::move(_constraint));}
    const std::vector<std::unique_ptr< Constraint > >& getConstraints() const { return m_constraints;}

    void setDescription(Tucuxi::Common::TranslatableString _description) { m_description = _description;}

    const Tucuxi::Common::TranslatableString& getDescription() const { return m_description;}


    INVARIANTS(
            LAMBDA_INVARIANT(Invariants::INV_DRUGMODELDOMAIN_0001, {bool ok = true;for(size_t i = 0; i < m_constraints.size(); i++) {ok &= m_constraints.at(i)->checkInvariants();} return ok;})
            )

protected:
    std::vector<std::unique_ptr<Constraint> > m_constraints;

    Tucuxi::Common::TranslatableString m_description;
};


} // namespace Core
} // namespace Tucuxi


#endif // DRUGMODELDOMAIN_H
