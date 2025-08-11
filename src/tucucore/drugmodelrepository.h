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


#ifndef TUCUXI_CORE_DRUGMODELREPOSITORY_H
#define TUCUXI_CORE_DRUGMODELREPOSITORY_H

#include <string>
#include <vector>

#include "tucucommon/component.h"

namespace Tucuxi {
namespace Core {

class DrugModel;

class IDrugModelRepository : public Tucuxi::Common::Interface
{
public:
    virtual ~IDrugModelRepository() = default;

    virtual DrugModel* getDrugModelById(std::string _drugModelId) = 0;

    virtual std::vector<DrugModel*> getDrugModelsByDrugId(std::string _drugId) = 0;
};

///
/// \brief The DrugModelRepository class
///
/// The role of the DrugModelRepository is to load and store DrugModel objects
/// so as to offer them when required.
///
/// Getting a drug model can be done by Id:
///
/// \code
/// DrugModel *theModel = drugModelRepository->getDrugModelById("theDrugModelId");
/// \endcode
///
/// Or by drug Id:
///
/// \code
/// std::vector<DrugModel *> theModels = drugModelRepository->getDrugById("theDrugId");
/// \endcode
///
/// In that second case, a vector of DrugModel objects is returned. Each DrugModel is
/// applicable on the drug represented by the DrugId.
///
/// There are two ways of populating the repository:
/// 1. By loading drug model files (.tdd) from a specific folder
/// 2. By setting folders in which the repository will look for specific drug models when required
///
/// For the first option, simply call loadFolder(<folderpath>) on the folders containing the .tdd files.
/// It will directly load all the valid drug models found in that directory. Please note that this loading
/// is not done recursively.
///
/// For the second option, the concept is to call addFolderPath(<folderpath>).
/// This function will simply store the folder path in a list, and the drug model is loaded only
/// when asked through getDrugModelById() or getDrugModelsByDrugId(). As by convention, the name
/// of the drug model file is <drugModelId>.tdd, the repository can try to find this file and load it
/// if it finds it. A model that has been loaded should then stay in the repository. It means that
/// the second time its Id is asked, then there will be no need to reload it.
///
/// The main difference between the two options is the computing time. With the first one,
/// some time is spent at the beginning to load all drug models. With the second one, no time is
/// lost at beginning, but some latency could be observed the first time a drug model is used.
///
/// For automated tests in command line it is suggested to use the second approach, as there is a good
/// chance that only a single drug model will be used. It will save processing time.
///
/// It can also be noticed that both approaches are compatible. For instance it is possible to load
/// a folder at the beginning, add other folders without loading. When getting a drug model, the
/// repository will look in the list of already loaded drug models, and if it is not found, then
/// it will try to load a file from one of the available folders.
///
class DrugModelRepository : public Tucuxi::Common::Component, public IDrugModelRepository
{
public:
    static Tucuxi::Common::Interface* createComponent();

    DrugModelRepository();

    ~DrugModelRepository() override;

    //bool initialize() override;

    ///
    /// \brief adds a folder path to the list of available folders
    /// \param _folderPath The folder path
    ///
    /// This function does not load the drug models, but only adds
    /// the path into the list of potential drug model files paths
    ///
    void addFolderPath(const std::string& _folderPath);

    ///
    /// \brief loads all drug models from a folder
    /// \param _folder The folder path
    ///
    /// This function looks for all *.tdd files in that folder and
    /// loads all these files to populate the list of available drug models.
    ///
    void loadFolder(const std::string& _folder);

    ///
    /// \brief loads a drug mode file
    /// \param _fileName The file path
    ///
    /// This function loads the drug model from the specified file.
    ///
    //    void loadFile(std::string _fileName);

    ///
    /// \brief loads a drug mode file
    /// \param _fileName The file path
    /// \return A pointer to the drug model, nullptr if not found
    /// This function loads the drug model from the specified file.
    ///
    DrugModel* loadFile(const std::string& _fileName);

    ///
    /// \brief adds a drug model to the repository
    /// \param _drugModel A valid drug model
    ///
    /// The ownership of the drug model is given to the repository who
    /// is then responsible to delete in the repository destructor.
    ///
    void addDrugModel(DrugModel* _drugModel);

    ///
    /// \brief get a drug model from its Id
    /// \param _drugModelId The Id of the drug model of interest
    /// \return A pointer to the drug model, nullptr if not found
    ///
    /// This function first tries to find the drug model in its list of models
    /// already loaded. If it is not there, it tries to directly load a file
    /// called <_drugModelId>.tdd from one of the paths that were added with
    /// addFolderPath(). If it is still not there, then it tries to load all the
    /// files from all available paths.
    ///
    DrugModel* getDrugModelById(std::string _drugModelId) override;

    ///
    /// \brief get a vector of drug models that correspond to a specific drug
    /// \param _drugId The Id of the drug
    /// \return A vector of drug models that correspond to the drug Id
    ///
    /// This function first ensures that all drug models are loaded from all the
    /// folder patsh available. Then it looks for all the compatible drug models
    /// and adds them in the resulting vector.
    ///
    std::vector<DrugModel*> getDrugModelsByDrugId(std::string _drugId) override;

protected:
    /// \brief The list of loaded drug models
    std::vector<DrugModel*> m_drugModels;

    /// \brief The list of not loaded drug models
    std::vector<std::string> m_drugPaths;

    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override;
};


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_DRUGMODELREPOSITORY_H
