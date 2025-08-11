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


#ifndef COMPUTINGREQUEST_H
#define COMPUTINGREQUEST_H

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The ComputingRequest class
/// This class embeds all the information needed to start a computation.
/// For every computation an object of this class is sent to the IComputingService.
/// It mainly consists in the DrugModel and the DrugTreatment, and a set of
/// ComputingTraits. Therefore it is possible to send a request for a patient
/// represented by its treatment and drug model, and embed subrequests in it.
/// For instance you can ask for a prediction, percentiles and an adjustment in
/// a single request.
///
/// A ComputingRequest is not responsible for the objects it embedds. It is a simple
/// container.
///
class ComputingRequest
{
public:
    ///
    /// \brief ComputingRequest
    /// \param _id The unique Id of the request
    /// \param _drugModel A reference to the drug model to be used for computation
    /// \param _drugTreatment The patient drug treatment
    /// \param _computingTraits The list of computings to be performed
    ///


    ComputingRequest(
            RequestResponseId _id,
            const DrugModel& _drugModel,
            const DrugTreatment& _drugTreatment,
            std::unique_ptr<ComputingTrait> _computingTrait);
    ///
    /// \brief getId Retrieves the Id of the request
    /// \return The Id of the request
    ///
    RequestResponseId getId() const;

    ///
    /// \brief getDrugModel Retrieves a reference to the drug model
    /// \return The reference to the drug model
    ///
    const DrugModel& getDrugModel() const;

    ///
    /// \brief getDrugTreatment Retrieves a reference to the drug treatment
    /// \return The reference to the drug treatment
    ///
    const DrugTreatment& getDrugTreatment() const;

    ///
    /// \brief getComputingTraits Retrieves a reference to the list of computing traits
    /// \return A reference to the list of computing traits
    ///
    const ComputingTraits& getComputingTraits() const;

protected:
    //! Id of the request
    RequestResponseId m_id;

    //! Reference to the drug model
    const DrugModel& m_drugModel;

    //! Reference to the drug treatment
    const DrugTreatment& m_drugTreatment;

    //! Reference to the list of computing traits
    std::unique_ptr<ComputingTraits> m_computingTraits;
};

} // namespace Core
} // namespace Tucuxi

#endif // COMPUTINGREQUEST_H
