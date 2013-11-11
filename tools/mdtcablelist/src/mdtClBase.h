/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_CL_BASE_H
#define MDT_CL_BASE_H

#include "mdtError.h"
#include <QSqlDatabase>


/*! \brief Base class for helper class
 *
 * To edit entities (linke Article, Unit, ...) ,
 *  dedicated classes helps for interaction with database .
 *
 * This class is a base that regroups common methods .
 */
class mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtClBase(QSqlDatabase db);

  /*! \brief Destructor
   */
  virtual ~mdtClBase();

  /*! \brief Get database instance
   */
  QSqlDatabase database();

  /*! \brief Get last error
   */
  mdtError lastError();

 protected:

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool commitTransaction();

  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtClBase);

  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDTCLBASE_H
