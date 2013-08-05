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
#ifndef MDT_SQL_FORM_H
#define MDT_SQL_FORM_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QHash>
#include <QSqlError>

class mdtAbstractSqlWidget;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;


/*! \brief Base class for high level SQL widgets API
 *
 * To create SQL GUI's, some clases are available:
 *  - mdtSqlFormWidget
 *  - mdtSqlTableWidget
 *  - mdtSqlRelation
 *
 * They help to deal with several common problems,
 *  but they need several code (creating objects,
 *  configure them, handle memory, ...).
 *
 * This class was made to have less code for common database GUI.
 *
 * You should not use this class directly, but better one
 *  of its derivate.
 *
 * \sa mdtSqlFormWindow
 * \sa mdtSqlFormDialog
 */
class mdtSqlForm : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlForm(QObject *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtSqlForm();

  /*! \brief Get the main SQL widget
   *
   * Use this method to get the widget to give
   *  as argument to setupUi() method of your Ui widget.
   *
   * \post Returns a valid object.
   */
  mdtSqlFormWidget *mainSqlWidget();

  /*! \brief Set the main (or parent) table of the form
   *
   * Note: be shure that your Ui::setupUi() was called on widget
   *        returned by mainSqlWidget() before.
   *
   * \param tableName Table on witch the form must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use (pass a QSqlDatabase() if only one database is used).
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...)
   */
  bool setTable(const QString &tableName, const QString &userFriendlyTableName = "", QSqlDatabase db = QSqlDatabase());

  /*! \brief Add a child table to the form
   *
   * \param tableName Table on witch the new (internally created) widget must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use (pass a QSqlDatabase() if only one database is used).
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...).
   *          False can also be returned if main table was not set (see setTable() ).
   */
  virtual bool addChildTable(const QString &tableName, const QString &userFriendlyTableName = "", QSqlDatabase db = QSqlDatabase());

  /*! \brief Add a relation between main table and a child table
   *
   * \return True on succes,
   *          false if childTableName or a field does not exit.
   */
  bool addRelation(const QString &parentFieldName, const QString &childTableName, const QString &childFieldName);

  /*! \brief Get instance of the stored SQL widget related to a table name
   *
   * Returns a null pointer if widget related to tableName was not found.
   */
  mdtAbstractSqlWidget *sqlWidget(const QString &tableName);

  /*! \overload sqlWidget()
   */
  mdtSqlFormWidget *sqlFormWidget(const QString &tableName);

  /*! \overload sqlWidget()
   */
  mdtSqlTableWidget *sqlTableWidget(const QString &tableName);

  /*! \brief Get model that acts on given table name
   *
   * \return QSqlTableModel object or a null pointer if table was not found.
   */
  QSqlTableModel *model(const QString &tableName);

  /*! \brief Call select on model that acts on given table name
   *
   * \return True on success, false if tableName was not found or other error.
   *          On failure, the last error is avaliable with lastError()
   */
  bool select(const QString &tableName);

  /*! \brief Get last error of model that acts on tableName
   *
   * \return A valid QSqlError if table was found and a error occured.
   */
  QSqlError lastError(const QString &tableName);

  /*! \brief Get the current row of given table name
   *
   * Current row can be the currently selected row
   *  (case of a table view),
   *  or simply the current row that is displayed in a form view.
   *
   * Note that SQL widget related to given tableName is searched
   *  at each request. If several access to currentRow is needed,
   *  it can be better to get a pointer to related widget with sqlWidget() ,
   *  and call his currentRow() method.
   *
   * \return Current row, or value < 0 on error (f.ex. table not found).
   */
  int currentRow(const QString &tableName);

  /*! \brief Get current data for given table and field name
   *
   * Note that SQL widget related to given tableName is searched
   *  at each request. If several access to currentData is needed,
   *  it can be better to get a pointer to related widget with sqlWidget() ,
   *  and call his currentData() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table or field not found).
   */
  QVariant currentData(const QString &tableName, const QString &fieldName);

  /*! \brief Get data for given table , row and field name
   *
   * Note that SQL widget related to given tableName is searched
   *  at each request. If several access to data is needed,
   *  it can be better to get a pointer to related widget with sqlWidget() ,
   *  and call his data() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table, row or field not found).
   */
  QVariant data(const QString &tableName, int row, const QString &fieldName);

 protected:

  /*! \brief Add given child widget
   */
  virtual void addChildWidget(mdtAbstractSqlWidget *widget) = 0;

 private:

  Q_DISABLE_COPY(mdtSqlForm);

  QHash<QString, mdtSqlRelation*> pvRelationsByChildTableName;
  mdtSqlFormWidget *pvMainSqlWidget;
};

#endif  // #ifndef MDT_SQL_FORM_H
