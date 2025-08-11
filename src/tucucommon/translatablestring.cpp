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


#include <utility>

#include "translatablestring.h"


using namespace Tucuxi::Common;

std::string TranslatableString::sm_language = "en"; // NOLINT(readability-identifier-naming)

TranslatableString::TranslatableString() = default;

TranslatableString::TranslatableString(std::string _string)
{
    m_map[sm_language] = std::move(_string);
}

TranslatableString::TranslatableString(std::string _string, const std::string& _language)
{
    m_map[_language] = std::move(_string);
}

void TranslatableString::setString(std::string _string)
{
    m_map[sm_language] = std::move(_string);
}

void TranslatableString::setString(std::string _string, const std::string& _language)
{
    m_map[_language] = std::move(_string);
}

void TranslatableString::setDefaultLanguage(std::string _language)
{
    sm_language = std::move(_language);
}


std::string TranslatableString::getString(const std::string& _language) const
{
    auto search = m_map.find(_language);
    if (search != m_map.end()) {
        return search->second;
    }
    return "";
}

std::string TranslatableString::getString() const
{
    return getString(sm_language);
}
