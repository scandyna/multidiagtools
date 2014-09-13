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
#ifndef MDT_SQL_TABLE_VIEW_CONTROLLER_H
#define MDT_SQL_TABLE_VIEW_CONTROLLER_H

#include "mdtAbstractSqlTableController.h"

class QTableView;

/*! \brief
 */
class mdtSqlTableViewController : public mdtAbstractSqlTableController
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlTableViewController(QObject* parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlTableViewController();

  /*! \brief Set table view to control
   */
  void setTableView(QTableView *tv);

  /*! \brief Get current row
   */
  int currentRow() const;

 private slots:

  /*! \brief Will set pvTableView pointer to Null
   */
  void onTableViewDestroyed(QObject *obj);

 private:

  /*! \brief Current row changed event
   */
  void currentRowChangedEvent(int row);

  /*! \brief Table model set event
   */
  void modelSetEvent();

  /*! \brief Submit current row to model
   */
  bool doSubmit();

  /*! \brief Revert current row from model
   */
  bool doRevert();

  /*! \brief Insert a new row to model
   */
  bool doInsert();

  /*! \brief Submit new row to model
   */
  bool doSubmitNewRow();

  /*! \brief Revert new row
   */
  bool doRevertNewRow();

  /*! \brief Remove current row from model
   */
  bool doRemove();

  Q_DISABLE_COPY(mdtSqlTableViewController);

  QTableView *pvTableView;
};

#endif // #ifndef MDT_SQL_TABLE_VIEW_CONTROLLER_H
