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
#ifndef MDT_SQL_FORM_H
#define MDT_SQL_FORM_H

#include "mdtError.h"
#include <QWidget>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QHash>
#include <QSqlError>
#include <QIcon>

class mdtAbstractSqlWidget;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QTabWidget;
class QVBoxLayout;

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
 * This class was made to have less code for common database GUI .
 *
 * The mainSqlWidget is based on a mdtSqlFormWidget , and is placed
 *  on the upper part of the form .
 *  When adding child table with addChildTable() , a instance of mdtSqlTableWidget is created,
 *  and placed to a QTabWidget .
 *  It is also possible to add a non database related widget, using addChildWidget() .
 *  In this case, the given widget is simply added to the internal QTabWidget .
 */
class mdtSqlForm : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Given database will be used whenn
   *  adding new tables with setMainTable(const QString&, const QString&)
   *  and addChildTable(const QString&, const QString&) .
   *
   * database() will also return this version .
   */
  mdtSqlForm(QWidget *parent, QSqlDatabase db);

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
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...)
   */
  bool setMainTable(const QString &tableName, const QString &userFriendlyTableName);

  /*! \brief Set the main (or parent) table of the form
   *
   * Note: be shure that your Ui::setupUi() was called on widget
   *        returned by mainSqlWidget() before.
   *
   * This method is only useful if different tables have to act on different databases .
   *
   * \param tableName Table on witch the form must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use (pass a QSqlDatabase() if only one database is used).
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...)
   */
  bool setMainTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db);

  /*! \brief Add a child table to the form
   *
   * \param tableName Table on witch the new (internally created) widget must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...).
   *          False can also be returned if main table was not set (see setTable() ).
   */
  bool addChildTable(const QString &tableName, const QString &userFriendlyTableName);

  /*! \brief Add a child table to the form
   *
   * This method is only useful if different tables have to act on different databases .
   *
   * \param tableName Table on witch the new (internally created) widget must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use (pass a QSqlDatabase() if only one database is used).
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...).
   *          False can also be returned if main table was not set (see setTable() ).
   */
  bool addChildTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db);

  /*! \brief Add a widget to the internal QTabWidget
   *
   * Can be used if a widget that is not related to a database table must be placed
   *  in the QTabWidget .
   *
   * Note: the given widget will be reparented, and also deleted when the form is destroyed .
   */
  void addChildWidget(QWidget *widget, const QString & label, const QIcon & icon = QIcon());

  /*! \brief Add a relation between main table and a child table
   *
   * \return True on succes,
   *          false if childTableName or a field does not exit.
   */
  bool addRelation(const QString &parentFieldName, const QString &childTableName, const QString &childFieldName, const QString &operatorWithPreviousItem = "AND");

  /*! \brief Start internal state machines
   *
   * Will start state machine of each contained SQL widget.
   *  This method is called by mdtSqlWindow and mdtSqlDialog.
   */
  void start();

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

  /*! \brief Get database that was given by constructor
   */
  QSqlDatabase database();

  /*! \brief Get database that acts on given table name
   *
   * \return QSqlDatabase object that acts on given table,
   *          or database() if table was not found .
   */
  QSqlDatabase database(const QString &tableName);

  /*! \brief Get model that acts on given table name
   *
   * \return QSqlTableModel object or a null pointer if table was not found.
   */
  QSqlTableModel *model(const QString &tableName);

  /*! \brief Call select on model that acts on main table name
   *
   * \return True on success, false on error.
   *          On failure, the last error is avaliable with lastError()
   */
  bool select();

  /*! \brief Call select on model that acts on given table name
   *
   * \return True on success, false if tableName was not found or other error.
   *          On failure, the last error is avaliable with lastError()
   */
  bool select(const QString &tableName);

  /*! \brief Set main table's filter
   *
   * \param fieldName Field that must match data
   * \param matchData Match data
   *
   * Internally, a SQL statement is generated linke: fieldName = matchData
   */
  bool setMainTableFilter(const QString & fieldName, const QVariant & matchData);

  /*! \brief Get last error of model that acts on tableName
   *
   * \return A valid QSqlError if table was found and a error occured.
   */
  ///QSqlError lastSqlError(const QString &tableName);

  /*! \brief Get last error
   */
  mdtError lastError() const;

  /*! \brief Display last error in a message box
   */
  void displayLastError();

  /*! \brief Get the row count for given table name
   *
   * Note that SQL widget related to given tableName is searched
   *  at each request. If several access to rowCount is needed,
   *  it can be better to get a pointer to related model with model() ,
   *  and call his rowCount() method.
   *
   * \return Row count, or value < 0 on error (f.ex. table not found).
   */
  int rowCount(const QString &tableName);

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

  /*! \brief Set the first record that matches given criteria as current record (index).
   *
   * This method will act on main table witch was set with setMainTable().
   *
   * \param fieldName Field that must contain searched value
   * \param value Value that must matche
   * \return True if matching record was found
   *          (in this case, setCurrentIndex() is called on sql widget witch acts on main table to update currentRow),
   *          false else.
   */
  bool setCurrentRecord(const QString &fieldName, const QVariant &value);

  /*! \brief Set (update) data of current row (record) for given table and field name
   *
   * Note that SQL widget related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related widget with sqlWidget() ,
   *  and call his setCurrentData() method.
   */
  bool setCurrentData(const QString &tableName, const QString &fieldName, const QVariant &data, bool submit = true);

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

  /*! \brief Call insert method on mainSqlWidget
   */
  void insert();

  /*! \brief Setup tables
   *
   * Must be re-implemented in subclass, default implementation does nothing .
   */
  virtual bool setupTables();

  /*! \brief Check if all data are saved
   *
   * Will call allDataAreSaved() on mainSqlWidget.
   *
   * \return true if all data are saved, false if a SQL widget has pending data.
   */
  bool allDataAreSaved();

 protected:

  /*! \brief Get lastError ogject for write acces
   *
   * Subclass can use this method to set lastError
   */
  mdtError & lastErrorW();

  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtSqlForm);

  QHash<QString, mdtSqlRelation*> pvRelationsByChildTableName;
  mdtSqlFormWidget *pvMainSqlWidget;
  QTabWidget *pvChildsTabWidget;
  QVBoxLayout *pvMainLayout;
  QSqlDatabase pvDatabase;
};

#endif  // #ifndef MDT_SQL_FORM_H
