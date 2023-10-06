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
