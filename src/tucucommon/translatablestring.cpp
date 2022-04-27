//@@lisence@@

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
