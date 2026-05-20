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
#include <vector>

#include "translatablestring.h"


using namespace Tucuxi::Common;

std::string& TranslatableString::defaultLanguage()
{
    static std::string lang = "en";
    return lang;
}

TranslatableString::TranslatableString() = default;

TranslatableString::TranslatableString(std::string _string)
{
    m_map[defaultLanguage()] = std::move(_string);
}

TranslatableString::TranslatableString(std::string _string, const std::string& _language)
{
    m_map[_language] = std::move(_string);
}

void TranslatableString::setString(std::string _string)
{
    m_map[defaultLanguage()] = std::move(_string);
}

void TranslatableString::setString(std::string _string, const std::string& _language)
{
    m_map[_language] = std::move(_string);
}

void TranslatableString::setDefaultLanguage(std::string _language)
{
    defaultLanguage() = std::move(_language);
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
    return getString(defaultLanguage());
}

std::vector<std::string> TranslatableString::getLanguages() const
{
    std::vector<std::string> languages;
    languages.reserve(m_map.size());

    for (const auto& pair : m_map) {
        languages.push_back(pair.first);
    }
    return languages;
}