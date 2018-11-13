/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef DRUGMODELDOMAIN_H
#define DRUGMODELDOMAIN_H


#include "tucucore/operation.h"
#include "tucucommon/translatablestring.h"

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

protected:
    std::vector<std::unique_ptr<Constraint> > m_constraints;

    Tucuxi::Common::TranslatableString m_description;
};


} // namespace Core
} // namespace Tucuxi


#endif // DRUGMODELDOMAIN_H
