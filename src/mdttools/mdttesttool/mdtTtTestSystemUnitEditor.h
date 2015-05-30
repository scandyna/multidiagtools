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
#ifndef MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H
#define MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>
#include <QList>
#include <QLocale>

class QWidget;

/*! \brief Test system unit editor
 */
class mdtTtTestSystemUnitEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestSystemUnitEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Set test system component
   */
  void setTestSystemComponent();

 private:

  /*! \brief Setup Unit_tbl + TestSystemUnit_tbl part
   */
  bool setupTestSystemUnitTables();

  Q_DISABLE_COPY(mdtTtTestSystemUnitEditor);
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H
