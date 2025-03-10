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


#ifndef ICOMPUTINGSERVICE_H
#define ICOMPUTINGSERVICE_H

#include <memory>

#include "tucucommon/interface.h"

#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class ComputingRequest;
class ComputingResponse;
class PkModelCollection;


///
/// \brief The IComputingService class
/// This interface exposes the entry point for any computation. All information is embedded into
/// the classes ComputingRequest and ComputingResponse
class IComputingService : public Tucuxi::Common::Interface
{
public:
    virtual ~IComputingService() = default;

    ///
    /// \brief computes
    /// \param _request The request containing all information required for the computation
    /// \param _response The result of the computation
    /// \return ComputingResult::Ok if everything went well with the computation, another code else
    /// The response is a reference to a unique pointer that has to be allocated within compute()
    ///
    virtual ComputingStatus compute(
            const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response) = 0;

    ///
    /// \brief returns a description of the last error in case of failed computation
    /// \return A description of the last error
    /// This function should only be called if compute() returned something different from ComputingResult::Ok
    ///
    virtual std::string getErrorString() const = 0;

    ///
    /// \brief setPkModelCollection sets the Pk models collection to be used in further computations
    /// \param _collection A shared pointer to the Pk models collection
    /// By default, at creation, the ComputingComponent calls defaultPopulate() to create the Pk
    /// models collection. This function allows to override it with a custom collection.
    ///
    virtual void setPkModelCollection(std::shared_ptr<PkModelCollection> _collection) = 0;
};

} // namespace Core
} // namespace Tucuxi

#endif // ICOMPUTINGSERVICE_H
