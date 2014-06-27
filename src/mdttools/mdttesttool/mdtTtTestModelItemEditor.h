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
#ifndef MDT_TT_TEST_MODEL_ITEM_EDITOR_H
#define MDT_TT_TEST_MODEL_ITEM_EDITOR_H

#include "mdtSqlForm.h"
#include <QVariant>

/*! \brief Test item editor
 */
class mdtTtTestModelItemEditor : public mdtSqlForm
{
 Q_OBJECT

  public:

  /*! \brief Constructor
   */
  ///mdtTtTestModelItemEditor(QObject *parent, QSqlDatabase db);
  mdtTtTestModelItemEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup all tables
   */
  bool setupTables();

  /*! \brief Set test model ID
   */
  void setTestModelId(const QVariant & id);

  /*! \brief Set sequence number
   */
  void setSequenceNumber(const QVariant & seqNumber);

 private slots:

  /*! \brief Add a test link
   */
  void addTestLink();

  /*! \brief Remove test links
   */
  void removeTestLinks();

  /*! \brief Add a test node unit to setup list
   */
  void addNodeUnit();

  /*! \brief Setup a test node unit
   */
  void setupNodeUnit();

  /*! \brief Remove test node units from setup list
   */
  void removeNodeUnits();

  /*! \brief Set test link
   */
  ///void setTestLink();

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

  /*! \brief Setup test item table
   */
  bool setupTestItemTable();

  /*! \brief Setup test link table
   */
  bool setupTestLinkTable();

  /*! \brief Setup test node unit setup table
   */
  bool setupTestNodeUnitSetupTable();

  Q_DISABLE_COPY(mdtTtTestModelItemEditor);
};

#endif // #ifndef MDT_TT_TEST_MODEL_ITEM_EDITOR_H
