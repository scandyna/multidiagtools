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
#ifndef MDT_TT_TEST_ITEM_EDITOR_H
#define MDT_TT_TEST_ITEM_EDITOR_H


#include "mdtClEditor.h"

/*! \brief Test item editor
 */
class mdtTtTestItemEditor : public mdtClEditor
{
 Q_OBJECT

  public:

  /*! \brief Constructor
   */
  mdtTtTestItemEditor(QObject *parent, QSqlDatabase db);

 private slots:

  /*! \brief Set test link
   */
  void setTestLink();

  /*! \brief Generate test nodue unit setup
   */
  void generateTestNodeUnitSetup();

  /*! \brief Remove test node unit setup
   */
  void removeTestNodeUnitSetup();

 private:

  /*! \brief Select a test link
   */
  QVariant selectTestLink(const QString & bus);

  /*! \brief Setup all tables
   */
  bool setupTables();

  /*! \brief Setup test item table
   */
  bool setupTestItemTable();

  /*! \brief Setup test link table
   */
  bool setupTestLinkTable();

  /*! \brief Setup test node unit setup table
   */
  bool setupTestNodeUnitSetupTable();

  Q_DISABLE_COPY(mdtTtTestItemEditor);
};

#endif // #ifndef MDT_TT_TEST_ITEM_EDITOR_H
