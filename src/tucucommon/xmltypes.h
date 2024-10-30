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


#ifndef TUCUXI_TUCUCOMMON_XMLTYPES_H
#define TUCUXI_TUCUCOMMON_XMLTYPES_H

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Models an XML node
/// \sa XmlNode
enum class EXmlNodeType
{
    Document,    /// A document node. Name and value are empty.
    Element,     /// An element node. Name contains element name. Value contains text of first data node.
    Comment,     /// A comment node. Name is empty. Value contains comment text.
    Data,        /// A data node. Name is empty. Value contains data text.
    CData,       /// A CDATA node. Name is empty. Value contains data text.
    Declaration, /// A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
    Doctype,     /// A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    Pi,          /// A "Processing Instruction" node. Name contains target. Value contains instructions.
    Undefined    /// Type not defined yet.
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_XMLTYPES_H
