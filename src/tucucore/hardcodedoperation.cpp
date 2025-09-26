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


#include "tucucore/hardcodedoperation.h"

using namespace Tucuxi::Core;

bool IdealBodyWeight::compute(const OperationInputList& _inputs, double& _result) const
{
    int height;
    double sex;
    bool isMale;

    if (!getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    isMale = (sex >= 0.5);

    if (isMale) {
        _result = height - 100 - (height - 150) / 4.0;
    }
    else {
        _result = height - 100 - (height - 150) / 2.5;
    }

    return true;
}


void IdealBodyWeight::fillRequiredInputs()
{
    OperationInput height("height", InputType::INTEGER);
    OperationInput isMale("sex", InputType::DOUBLE);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(isMale);
}


bool BodySurfaceArea::compute(const OperationInputList& _inputs, double& _result) const
{
    int height;
    double weight;

    if (!getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "bodyweight", weight)) {
        return false;
    }

    _result = 0.007184 * std::pow(height, 0.725) * std::pow(weight, 0.425);

    return true;
}


void BodySurfaceArea::fillRequiredInputs()
{
    OperationInput height("height", InputType::INTEGER);
    OperationInput weight("bodyweight", InputType::DOUBLE);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(weight);
}


bool eGFR_CockcroftGaultGeneral::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int age;
    double creatinine;
    double sex;
    bool isMale;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "age", age)
        || !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    _result = (140 - age) * weight / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void eGFR_CockcroftGaultGeneral::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool eGFR_CockcroftGaultIBW::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    double sex;
    bool isMale;
    double IBW; // NOLINT(readability-identifier-naming)

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "age", age)
        || !getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    IdealBodyWeight IBWComputation; // NOLINT(readability-identifier-naming)
    bool rc = IBWComputation.evaluate({OperationInput("height", height), OperationInput("sex", sex)}, IBW);
    if (!rc || IBW <= 0) {
        return false;
    }

    _result = (140 - age) * (weight < IBW ? weight : IBW) / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void eGFR_CockcroftGaultIBW::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool eGFR_CockcroftGaultAdjIBW::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    double sex;
    bool isMale;
    double IBW; // NOLINT(readability-identifier-naming)

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "age", age)
        || !getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    IdealBodyWeight IBWComputation; // NOLINT(readability-identifier-naming)
    bool rc = IBWComputation.evaluate({OperationInput("height", height), OperationInput("sex", sex)}, IBW);
    if (!rc || IBW <= 0) {
        return false;
    }

    _result = (140 - age) * (IBW + 0.4 * (weight - IBW)) / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void eGFR_CockcroftGaultAdjIBW::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool eGFR_MDRD::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    double sex;
    bool isMale;
    bool isAB;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "height", height)
        || !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex) || !getInputValue(_inputs, "isAB", isAB)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    double eGFR = 175 * std::pow(0.0113 * creatinine, -1.154) * std::pow(age, -0.203) * (isMale ? 1 : 0.742)
                  * (isAB ? 1.212 : 1);

    double BSA;                     // NOLINT(readability-identifier-naming)
    BodySurfaceArea BSAComputation; // NOLINT(readability-identifier-naming)
    bool rc = BSAComputation.evaluate({OperationInput("height", height), OperationInput("bodyweight", weight)}, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void eGFR_MDRD::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);
    OperationInput isAB("isAB", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(isAB);
}


bool eGFR_CKD_EPI::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    double sex;
    bool isMale;
    bool isAB;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "height", height)
        || !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex) || !getInputValue(_inputs, "isAB", isAB)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    double eGFR = 141 * std::pow(std::min(0.0113 * creatinine / (isMale ? 0.9 : 0.7), 1.0), isMale ? -0.411 : -0.329)
                  * std::pow(std::max(0.0113 * creatinine / (isMale ? 0.9 : 0.7), 1.0), -1.209) * std::pow(0.993, age)
                  * (isMale ? 1 : 1.018) * (isAB ? 1.159 : 1);

    double BSA;                     // NOLINT(readability-identifier-naming)
    BodySurfaceArea BSAComputation; // NOLINT(readability-identifier-naming)
    bool rc = BSAComputation.evaluate({OperationInput("height", height), OperationInput("bodyweight", weight)}, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void eGFR_CKD_EPI::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);
    OperationInput isAB("isAB", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(isAB);
}


bool eGFR_Schwartz::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    double sex;
    bool isMale;
    bool bornAtTerm;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "height", height)
        || !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex) || !getInputValue(_inputs, "bornAtTerm", bornAtTerm)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    _result = height / creatinine;
    if (age <= 1 && weight <= 2.5) {
        _result *= 0.33;
    }
    else if (age <= 1 && bornAtTerm) {
        _result *= 0.45;
    }
    else if (age > 2 && (age <= 13 || (age <= 20 && !isMale))) {
        _result *= 0.55;
    }
    else if (age > 13 && age <= 20 && isMale) {
        _result *= 0.70;
    }
    else {
        return false;
    }

    return true;
}


void eGFR_Schwartz::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);
    OperationInput bornAtTerm("bornAtTerm", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(bornAtTerm);
}


bool GFR_Jelliffe::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    double sex;
    bool isMale;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "age", age)
        || !getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    double eGFR = (isMale ? 1 : 0.9) * (98 - (0.8 * age - 20)) / (0.0113 * creatinine);

    double BSA;                     // NOLINT(readability-identifier-naming)
    BodySurfaceArea BSAComputation; // NOLINT(readability-identifier-naming)
    bool rc = BSAComputation.evaluate({OperationInput("height", height), OperationInput("bodyweight", weight)}, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void GFR_Jelliffe::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool eGFR_SalazarCorcoran::compute(const OperationInputList& _inputs, double& _result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    double sex;
    bool isMale;

    if (!getInputValue(_inputs, "bodyweight", weight) || !getInputValue(_inputs, "age", age)
        || !getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "creatinine", creatinine)
        || !getInputValue(_inputs, "sex", sex)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    isMale = (sex >= 0.5);

    _result = isMale ? ((137 - age) * (0.285 * weight + 12.1 * height * height) * 0.0113 / (51 * creatinine))
                     : ((146 - age) * (0.287 * weight + 9.74 * height * height) * 0.0113 / (60 * creatinine));

    return true;
}


void eGFR_SalazarCorcoran::fillRequiredInputs()
{
    OperationInput weight("bodyweight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("sex", InputType::DOUBLE);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool direct::compute(const OperationInputList& _inputs, double& _result) const
{
    double input0;

    if (!getInputValue(_inputs, "input0", input0)) {
        return false;
    }

    _result = input0;

    return true;
}


void direct::fillRequiredInputs()
{
    OperationInput input0("input0", InputType::DOUBLE);

    m_requiredInputs.push_back(input0);
}

bool sum2::compute(const OperationInputList& _inputs, double& _result) const
{
    double input0;
    double input1;

    if (!getInputValue(_inputs, "input0", input0)) {
        return false;
    }
    if (!getInputValue(_inputs, "input1", input1)) {
        return false;
    }

    _result = input0 + input1;

    return true;
}


void sum2::fillRequiredInputs()
{
    OperationInput input0("input0", InputType::DOUBLE);
    OperationInput input1("input1", InputType::DOUBLE);

    m_requiredInputs.push_back(input0);
    m_requiredInputs.push_back(input1);
}



bool OperationCollection::addOperation(std::shared_ptr<Operation> _operation, const std::string& _operationId)
{
    m_collection[_operationId] = std::move(_operation);
    return true;
}

std::shared_ptr<Operation> OperationCollection::getOperationFromId(const std::string& _operationId) const
{
    auto pos = m_collection.find(_operationId);
    if (pos == m_collection.end()) {
        return nullptr;
    }
    return pos->second;
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_OPERATION_TO_COLLECTION(_TYPE)                                \
    do {                                                                  \
        std::shared_ptr<Operation> operation = std::make_shared<_TYPE>(); \
        addOperation(operation, #_TYPE);                                  \
    } while (0);


bool OperationCollection::populate()
{
    // Here we add all the known hardcoded operations.
    // The Id of the operation is the name of the class.

    ADD_OPERATION_TO_COLLECTION(IdealBodyWeight);
    ADD_OPERATION_TO_COLLECTION(BodySurfaceArea);
    ADD_OPERATION_TO_COLLECTION(eGFR_CockcroftGaultGeneral);
    ADD_OPERATION_TO_COLLECTION(eGFR_CockcroftGaultIBW);
    ADD_OPERATION_TO_COLLECTION(eGFR_CockcroftGaultAdjIBW);
    ADD_OPERATION_TO_COLLECTION(eGFR_MDRD);
    ADD_OPERATION_TO_COLLECTION(eGFR_CKD_EPI);
    ADD_OPERATION_TO_COLLECTION(eGFR_Schwartz);
    ADD_OPERATION_TO_COLLECTION(GFR_Jelliffe);
    ADD_OPERATION_TO_COLLECTION(eGFR_SalazarCorcoran);
    ADD_OPERATION_TO_COLLECTION(direct);
    ADD_OPERATION_TO_COLLECTION(sum2);
    return true;
}
