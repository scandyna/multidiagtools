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
#ifndef MDT_CL_UNIT_EDITOR_H
#define MDT_CL_UNIT_EDITOR_H

#include <QSqlDatabase>

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;

/*! \brief Cable list's device editor
 */
class mdtClUnitEditor
{
 public:

  /*! \brief Contruct a cable editor
   */
  mdtClUnitEditor(QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClUnitEditor();

  /*! \brief Setup tables
   *
   * \param includeConnections If true, connection_tbl will be added as child widget.
   */
  bool setupTables(bool includeConnections);

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  void setupUi(mdtSqlWindow *window);

 private:

  Q_DISABLE_COPY(mdtClUnitEditor);

  ///Ui::mdtClUnitEditor pvUiUnitWidget;
  QSqlDatabase pvDatabase;
  mdtSqlFormWidget *pvUnitWidget;
  mdtSqlTableWidget *pvConnectionWidget;
  QSqlTableModel *pvUnitModel;
  QSqlTableModel *pvConnectionModel;
  mdtSqlRelation *pvRelation;
};

#endif  // #ifndef
