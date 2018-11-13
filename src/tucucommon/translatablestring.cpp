#include "translatablestring.h"

using namespace Tucuxi::Common;

std::string TranslatableString::sm_language = "en";

TranslatableString::TranslatableString()
{

}

void TranslatableString::setString(std::string _string)
{
    m_map[sm_language] = _string;
}

void TranslatableString::setString(std::string _string, std::string _language)
{
    m_map[_language] = _string;
}

void TranslatableString::setDefaultLanguage(std::string _language)
{
    sm_language = _language;
}


std::string TranslatableString::getString(std::string _language) const
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
