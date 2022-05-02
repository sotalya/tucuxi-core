//@@license@@

#ifndef COMPUTINGABORTER_H
#define COMPUTINGABORTER_H


namespace Tucuxi {
namespace Core {

///
/// \brief The ComputingAborter class
/// This class is passed to methods that require a huge processing time.
/// It only supply a method to test if the processing should be aborted or not.
/// If it returns true, the processing should be aborted and all resources freed.
/// If it returns true a second call would return false, so be carful with that.
class ComputingAborter
{
public:
    ///
    /// \brief Indicates if the calculation should be aborted
    /// \return true if the calculation should be aborted, false else
    /// This function should be called at some points in calculation in order to abort as soon as possible
    /// when asked by an external component
    ///
    virtual bool shouldAbort()
    {
        return false;
    }

    virtual ~ComputingAborter();
};

} // namespace Core
} // namespace Tucuxi


#endif // COMPUTINGABORTER_H
