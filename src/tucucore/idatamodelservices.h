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


#ifndef TUCUXI_CORE_IDATAMODELSERVICES_H
#define TUCUXI_CORE_IDATAMODELSERVICES_H

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Core {

class IDataModelServices : public Tucuxi::Common::Interface
{
public:
    virtual bool loadDrug(const std::string& _xmlDrugDescription) = 0;
    virtual bool loadTreatment(const std::string& _xmlTreatmentDescription) = 0;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_IDATAMODELSERVICES_H
