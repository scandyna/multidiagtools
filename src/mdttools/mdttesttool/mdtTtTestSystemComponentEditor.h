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
#ifndef MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H
#define MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>
#include <QModelIndex>
#include <QSqlRecord>
#include <QList>
#include <QLocale>

class QWidget;

/*! \brief Test system component editor
 */
class mdtTtTestSystemComponentEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestSystemComponentEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Set test system component type
   */
  void setType();

  /*! \brief Add test system assignation
   */
  void addTestSystemAssignation();

  /*! \brief Remove test system assignation
   */
  void removeTestSystemAssignation();

  /*! \brief Create a new test system unit
   */
  void createUnit();

  /*! \brief Edit selected test system unit
   */
  void editUnit(const QSqlRecord & record);

  /*! \brief Edit current test system unit
   */
  void editUnit();

  /*! \brief Remove selected test system units
   */
  void removeUnits();

 private:

  /*! \brief Setup TestSystemComponent_tbl part
   */
  bool setupTestSystemComponentTable();

  /*! \brief Setup TestSystemOfComponent_view
   */
  bool setupTestSystemAssignationTable();

  /*! \brief Setup TestSystemUnit_view table
   */
  bool setupTestSystemUnitTable();

  Q_DISABLE_COPY(mdtTtTestSystemComponentEditor);
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H
