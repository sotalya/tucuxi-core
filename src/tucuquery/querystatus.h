#ifndef QUERYSTATUS_H
#define QUERYSTATUS_H

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

#endif // QUERYSTATUS_H
