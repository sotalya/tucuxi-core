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


#ifndef QUERYLOGGER_H
#define QUERYLOGGER_H

#include <string>

#include "tucucommon/component.h"

namespace Tucuxi {
namespace Query {

class IQueryLogger : public Tucuxi::Common::Interface
{
public:
    virtual ~IQueryLogger() = default;

    virtual std::string getFolderPath(void) = 0;

    virtual void saveQuery(const std::string& _queryString, const std::string& _queryID) = 0;
};

class QueryLogger : public Tucuxi::Common::Component, public IQueryLogger
{
public:
    static Tucuxi::Common::Interface* createComponent(const std::string& _folderPath);

    QueryLogger(const std::string& _folderPath);

    ~QueryLogger() override;

    void saveQuery(const std::string& _queryString, const std::string& _queryID) override;

    std::string getFolderPath(void) override;

protected:
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override;

private:
    std::string m_folderPath;
};

} // namespace Query
} // namespace Tucuxi

#endif // QUERYLOGGER_H
