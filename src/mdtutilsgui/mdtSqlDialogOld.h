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
#ifndef MDT_SQL_DIALOG_H
#define MDT_SQL_DIALOG_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVariant>
#include <QList>
#include <QPair>

class mdtAbstractSqlWidget;
class QTabWidget;
class QVBoxLayout;
class QPushButton;

/*! \brief Dialog for a database table view/editor
 *
 * This class offers some functionnality for the GUI part.
 *
 * It was designed to work with a subclass of mdtAbstractSqlWidget.
 */
class mdtSqlDialogOld : public QDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Will construct a empty window.
   *  Use setSqlWidget() to set central widget,
   *  setCurrentRow() to edit a existing row,
   *  insertRow() to create and edit a new row.
   */
  mdtSqlDialogOld(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlDialogOld();

  /*! \brief Set form/table view/editor
   *
   * Will make some signal/slot connection,
   *  then place sqlWidget in central area of dialog.
   *
   * \pre sqlWidget must be a valid pointer.
   */
  void setSqlWidget(mdtAbstractSqlWidget *sqlWidget);

  /*! \brief Add a child (details) widget
   *
   * \param widget Will be added to the childs tab
   * \param label Will be displayed in tab of tab widget
   *
   * \pre Parent (main) widget must be set with setSqlWidget() before using this method
   * \pre widget must be a valid pointer
   */
  void addChildWidget(QWidget *widget, const QString & label);

  /*! \brief Set row (record) to edit (in main table)
   *
   * \pre Parent (main) widget must be set with setSqlWidget() before using this method
   */
  void setCurrentRow(int row);

  /*! \brief Set the first record that matches given criteria as row (record) to edit (in main table)
   *
   * \param fieldName Field that must contain searched value
   * \param matchData Value that must matche
   * \return True if matching record was found
   * \pre Parent (main) widget must be set with setSqlWidget() before using this method
   */
  bool setCurrentRow(const QString & fieldName, const QVariant & matchData);

  /*! \brief Not implemented yet
   *
   * \todo Implement in mdtAbstractSqlWidget and here
   */
  bool setCurrentRow(const QList<QPair<QString, QVariant> > & matchList);

 public slots:

  /*! \brief Overloads QDialog::accept()
   */
  void accept();

  /*! \brief Overloads QDialog::reject()
   */
  void reject();

 private:

  Q_DISABLE_COPY(mdtSqlDialogOld);

  mdtAbstractSqlWidget *pvMainSqlWidget;
  QVBoxLayout *pvMainLayout;
  int pvCentralWidgetIndexInMainLayout;
  QPushButton *pbSubmit;
  QPushButton *pbRevert;
  QTabWidget *pvChildsTabWidget;
};

#endif  // #ifndef MDT_SQL_DIALOG_H
