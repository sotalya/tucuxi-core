#include "tucucore/hardcodedoperation.h"

bool
IdealBodyWeight::compute(const OperationInputList &_inputs, double &_result) const
{
    int height;
    bool isMale;

    if (!getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    if (isMale) {
        _result = height - 100 - (height - 150) / 4.0;
    } else {
        _result = height - 100 - (height - 150) / 2.5;
    }

    return true;
}


void
IdealBodyWeight::fillRequiredInputs()
{
    OperationInput height("height", InputType::INTEGER);
    OperationInput isMale("isMale", InputType::BOOL);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(isMale);
}


bool
BodySurfaceArea::compute(const OperationInputList &_inputs, double &_result) const
{
    int height;
    double weight;

    if (!getInputValue(_inputs, "height", height) || !getInputValue(_inputs, "weight", weight)) {
        return false;
    }

    _result = 0.007184 * std::pow(height, 0.725) * std::pow(weight, 0.425);

    return true;
}


void
BodySurfaceArea::fillRequiredInputs()
{
    OperationInput height("height", InputType::INTEGER);
    OperationInput weight("weight", InputType::DOUBLE);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(weight);
}


bool
eGFR_CockcroftGaultGeneral::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int age;
    double creatinine;
    bool isMale;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "age", age) ||
            !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || age < 0) {
        return false;
    }

    _result = (140 - age) * weight / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void
eGFR_CockcroftGaultGeneral::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool
eGFR_CockcroftGaultIBW::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    bool isMale;
    double IBW;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "age", age) ||
            !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    IdealBodyWeight IBWComputation;
    bool rc = IBWComputation.evaluate({ OperationInput("height", height), OperationInput("isMale", isMale) }, IBW);
    if (!rc || IBW <= 0) {
        return false;
    }

    _result = (140 - age) * (weight < IBW ? weight : IBW) / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void
eGFR_CockcroftGaultIBW::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool
eGFR_CockcroftGaultAdjIBW::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    bool isMale;
    double IBW;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "age", age) ||
            !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    IdealBodyWeight IBWComputation;
    bool rc = IBWComputation.evaluate({ OperationInput("height", height), OperationInput("isMale", isMale) }, IBW);
    if (!rc || IBW <= 0) {
        return false;
    }

    _result = (140 - age) * (IBW + 0.4 * (weight - IBW)) / creatinine * (isMale ? 1.23 : 1.04);

    return true;
}


void
eGFR_CockcroftGaultAdjIBW::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool
GFR_MDRD::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    bool isMale;
    bool isAB;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine) ||
            !getInputValue(_inputs, "isMale", isMale) || !getInputValue(_inputs, "isAB", isAB)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    double eGFR = 175 * std::pow(0.0113 * creatinine, -1.154) * std::pow(age, -0.203) * (isMale ? 1 : 0.742) * (isAB ? 1.212 : 1);

    double BSA;
    BodySurfaceArea BSAComputation;
    bool rc = BSAComputation.evaluate({ OperationInput("height", height), OperationInput("weight", weight) }, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void
GFR_MDRD::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);
    OperationInput isAB("isAB", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(isAB);
}


bool
GFR_CKD_EPI::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    bool isMale;
    bool isAB;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine) ||
            !getInputValue(_inputs, "isMale", isMale) || !getInputValue(_inputs, "isAB", isAB)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    double eGFR = 141 * std::pow(std::min(0.0113 * creatinine / (isMale ? 0.9 : 0.7), 1.0), isMale ? -0.411 : -0.329) * \
            std::pow(std::max(0.0113 * creatinine / (isMale ? 0.9 : 0.7), 1.0), -1.209) * std::pow(0.993, age) * \
            (isMale ? 1 : 1.018) * (isAB ? 1.159 : 1);

    double BSA;
    BodySurfaceArea BSAComputation;
    bool rc = BSAComputation.evaluate({ OperationInput("height", height), OperationInput("weight", weight) }, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void
GFR_CKD_EPI::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);
    OperationInput isAB("isAB", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(isAB);
}


bool
eGFR_Schwartz::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int height;
    int age;
    double creatinine;
    bool isMale;
    bool bornAtTerm;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "age", age) || !getInputValue(_inputs, "creatinine", creatinine) ||
            !getInputValue(_inputs, "isMale", isMale) || !getInputValue(_inputs, "bornAtTerm", bornAtTerm)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    _result = height / creatinine;
    if (age <= 1 && weight <= 2.5) {
        _result *= 0.33;
    } else if (age <= 1 && bornAtTerm) {
        _result *= 0.45;
    } else if (age > 2 && (age <= 13 || (age <= 20 && !isMale))) {
        _result *= 0.55;
    } else if (age > 13 && age <= 20 && isMale) {
        _result *= 0.70;
    } else {
        return false;
    }

    return true;
}


void
eGFR_Schwartz::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);
    OperationInput bornAtTerm("bornAtTerm", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
    m_requiredInputs.push_back(bornAtTerm);
}


bool
GFR_Jelliffe::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    bool isMale;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "age", age) ||
            !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    double eGFR = (isMale ? 1 : 0.9) * (98 - (0.8 * age - 20)) / (0.0113 * creatinine);

    double BSA;
    BodySurfaceArea BSAComputation;
    bool rc = BSAComputation.evaluate({ OperationInput("height", height), OperationInput("weight", weight) }, BSA);
    if (!rc || BSA <= 0) {
        return false;
    }

    _result = eGFR * BSA / 1.73;

    return true;
}


void
GFR_Jelliffe::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}


bool
eGFR_SalazarCorcoran::compute(const OperationInputList &_inputs, double &_result) const
{
    double weight;
    int age;
    int height;
    double creatinine;
    bool isMale;

    if (!getInputValue(_inputs, "weight", weight) || !getInputValue(_inputs, "age", age) ||
            !getInputValue(_inputs, "height", height) ||
            !getInputValue(_inputs, "creatinine", creatinine) || !getInputValue(_inputs, "isMale", isMale)) {
        return false;
    }

    // Sanity check on parameters
    if (creatinine <= 0 || weight <= 0 || height <= 0 || age < 0) {
        return false;
    }

    _result = isMale ? ((137 - age) * (0.285 * weight + 12.1 * height * height) * 0.0113 / (51 * creatinine)) \
                     : ((146 - age) * (0.287 * weight + 9.74 * height * height) * 0.0113 / (60 * creatinine));

    return true;
}


void
eGFR_SalazarCorcoran::fillRequiredInputs()
{
    OperationInput weight("weight", InputType::DOUBLE);
    OperationInput height("height", InputType::INTEGER);
    OperationInput age("age", InputType::INTEGER);
    OperationInput creatinine("creatinine", InputType::DOUBLE);
    OperationInput isMale("isMale", InputType::BOOL);

    m_requiredInputs.push_back(weight);
    m_requiredInputs.push_back(age);
    m_requiredInputs.push_back(height);
    m_requiredInputs.push_back(creatinine);
    m_requiredInputs.push_back(isMale);
}
