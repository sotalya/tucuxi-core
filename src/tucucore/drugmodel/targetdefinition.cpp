#include "targetdefinition.h"


namespace Tucuxi {
namespace Core {

std::string toString(TargetType _type)
{
    static std::map<TargetType, std::string> map = {{TargetType::UnknownTarget, "unknown"},
                                                    {TargetType::Residual, "residual"},
                                                    {TargetType::Peak, "peak"},
                                                    {TargetType::Mean, "mean"},
                                                    {TargetType::Auc, "auc"},
                                                    {TargetType::Auc24, "auc24"},
                                                    {TargetType::CumulativeAuc, "CumulativeAuc"},
                                                    {TargetType::AucOverMic, "aucOverMic"},
                                                    {TargetType::Auc24OverMic, "auc24OverMic"},
                                                    {TargetType::TimeOverMic, "timeOverMic"},
                                                    {TargetType::AucDividedByMic, "aucDividedByMic"},
                                                    {TargetType::Auc24DividedByMic, "auc24DividedByMic"},
                                                    {TargetType::PeakDividedByMic, "peakDividedByMic"},
                                                   };
    return map.at(_type);
}


} // namespace Core
} // namespace Tucuxi
