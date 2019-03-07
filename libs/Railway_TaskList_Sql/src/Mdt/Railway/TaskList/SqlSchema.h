/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_RAILWAY_TASK_LIST_SQL_SCHEMA_H
#define MDT_RAILWAY_TASK_LIST_SQL_SCHEMA_H

#include "Mdt/Error.h"
#include "MdtRailway_TaskList_SqlExport.h"
#include <QObject>
#include <QSqlDatabase>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief SQL schema
   */
  class MDT_RAILWAY_TASKLIST_SQL_EXPORT SqlSchema : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    SqlSchema(QObject *parent = nullptr);

    /*! \brief Create schema
     *
     * \pre \a db must be valid and open
     */
    bool create(const QSqlDatabase & db);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    void setLastError(const Mdt::Error & error);

    Mdt::Error mLastError;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_SQL_SCHEMA_H
