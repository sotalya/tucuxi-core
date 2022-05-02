//@@license@@

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
