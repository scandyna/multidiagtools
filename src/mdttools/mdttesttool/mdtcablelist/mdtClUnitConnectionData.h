/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_CL_UNIT_CONNECTION_DATA_H
#define MDT_CL_UNIT_CONNECTION_DATA_H

#include "mdtSqlRecord.h"
#include "mdtClArticleConnectionData.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Data container class for unit connection data
 *
 * Permit to echange data with UnitConnection_tbl.
 *
 * This class was made for data edition.
 *  It is also mandatory that fields matches UnitConnection_tbl.
 *  To get data from a view, witch also can miss fields, 
 *  or have different field name, considere QSqlRecord.
 */
class mdtClUnitConnectionData : public mdtSqlRecord
{
 public:

  /*! \brief Construct a empty mdtClUnitConnectionData
   */
  mdtClUnitConnectionData();

  /*! \brief Contruct a mdtClUnitConnectionData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from UnitConnection_tbl must exist in record
   */
  mdtClUnitConnectionData(const QSqlRecord & record);

  /*! \brief Setup fields from UnitConnection_tbl
   *
   * \param setupAcd If true, fields from article connection part are also added.
   */
  bool setup(const QSqlDatabase & db, bool setupAcd);

  /*! \brief Access article connection data (RD)
   */
  const mdtClArticleConnectionData & articleConnectionData() const;

  /*! \brief Access article connection data (WR)
   */
  mdtClArticleConnectionData & articleConnectionData();

 private:

  mdtClArticleConnectionData pvArticleConnectionData;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_DATA_H
