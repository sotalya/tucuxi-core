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


#ifndef TUCUXI_SIGNPARSER_H
#define TUCUXI_SIGNPARSER_H

#include <string>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

#include "signature.h"

namespace Tucuxi {
namespace Sign {

const std::vector<std::string> OPTIONAL_NODES = {"comments", "description"};

enum class ParsingError
{
    SIGNATURE_OK = 1,
    UNABLE_TO_LOAD_DRUGFILE = -1,
    NO_SIGNATURE = -2,
};

class SignParser
{
public:
    /// \brief Load a Signature from a drug file
    /// \param signedDrugfilePath The signed drug file path
    /// \return SIGNATURE_OK if the signature has been loaded
    /// UNABLE_TO_LOAD_DRUGFILE if the drug file couldn't be loaded
    /// NO_SIGNATURE if the drug file has not been signed (does not contain <signature>)
    static ParsingError loadSignature(std::string signedDrugfilePath, Signature& signature);

private:
    /// \brief Extract the information from the signature node
    /// \param node The signature node
    /// \param signature The signature object to set the information
    static void extractSignature(Tucuxi::Common::XmlNodeIterator node, Signature& signature);

    /// \brief Extract the information from the certificates node
    /// \param node The signature node
    /// \param signature The signature object to set the information
    static void extractCertificates(Tucuxi::Common::XmlNodeIterator node, Signature& signature);

    /// \brief Extract the data that have been signed from the xml document
    /// \param document The xml document
    /// \param root The root node
    /// \param signatureIterator The signature iterator
    /// \param signature The signature object to set the information
    static void extractSignedData(
            Tucuxi::Common::XmlDocument& document,
            Tucuxi::Common::XmlNode& root,
            Tucuxi::Common::XmlNodeIterator signatureIterator,
            Signature& signature);
};

} // namespace Sign
} // namespace Tucuxi

#endif //TUCUXI_SIGNPARSER_H
