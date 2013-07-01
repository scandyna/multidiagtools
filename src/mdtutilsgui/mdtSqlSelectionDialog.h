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

class QTableView;
class QSqlQueryModel;
class QLabel;

/*! \brief Provide a way for the user to select a row in a set of data
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

  /*! \brief Set model that contain data to display
   *
   * \pre model must be a valid pointer.
   */
  void setModel(QSqlQueryModel *model);

  /*! \brief Set a coulumn as hidden
   */
  void setColumnHidden(int column, bool hide);

  /*! \brief Set a coulumn as hidden
   */
  void setColumnHidden(const QString &fieldName, bool hide);

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
   */
  QList<QVariant> selectionResult();

 public slots:

  /*! \brief Overloads QDialog to tell selectionResult() that it must return a empty result
   */
  void reject();

 private:

  Q_DISABLE_COPY(mdtSqlSelectionDialog);

  QLabel *pvMessageLabel;
  QTableView *pvTableView;
  QSqlQueryModel *pvModel;
  QList<int> pvSelectionResultColumns;
  bool pvDialogRejected;
};

#endif  // #ifndef MDT_SQL_SELECTION_DIALOG_H
