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
#ifndef MDT_TT_TEST_DATA
#define MDT_TT_TEST_DATA

#include "mdtSqlRecord.h"
#include "mdtTtTestModelData.h"
#include <QSqlRecord>

/// \todo Check if not obselete ?
class mdtTtTestData : public mdtSqlRecord
{
 public:

  /*! \brief Construct a empty mdtTtTestData
   */
  mdtTtTestData();

  /*! \brief Contruct a mdtTtTestData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from Test_tbl must exist in record
   */
  mdtTtTestData(const QSqlRecord & record);

  /*! \brief Setup fields from TestModel_tbl
   */
  bool setup(const QSqlDatabase & db, bool setupModelData);

  /*! \brief Clear values
   */
  void clearValues();

  /*! \brief Set model data
   *
   * Will also update TestModel_Id_FK
   */
  void setModelData(const mdtTtTestModelData & data);

  /*! \brief Get model data
   */
  inline const mdtTtTestModelData & modelData() const { return pvModelData; }

 private:

  mdtTtTestModelData pvModelData;
};

#endif  // #ifndef MDT_TT_TEST_DATA
