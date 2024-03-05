//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/iterator.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/utils.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/parametersextractor.h"

#include "testutils.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common::Utils;

/// \brief Check whether a covariate event with a given name, date, and value is present in a timed covariate series.
/// \param _id Name of the covariate to check.
/// \param _date Expected date of the covariate event.
/// \param _value Expected value of the covariate at the time of the event.
/// \param _series Timed map containing the covariate events.
/// \return True if the event is present, false otherwise.
static bool covariateEventIsPresent(
        const std::string& _id,
        const DateTime& _date,
        const Value& _value,
        const std::map<DateTime, std::vector<std::pair<std::string, Value>>>& _timedCValues)
{
    if (_timedCValues.find(_date) == _timedCValues.end()) {
        return false;
    }
    if (std::find_if(
                _timedCValues.at(_date).begin(),
                _timedCValues.at(_date).end(),
                [_id, _value](const std::pair<std::string, Value>& _v) {
                    return _v.first == _id && _v.second == _value;
                })
        == _timedCValues.at(_date).end()) {
        return false;
    }
    else {
        return true;
    }
}


/// \brief Print the elements of a timed covariate series.
/// \param _timedCValues Series to print.
static void printCovariateSeries(const std::map<DateTime, std::vector<std::pair<std::string, Value>>>& _timedCValues)
{
    std::cerr << "--------- Number of time instants: " << _timedCValues.size() << " ----------\n";
    for (const auto& covV : _timedCValues) {
        std::cerr << "@ " << covV.first << " :\n";
        for (const auto& covEl : covV.second) {
            std::cerr << "\t" << covEl.first << " = " << covEl.second << "\n";
        }
    }
    std::cerr << "-----------------------------------------\n";
}

class MyParameterDefinitionIterator : public Tucuxi::Common::Iterator<const ParameterDefinition*>
{
public:
    typedef ParameterDefinitions::const_iterator iteratorType;

    MyParameterDefinitionIterator(iteratorType _first, iteratorType _last) : m_first(_first), m_last(_last)
    {
        reset();
    }
    Tucuxi::Common::Iterator<const ParameterDefinition*>& next() override
    {
        m_it++;
        return *this;
    }
    bool isDone() const override
    {
        return (m_it == m_last);
    }
    const ParameterDefinition* operator*() override
    {
        return m_it->get();
    }
    void reset() override
    {
        m_it = m_first;
    }

private:
    iteratorType m_first;
    iteratorType m_last;
    iteratorType m_it;
};

class TestParameterExtractor
{
public:
    /// \brief Print the elements of a parameter set series.
    /// \param _series Series to print.
    static void printParameterSetSeries(const ParameterSetSeries& _series)
    {
        std::cerr << "--------- Number of time instants: " << _series.m_parameterSets.size() << " ----------\n";
        for (const auto& ps : _series.m_parameterSets) {
            std::cerr << "@ " << ps.m_time << " :\n";
            for (const auto& p : ps.m_parameters) {
                std::cerr << "\t" << p.m_definition.getId() << " = " << p.m_value << "\n";
            }
        }
        std::cerr << "-----------------------------------------\n";
    }

    /// \brief Check whether a parameter event with a given name, date, and value is present in a parameters series.
    /// \param _id Name of the parameter to check.
    /// \param _date Expected date of the parameter event.
    /// \param _value Expected value of the parameter at the time of the event.
    /// \param _series Set of parameter events.
    /// \return True if the event is present, false otherwise.
    static bool parameterEventIsPresent(
            const std::string& _id, const DateTime& _date, const Value& _value, const ParameterSetSeries& _series)
    {
        // Check if the date was available or not.
        std::vector<ParameterSetEvent>::const_iterator it;
        it = std::find_if(
                _series.m_parameterSets.begin(), _series.m_parameterSets.end(), [_date](const ParameterSetEvent& _ev) {
                    return _ev.m_time == _date;
                });
        if (it == _series.m_parameterSets.end()) {
            return false;
        }
        const Parameters& parameters = it->m_parameters;
        if (std::find_if(
                    parameters.begin(),
                    parameters.end(),
                    [_id, _value](const Parameter& _p) {
                        return _p.m_definition.getId() == _id && _p.m_value == _value;
                    })
            == parameters.end()) {
            return false;
        }
        else {
            return true;
        }
    }

    static std::map<DateTime, std::vector<std::pair<std::string, Value>>> get_m_timedCValues(
            ParametersExtractor* extractor)
    {
        return extractor->m_timedCValues;
    }
};

TEST(Core_TestParameterExtractor, PE_constructor)
{
    ParameterDefinitions pDefinitions;
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("ParamA", 1));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("ParamB", 2));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("ParamC", 3));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("ParamD", 4));
    MyParameterDefinitionIterator itDefinitions(pDefinitions.begin(), pDefinitions.end());

    // Invalid definition (_start > _end).
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                std::runtime_error);
    }

    // Covariate appearing out of blue sky.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
        ADD_CDEF_NO_R(Ghost, true, Standard, Bool, Direct, cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));
        // Event appearing out of nowhere.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)),
                std::runtime_error);
    }

    // Duplicate event (same value).
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        // Duplicated event.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)),
                std::runtime_error);
    }

    // Duplicate event (different value).
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        // Duplicated event.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 132));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)),
                std::runtime_error);
    }

    // No duplicate error if time instant is different.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), 123));
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 132));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_NO_THROW(ParametersExtractor(
                cSeries,
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)));
    }

    // No covariates, no parameters. Should still throw no error.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ParameterDefinitions pDef;
        MyParameterDefinitionIterator itDefinitions(pDef.begin(), pDef.end());
        ASSERT_NO_THROW(ParametersExtractor(
                CovariateSeries(),
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)));

        ParametersExtractor extractor = ParametersExtractor(
                CovariateSeries(),
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(1));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0))
                        .size(),
                static_cast<size_t>(0));
    }

    // Some covariates, no parameters. Weird, but should throw no error. The timed covariate values map should be
    // correctly filled nonetheless.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ParameterDefinitions pDef;
        MyParameterDefinitionIterator itDefinitions(pDef.begin(), pDef.end());
        ASSERT_NO_THROW(ParametersExtractor(
                cSeries,
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)));

        ParametersExtractor extractor = ParametersExtractor(
                cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        //        printCovariateSeries(TestParameterExtractor::get_m_timedCValues(&extractor));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(2));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(1));
        // Events pushed forward.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0))
                        .size(),
                static_cast<size_t>(3));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                varToValue(false),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Weight",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                15,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                111,
                TestParameterExtractor::get_m_timedCValues(&extractor)));

        // Events in the correct time span.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0))
                        .size(),
                static_cast<size_t>(2));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                varToValue(true),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                123,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
    }

    // No covariates, some parameters. This can happen if the parameters do not depend on covariates, and should
    // throw no error.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_NO_THROW(ParametersExtractor(
                CovariateSeries(),
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)));

        ParametersExtractor extractor = ParametersExtractor(
                CovariateSeries(),
                itDefinitions,
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(1));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0))
                        .size(),
                static_cast<size_t>(0));
    }

    // Covariates and parameters available.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ParametersExtractor extractor = ParametersExtractor(
                cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        //        printCovariateSeries(TestParameterExtractor::get_m_timedCValues(&extractor));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(2));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(1));
        // Events pushed forward.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0))
                        .size(),
                static_cast<size_t>(3));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                varToValue(false),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Weight",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                15,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                111,
                TestParameterExtractor::get_m_timedCValues(&extractor)));

        // Events in the correct time span.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0))
                        .size(),
                static_cast<size_t>(2));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                varToValue(true),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                123,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
    }

    // Covariates and parameters available, but all at a precise moment after m_start.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ParametersExtractor extractor = ParametersExtractor(
                cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        //            printCovariateSeries(TestParameterExtractor::get_m_timedCValues(&extractor));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(2));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(0));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0)),
                static_cast<size_t>(1));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0))
                        .size(),
                static_cast<size_t>(3));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                varToValue(false),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Weight",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                15,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                111,
                TestParameterExtractor::get_m_timedCValues(&extractor)));

        // Later events.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0))
                        .size(),
                static_cast<size_t>(2));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                varToValue(true),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0),
                123,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
    }

    // Covariates and parameters available, but all at a precise moment after m_start except one which is before
    // m_start. This should fail.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
        ADD_CDEF_NO_R(Foobar, 123, Standard, Double, Linear, cDefinitions);

        CovariateSeries cSeries;
        // Event before m_start that should make everything fail.
        cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 143));
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)),
                std::runtime_error);
    }

    // Covariates and parameters available, but all at a precise moment after m_start except one which is slightly
    // late. This should fail.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
        ADD_CDEF_NO_R(Foobar, 123, Standard, Double, Linear, cDefinitions);

        CovariateSeries cSeries;
        // Event slightly in late that should make everything fail.
        cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 1, 0), 143));
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));

        ASSERT_THROW(
                ParametersExtractor(
                        cSeries,
                        itDefinitions,
                        DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                        DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)),
                std::runtime_error);
    }

    // Covariates and parameters available. Multiple values for a covariate before the start, all discarded except
    // the most recent.
    {
        CovariateDefinitions cDefinitions;
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

        CovariateSeries cSeries;
        // These events are before _start -> should be pushed forward at the parameter's interval beginning.
        cSeries.push_back(
                CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 0, 0), 111));
        // Events in interval time span.
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 123));
        // This one is past _end -> should be discarded.
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 143));
        // Additional events before _start.
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 30, 0), 19));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 12, 8, 45, 0), 21));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 13, 8, 45, 0), 33));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 14, 8, 45, 0), 44));

        ParametersExtractor extractor = ParametersExtractor(
                cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

        //        printCovariateSeries(TestParameterExtractor::get_m_timedCValues(&extractor));

        ASSERT_EQ(TestParameterExtractor::get_m_timedCValues(&extractor).size(), static_cast<size_t>(3));
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor).count(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0)),
                static_cast<size_t>(1));
        // Events pushed forward.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0))
                        .size(),
                static_cast<size_t>(3));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                varToValue(false),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Weight",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                33,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                111,
                TestParameterExtractor::get_m_timedCValues(&extractor)));

        // Events in the correct time span.
        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 14, 8, 45, 0))
                        .size(),
                static_cast<size_t>(1));
        ASSERT_TRUE(covariateEventIsPresent(
                "Weight",
                DATE_TIME_NO_VAR(2017, 8, 14, 8, 45, 0),
                44,
                TestParameterExtractor::get_m_timedCValues(&extractor)));

        ASSERT_EQ(
                TestParameterExtractor::get_m_timedCValues(&extractor)
                        .at(DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0))
                        .size(),
                static_cast<size_t>(2));
        ASSERT_TRUE(covariateEventIsPresent(
                "Gist",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                varToValue(true),
                TestParameterExtractor::get_m_timedCValues(&extractor)));
        ASSERT_TRUE(covariateEventIsPresent(
                "Height",
                DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0),
                123,
                TestParameterExtractor::get_m_timedCValues(&extractor)));
    }
}

TEST(Core_TestParameterExtractor, PE_extract1_0)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

    // Set of covariate events.
    CovariateSeries cSeries;
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 123));

    ParameterDefinitions pDefinitions;

    // Non-computed parameters.
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_A", 1234));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_B", 5678));

    // Computed parameters
    ADD_EXPR2_PDEF(Param_C_C, "Gist * 15 + Weight", "Gist", "Weight", pDefinitions);
    ADD_EXPR2_PDEF(Param_C_D, "Gist * 2 + Height", "Height", "Gist", pDefinitions);

    MyParameterDefinitionIterator itDefinitions(pDefinitions.begin(), pDefinitions.end());
    ASSERT_NO_THROW(ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0)));
    ParametersExtractor extractor = ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0));

    ParameterSetSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printParameterSetSeries(series);

    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_EQ(series.m_parameterSets.size(), static_cast<size_t>(2));

    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_A", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 1234, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_B", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 5678, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 30, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 125, series));
}

TEST(Core_TestParameterExtractor, PE_extract1_1)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

    // Set of covariate events.
    CovariateSeries cSeries;
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));

    ParameterDefinitions pDefinitions;

    // Non-computed parameters.
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_A", 1234));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_B", 5678));

    // Computed parameters
    ADD_EXPR2_PDEF(Param_C_C, "Gist * 15 + Weight", "Gist", "Weight", pDefinitions);
    ADD_EXPR2_PDEF(Param_C_D, "Gist * 2 + Height", "Height", "Gist", pDefinitions);

    MyParameterDefinitionIterator itDefinitions(pDefinitions.begin(), pDefinitions.end());
    ASSERT_NO_THROW(ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0)));
    ParametersExtractor extractor = ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0));

    ParameterSetSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printParameterSetSeries(series);

    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_EQ(series.m_parameterSets.size(), static_cast<size_t>(3));

    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_A", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 1234, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_B", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 5678, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 30, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 113, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 125, series));
}

TEST(Core_TestParameterExtractor, DISABLED_PE_extractParamFromParam)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);

    // Set of covariate events.
    CovariateSeries cSeries;
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));

    ParameterDefinitions pDefinitions;

    // Non-computed parameters.
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_A", 1234));
    pDefinitions.push_back(std::make_unique<ParameterDefinition>("Param_NC_B", 5678));

    // Computed parameters
    ADD_EXPR2_PDEF(Param_C_C, "Gist * 15 + Weight", "Gist", "Weight", pDefinitions);
    ADD_EXPR2_PDEF(Param_C_D, "Gist * 3 + Height", "Height", "Gist", pDefinitions);
    //ADD_EXPR2_PDEF(Param_C_D, "Param_NC_A", "Param_NC_A", "Gist", pDefinitions);

    MyParameterDefinitionIterator itDefinitions(pDefinitions.begin(), pDefinitions.end());
    ASSERT_NO_THROW(ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0)));
    ParametersExtractor extractor = ParametersExtractor(
            cSeries, itDefinitions, DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0));

    ParameterSetSeries series;
    ComputingStatus rc;
    rc = extractor.extract(series);

    //        printParameterSetSeries(series);

    ASSERT_EQ(rc, ComputingStatus::Ok);
    ASSERT_EQ(series.m_parameterSets.size(), static_cast<size_t>(3));

    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_A", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 1234, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_NC_B", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 5678, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_C", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 30, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), 113, series));
    ASSERT_TRUE(TestParameterExtractor::parameterEventIsPresent(
            "Param_C_D", DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 125, series));
}
