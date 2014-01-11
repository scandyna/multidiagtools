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
#ifndef MDT_TT_TEST_NODE_UNIT_EDITOR_H
#define MDT_TT_TEST_NODE_UNIT_EDITOR_H

#include "mdtSqlForm.h"
#include "ui_mdtTtTestNodeUnitEditor.h"
#include <QSqlDatabase>

class QWidget;

/*! \brief Class to edit test node unit
 */
class mdtTtTestNodeUnitEditor : public mdtSqlForm, Ui::mdtTtTestNodeUnitEditor
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeUnitEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private:

  Q_DISABLE_COPY(mdtTtTestNodeUnitEditor);

  /*! \brief Setup test node unit table
   */
  bool setupTestNodeUnitTable();

  /*! \brief Setup widget mapping with test node table
   */
  bool setupTestNodeTableWidgetMapping();

  /*! \brief Setup widget mapping with unit table
   */
  bool setupUnitTableWidgetMapping();

  /*! \brief Setup widget mapping with type table
   */
  bool setupTypeTableWidgetMapping();
};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_EDITOR_H
