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
#ifndef MDT_CL_CONNECTION_TYPE_MODEL_H
#define MDT_CL_CONNECTION_TYPE_MODEL_H

#include "mdtError.h"
#include "mdtClConnectionTypeData.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>

class mdtClConnectionTypeModel : public QSqlQueryModel
{
 public:

  /*! \brief Constructor
   */
  mdtClConnectionTypeModel(QObject *parent, QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Constructor
   */
  mdtClConnectionTypeModel(QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Get row that contains given key
   *
   * \return Row of searched key or -1 if not found.
   */
  int row(const mdtClConnectionTypeKeyData & key);

  /*! \brief Get connection type key data for given row
   *
   * If row is < 0, a null key is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a null key is returned and lastError() contains error.
   * If all works fine, requested key is returned.
   */
  mdtClConnectionTypeKeyData keyData(int row);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClConnectionTypeModel);

  /*! \brief Check if QSqlQueryModel is in error (and store if true)
   */
  bool isInError();

  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_CONNECTION_TYPE_MODEL_H
