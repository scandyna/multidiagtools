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
#ifndef MDT_SQL_TABLE_WIDGET_H
#define MDT_SQL_TABLE_WIDGET_H

#include "mdtAbstractSqlWidget.h"

#include <QModelIndex>

class QTableView;
class QHBoxLayout;
class QPushButton;

/*! \brief Table view based SQL widget
 *
 * Internally, QTableView is used.
 *  This class add some common needed methods,
 *  like insertion, removing, ...
 */
class mdtSqlTableWidget : public mdtAbstractSqlWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlTableWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlTableWidget();

  /*! \brief Get the current row
   */
  int currentRow() const;

  /*! \brief Enable local navigation
   *
   * Will build a navigation bar
   *  with |<< < > >>| buttons.
   *
   * In normal way, when using mdtSqlWindow,
   *  local navigation is not needed.
   *  In master/detail forms, if this widget
   *  is a details (child) widget and navigation
   *  is needed, then call this method.
   */
  void enableLocalNavigation();

  /*! \brief Enable local edition
   *
   * Will add insert/revert/save/delete buttons.
   *
   * In normal way, when using mdtSqlWindow,
   *  local navigation is not needed.
   *  In master/detail forms, if this widget
   *  is a details (child) widget and edition
   *  is needed, then call this method.
   */
  void enableLocalEdition();

 private slots:

  /*! \brief Emit dataEdited()
   */
  void onDataChanged(const QModelIndex &, const QModelIndex &);

  /*! \brief Does some tasks when entering new row
   */
  void onCurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);

 private:

  /*! \brief Set model
   *
   * Set the model that handle database.
   *  (See Qt's QSqlTableModel documentation for details).
   *
   * \pre model must be a valid pointer.
   */
  void doSetModel(QSqlTableModel *model);

  bool doSubmit();

  bool doRevert();

  bool doInsert();

  bool doSubmitNewRow();

  bool doRevertNewRow();

  bool doRemove();

  /*! \brief Set first record as current record
   */
  void toFirst();

  /*! \brief Set last record as current record
   */
  void toLast();

  /*! \brief Set previous record as current record
   */
  void toPrevious();

  /*! \brief Set next record as current record
   */
  void toNext();

  Q_DISABLE_COPY(mdtSqlTableWidget);

  QTableView *pvTableView;
  QHBoxLayout *pvNavigationLayout;
  // Navigation buttons
  QPushButton *pbNavToFirst;
  QPushButton *pbNavToPrevious;
  QPushButton *pbNavToNext;
  QPushButton *pbNavToLast;
  // Edition buttons
  QPushButton *pbInsert;
  QPushButton *pbSubmit;
  QPushButton *pbRevert;
  QPushButton *pbRemove;
};

#endif  // #ifndef MDT_SQL_TABLE_WIDGET_H
