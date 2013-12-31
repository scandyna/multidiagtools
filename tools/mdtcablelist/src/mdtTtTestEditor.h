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
#ifndef MDT_TT_TEST_EDITOR_H
#define MDT_TT_TEST_EDITOR_H

#include "mdtClEditor.h"

/*! \brief Editor fot tests
 */
class mdtTtTestEditor : public mdtClEditor
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestEditor(QObject *parent, QSqlDatabase db);

 public slots:

  /*! \brief Add a test item
   */
  void addTestItem();

  /*! \brief Edit a test item
   */
  void editTestItem();

  /*! \brief Remove a test item
   */
  void removeTestItem();

  /*! \brief Generate test nodue unit setup
   */
  void generateTestNodeUnitSetupList();

  /*! \brief Remove test node unit setup
   */
  ///void removeTestNodeUnitSetup();

 private:

  /*! \brief Select a test link
   */
  ///QVariant selectTestLink(const QString & msg);

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup test table
   */
  bool setupTestTable();

  /*! \brief Setup test link table
   */
  bool setupTestLinkTable();

  /*! \brief Setup used nodes table
   */
  bool setupTestNodeTable();

  /*! \brief Setup test node unit setup table
   */
  bool setupTestNodeUnitSetupTable();

  Q_DISABLE_COPY(mdtTtTestEditor);
};

#endif // #ifndef MDT_TT_TEST_EDITOR_H
