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
#ifndef MDT_TT_TEST_NODE_UNIT_DATA_H
#define MDT_TT_TEST_NODE_UNIT_DATA_H

#include "mdtSqlRecord.h"
///#include "mdtClUnitConnectionData.h"
#include <QSqlDatabase>

/*! \brief Storage class for test node unit
 */
class mdtTtTestNodeUnitData : public mdtSqlRecord
{
 public:

  /*! \brief Construct a empty test node data container
   */
  mdtTtTestNodeUnitData();

  /*! \brief Construct a test node data based on a QSqlRecord
   *
   * \pre record must contain all fields from TestNodeUnit_tbl
   */
  mdtTtTestNodeUnitData(const QSqlRecord & record);

  /*! \brief Setup fields from TestNodeUnit_tbl
   *
   * \param setupConnectionData If true, internal unit connection data will also be setup.
   *
   * Note: unit data part is allways setup.
   */
  bool setup(const QSqlDatabase & db/**, bool setupConnectionData*/);

  /*! \brief Clear values
   */
  void clearValues();

  /*! \brief Set unit data part
   *
   * Will also update Unit_Id_FK_PK field.
   *
   * \pre data must contain all fields from Unit_tbl
   */
  void setUnitData(const mdtSqlRecord & data);

  /*! \brief Get unit data part
   */
  inline mdtSqlRecord unitData() const { return pvUnitData; }

  /*! \brief Set test connection data
   *
   * Will also update TestConnection_Id_FK
   */
  ///void setTestConnectionData(const mdtClUnitConnectionData & data);

  /*! \brief Get test connection data
   */
  ///inline mdtClUnitConnectionData testConnectionData() const { return pvTestConnectionData; }

 private:

  mdtSqlRecord pvUnitData;  // Unit_tbl data part
  ///mdtClUnitConnectionData pvTestConnectionData;
};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_DATA_H
