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


#ifndef TUCUXI_CORE_DRUGMODELMETADATA_H
#define TUCUXI_CORE_DRUGMODELMETADATA_H

#include <vector>

#include "tucucommon/translatablestring.h"

namespace Tucuxi {
namespace Core {

class DrugModelMetadata
{
public:
    DrugModelMetadata();

    void setDrugName(const Tucuxi::Common::TranslatableString& _name)
    {
        m_drugName = _name;
    }
    const Tucuxi::Common::TranslatableString& getDrugName() const
    {
        return m_drugName;
    }

    void setDrugDescription(const Tucuxi::Common::TranslatableString& _name)
    {
        m_drugDescription = _name;
    }
    const Tucuxi::Common::TranslatableString& getDrugDescription() const
    {
        return m_drugDescription;
    }

    void setStudyName(Tucuxi::Common::TranslatableString _studyName)
    {
        m_studyName = std::move(_studyName);
    }
    const Tucuxi::Common::TranslatableString& getStudyName() const
    {
        return m_studyName;
    }

    void setDistribution(Tucuxi::Common::TranslatableString _distribution)
    {
        m_distribution = std::move(_distribution);
    }
    const Tucuxi::Common::TranslatableString& getDistribution() const
    {
        return m_distribution;
    }

    void setElimination(Tucuxi::Common::TranslatableString _elimination)
    {
        m_elimination = std::move(_elimination);
    }
    const Tucuxi::Common::TranslatableString& getElimination() const
    {
        return m_elimination;
    }

    void setDescription(Tucuxi::Common::TranslatableString _description)
    {
        m_description = std::move(_description);
    }
    const Tucuxi::Common::TranslatableString& getDescription() const
    {
        return m_description;
    }

    void setAuthorName(std::string _name)
    {
        m_authorName = std::move(_name);
    }
    const std::string& getAuthorName() const
    {
        return m_authorName;
    }

    void addAtc(std::string _atc)
    {
        m_atcs.push_back(std::move(_atc));
    }
    void setAtcs(const std::vector<std::string>& _atcs)
    {
        m_atcs = _atcs;
    }
    const std::vector<std::string>& getAtcs() const
    {
        return m_atcs;
    }

protected:
    Tucuxi::Common::TranslatableString m_drugName;
    Tucuxi::Common::TranslatableString m_drugDescription;
    Tucuxi::Common::TranslatableString m_studyName;
    Tucuxi::Common::TranslatableString m_distribution;
    Tucuxi::Common::TranslatableString m_elimination;
    Tucuxi::Common::TranslatableString m_description;
    std::string m_authorName;

    std::vector<std::string> m_atcs;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGMODELMETADATA_H
