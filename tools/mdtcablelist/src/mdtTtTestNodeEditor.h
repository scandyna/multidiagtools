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
#ifndef MDT_TT_TEST_NODE_EDITOR_H
#define MDT_TT_TEST_NODE_EDITOR_H

#include "mdtClEditor.h"
#include <QSqlDatabase>
#include <QList>

/*! \brief This class provides a test node editor
 */
class mdtTtTestNodeEditor : public mdtClEditor
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeEditor(QObject *parent, QSqlDatabase db);

 private slots:

  /*! \brief Set base vehicle type
   */
  void setBaseVehicleType();

  /*! \brief Add test node units
   */
  void addUnits();

 private:

  /*! \brief Select a TestNodeUnit type
   */
  QVariant selectTestNodeUnitType();

  /*! \brief Select a unit to use
   */
  QVariant selectUnitToUseAsTestNode(const QVariant & vehicleTypeId);

  /*! \brief Select a unit connection
   */
  QVariant selectUnitConnection(const QVariant & unitId);

  /*! \brief Select a list of unit IDs from given unit ID list
   */
  QList<QVariant> selectUnitIdList(const QList<QVariant> & unitIdList);

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup TestNode table (main form)
   */
  bool setupTestNodeTable();

  /*! \brief Setup TestNodeUnit table
   */
  bool setupTestNodeUnitTable();

  Q_DISABLE_COPY(mdtTtTestNodeEditor);
};

#endif // #ifndef MDT_TT_TEST_NODE_EDITOR_H
