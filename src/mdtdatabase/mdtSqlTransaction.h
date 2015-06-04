/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_TRANSACTION_H
#define MDT_SQL_TRANSACTION_H

#include "mdtError.h"
#include <QSqlDatabase>

/*! \brief Handle for SQL transaction
 */
class mdtSqlTransaction
{
 public:

  /*! \brief Constructor
   */
  mdtSqlTransaction(QSqlDatabase db);

  /*! \brief Destructor
   *
   * If a trasanction was started with begin(),
   *  and not ended with commit(),
   *  rollback() will be called.
   */
  ~mdtSqlTransaction();

  /*! \brief Begin a transaction
   */
  bool begin();

  /*! \brief Begin a transaction if not allready started
   */
  bool beginIfNotStarted()
  {
    if(pvIsStarted){
      return true;
    }
    return begin();
  }

  /*! \brief Check if a transaction was started
   *
   * \return true if begin was called (and successfull)
   */
  bool isStarted() const
  {
    return pvIsStarted;
  }

  /*! \brief Commit transaction
   */
  bool commit();

  /*! \brief Rollback transaction
   */
  bool rollback();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtSqlTransaction);

  QSqlDatabase pvDatabase;
  bool pvIsStarted;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_TRANSACTION_H
