//@@license@@

#ifndef TUCUXI_QUERY_QUERYSTATUS_H
#define TUCUXI_QUERY_QUERYSTATUS_H


namespace Tucuxi {
namespace Query {

enum class [[nodiscard]] QueryStatus{/// Everything fine
                                     Ok = 0,
                                     /// Some requests are wrong
                                     PartiallyOk,
                                     /// None of the requests could be calculated correctly
                                     Error,
                                     /// Error during xml query import
                                     ImportError,
                                     /// Xml file or string with bad format (not imported yet)
                                     BadFormat,
                                     /// Undefined (should not be observed when exporting a query)
                                     Undefined};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERYSTATUS_H
