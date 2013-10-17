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
#ifndef MDT_SQL_SELECTION_DIALOG_H
#define MDT_SQL_SELECTION_DIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QStringList>
#include <QModelIndex>

class QTableView;
class QSqlQueryModel;
class QLabel;

/*! \brief Provide a way for the user to select row in a set of data
 */
class mdtSqlSelectionDialog : public QDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlSelectionDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlSelectionDialog();

  /*! \brief Set a message that will be displayed to the user
   */
  void setMessage(const QString &message);

  /*! \brief Set allow empty result
   *
   * If this flag is true, the dialog cannot be accepted if nothing was selected.
   *
   * Default is false.
   */
  void setAllowEmptyResult(bool allow);

  /*! \brief Set model that contain data to display
   *
   * \pre model must be a valid pointer.
   */
  void setModel(QSqlQueryModel *model, bool allowMultiSelection = false);

  /*! \brief Set a user friendly name for a column
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setHeaderData(const QString &fieldName, const QString &data);

  /*! \brief Set a coulumn as hidden
   */
  void setColumnHidden(int column, bool hide);

  /*! \brief Set a coulumn as hidden
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setColumnHidden(const QString &fieldName, bool hide);

  /*! \brief Select rows
   *
   * Will select rows for witch data in field (designated by fieldName)
   *  matches given matchData .
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void selectRows(const QString &fieldName, const QVariant &matchData);

  /*! \brief Set a list of column that will contain the selected data
   *
   * Note: if a given column is out of range, it will be simply ignored.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setSelectionResultColumns(const QList<int> &columns);

  /*! \brief Set a list of column that will contain the selected data
   *
   * Note: if a given field not exists, it will be simply ignored.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setSelectionResultColumns(const QStringList &fields);

  /*! \brief Add a column to the list of column that will contain the selected data
   *
   * Note: if a given field not exists, it will be simply ignored.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void addSelectionResultColumn(const QString &field);

  /*! \brief Return data that user has selected
   *
   * The data are these that was specified with setSelectionResultColumns() ,
   *  in the same order.
   *
   * If the user reject the dialog, a empty list is returned.
   *
   * This method works only for single row selection.
   */
  QList<QVariant> selectionResult();

  /*! \brief Return data that user has selected
   *
   * The index list only contains columns that where set with
   *  setSelectionResultColumns() or addSelectionResultColumn() .
   *
   * This method alos works for multiple rows selection.
   */
  QModelIndexList selectionResults();

  /*! \brief Return data that user has selected
   *
   * Will return data from selection results that match given row,
   *  for given fieldName .
   *
   * Note that row to give does not match given model's row,
   *  but is the row in selection.
   *
   * This method alos works for multiple rows selection,
   *  but is slower than selectionResults() .
   *
   * If row is out of bound or fieldName not exists,
   *  a invalid QVariant is returned.
   */
  QVariant selectedData(int row, const QString &fieldName);

 public slots:

  /*! \brief Overloads QDialog to to build selection results
   */
  void accept();

  /*! \brief Overloads QDialog to tell selectionResult() that it must return a empty result
   */
  void reject();

 private:

  /*! \brief Build the election results
   */
  void buildSelectionResults();

  Q_DISABLE_COPY(mdtSqlSelectionDialog);

  QLabel *pvMessageLabel;
  QTableView *pvTableView;
  QSqlQueryModel *pvModel;
  QList<int> pvSelectionResultColumns;
  QModelIndexList pvSelectionResults;
  bool pvAllowEmptyResult;            // If false, dialog cannot be accepted if nothing was selected
};

#endif  // #ifndef MDT_SQL_SELECTION_DIALOG_H
