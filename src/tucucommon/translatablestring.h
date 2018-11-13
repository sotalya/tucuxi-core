#ifndef TRANSLATABLESTRING_H
#define TRANSLATABLESTRING_H

#include <map>

namespace Tucuxi {
namespace Common {


///
/// \brief The TranslatableString class
/// This class is used to store texts that have translations.
/// It allows to store many translations, to define a default language,
/// and to get a specific translation or the one of the default language
///
/// At startup, english is the default language.
///
class TranslatableString
{
public:

    ///
    /// \brief TranslatableString empty constructor
    ///
    TranslatableString();

    ///
    /// \brief setDefaultLanguage defines the application default language
    /// \param _language The Id of the language to set
    /// The language defined here will affect every TranslatableString object.
    ///
    static void setDefaultLanguage(std::string _language);

    ///
    /// \brief setString Sets the string for the default language
    /// \param _string The new string value
    ///
    void setString(std::string _string);

    ///
    /// \brief setString Sets the string for a specific language
    /// \param _string The new string value
    /// \param _language The Id of the language
    ///
    void setString(std::string _string, std::string _language);

    ///
    /// \brief getString Gets the string for a specific language
    /// \param _language The language corresponding
    /// \return
    ///
    std::string getString(std::string _language) const;

    ///
    /// \brief getString Gets the string for the default language
    /// \return The string corresponding to the default language
    ///
    std::string getString() const;

protected:

    /// A map of <language, text> to store a string for each language
    std::map<std::string, std::string> m_map;

    /// A static variable storing the default language used by every instance of
    /// TranslatableString
    static std::string sm_language;

};

} // namespace Common
} // namespace Tucuxi

#endif // TRANSLATABLESTRING_H
