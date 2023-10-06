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


#ifndef TUCUXI_CORE_PARAMTERIDS_H
#define TUCUXI_CORE_PARAMTERIDS_H

#include <map>
#include <string>

namespace Tucuxi {
namespace Core {

class ParameterId
{
public:
    static const int size = 45; // NOLINT(readability-identifier-naming)
    enum Enum
    {
        V,
        V1,
        V2,
        CL,
        Ka,
        Ke,
        K12,
        K21,
        K13,
        K31,
        F,
        Q,
        Q1,
        Q2,
        Ktr,
        Tlag,
        Km,
        Vmax,
        TestA,
        TestM,
        TestR,
        TestS,
        TestA0,
        TestM0,
        TestR0,
        TestS0,
        TestA1,
        TestM1,
        TestR1,
        TestS1,
        TestT,
        a,
        b,
        Kenz,
        Emax,
        ECmid,
        EDmid,
        DoseMid,
        Fmax,
        NN,
        MTT,
        AllmCL,
        RQCL,
        RV2V1,
        Unknown
    };

    static Enum fromString(const std::string& _id)
    {
        static const std::map<std::string, Enum> sm_paramIds = {
                // NOLINT(readability-identifier-naming)
                {"V", ParameterId::V},           {"V1", ParameterId::V1},
                {"V2", ParameterId::V2},         {"CL", ParameterId::CL},
                {"Ka", ParameterId::Ka},         {"Ke", ParameterId::Ke},
                {"K12", ParameterId::K12},       {"K21", ParameterId::K21},
                {"K13", ParameterId::K13},       {"K31", ParameterId::K31},
                {"F", ParameterId::F},           {"Q", ParameterId::Q},
                {"Q1", ParameterId::Q1},         {"Q2", ParameterId::Q2},
                {"Ktr", ParameterId::Ktr},       {"Tlag", ParameterId::Tlag},
                {"Km", ParameterId::Km},         {"Vmax", ParameterId::Vmax},
                {"TestA", ParameterId::TestA},   {"TestM", ParameterId::TestM},
                {"TestR", ParameterId::TestR},   {"TestS", ParameterId::TestS},
                {"TestA0", ParameterId::TestA0}, {"TestM0", ParameterId::TestM0},
                {"TestR0", ParameterId::TestR0}, {"TestS0", ParameterId::TestS0},
                {"TestA1", ParameterId::TestA1}, {"TestM1", ParameterId::TestM1},
                {"TestR1", ParameterId::TestR1}, {"TestS1", ParameterId::TestS1},
                {"TestT", ParameterId::TestT},   {"a", ParameterId::a},
                {"b", ParameterId::b},           {"Kenz", ParameterId::Kenz},
                {"Emax", ParameterId::Emax},     {"ECmid", ParameterId::ECmid},
                {"EDmid", ParameterId::EDmid},   {"DoseMid", ParameterId::DoseMid},
                {"Fmax", ParameterId::Fmax},     {"NN", ParameterId::NN},
                {"MTT", ParameterId::MTT},       {"AllmCL", ParameterId::AllmCL},
                {"RQCL", ParameterId::RQCL},     {"RV2V1", ParameterId::RV2V1}};
        std::map<std::string, Enum>::const_iterator it = sm_paramIds.find(_id);
        if (it != sm_paramIds.end()) {
            return it->second;
        }
        return ParameterId::Unknown;
    }
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_PARAMTERIDS_H
