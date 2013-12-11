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

 private:

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup TestNode table (main form)
   */
  bool setupTestNodeTable();

  Q_DISABLE_COPY(mdtTtTestNodeEditor);
};

#endif // #ifndef MDT_TT_TEST_NODE_EDITOR_H
