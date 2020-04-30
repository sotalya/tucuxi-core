#ifndef QUERYSTATUS_H
#define QUERYSTATUS_H

enum class QueryStatus{

    /// Everything fine
    Ok = 0,
    /// Some requests are wrong
    PartiallyOk,
    /// None of the requests could be calculated correctly
    Error,
    /// Error during xml query improt
    ImportError

};

#endif // QUERYSTATUS_H
