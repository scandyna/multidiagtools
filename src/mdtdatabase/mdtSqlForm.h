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
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlTableViewController.h"
#include "mdtSqlRelationInfo.h"
#include <QWidget>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QHash>
#include <QSqlError>
#include <QIcon>
#include <memory>

class mdtSqlTableWidget;
class QSqlTableModel;
class QTabWidget;
class QVBoxLayout;

/*! \brief Base class for high level SQL widgets API
 *
 * To create SQL GUI's, some clases are available:
 *  - mdtAbstractSqlTableController
 *  - mdtSqlDataWidgetController
 *  - mdtSqlTableViewController
 *  - mdtSqlTableWidget
 *
 * They help to deal with several common problems,
 *  but they need several code (creating objects,
 *  configure them, handle memory, ...).
 *
 * This class was made to have less code for common database GUI .
 *
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

  /*! \brief Set main table widget
   *
   *  If you have a UI file generated from QtDesigner,
   *  witch has a setupUi() method, simply use setMainTableUi()
   */
  void setMainTableWidget(QWidget *widget);

  /*! \brief Set main table UI
   *
   * Usefull for UI created with QtDesigner
   */
  template<typename T>
  void setMainTableUi()
  {
    T ui;
    setMainTableUi<T>(ui);
  }

  /*! \brief Set main table UI
   *
   * Usefull for UI created with QtDesigner
   */
  template<typename T>
  void setMainTableUi(T & ui)
  {
    QWidget *w = new QWidget;
    ui.setupUi(w);
    setMainTableWidget(w);
  }

  /*! \brief Set the main (or parent) table of the form
   *
   * Note: call setMainTableUi() or setMainTableWidget() before, else no data widget will be mapped.
   *        See mdtSqlDataWidgetController for details.
   *
   * \param tableName Table on witch the form must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param firstWidgetInTabOrder It's possible to specify the object name of the first widget in tab order.
   *                               This is then used by insertion to give this widget the focus.
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...)
   */
  bool setMainTable(const QString &tableName, const QString &userFriendlyTableName, const QString & firstWidgetInTabOrder = QString());

  /*! \brief Set the main (or parent) table of the form
   *
   * Note: call setMainTableUi() or setMainTableWidget() before, else no data widget will be mapped.
   *        See mdtSqlDataWidgetController for details.
   *
   * This method is only useful if different tables have to act on different databases .
   *
   * \param tableName Table on witch the form must act.
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use (pass a QSqlDatabase() if only one database is used).
   * \param firstWidgetInTabOrder It's possible to specify the object name of the first widget in tab order.
   *                               This is then used by insertion to give this widget the focus.
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...)
   */
  bool setMainTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db, const QString & firstWidgetInTabOrder = QString());

  /*! \brief Add a child table to the form
   *
   * \param relationInfo Informations about relation between main table and child table to create (note: parent table name is not used).
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...).
   *          False can also be returned if main table was not set (see setMainTable() ).
   */
  bool addChildTable(const mdtSqlRelationInfo &relationInfo, const QString &userFriendlyTableName);

  /*! \brief Add a child table to the form
   *
   * This method is only useful if different tables have to act on different databases .
   *
   * \param relationInfo Informations about relation between main table and child table to create (note: parent table name is not used).
   * \param userFriendlyTableName If set, will be used at places that are visible from user.
   * \param db Database to use.
   * \return True on success,
   *          false if given table name does not exist or on some other errors (database not open, access privileges, ...).
   *          False can also be returned if main table was not set (see setMainTable() ).
   */
  bool addChildTable(const mdtSqlRelationInfo &relationInfo, const QString &userFriendlyTableName, QSqlDatabase db);

  /*! \brief Add a widget to the internal QTabWidget
   *
   * Can be used if a widget that is not related to a database table must be placed
   *  in the QTabWidget .
   *
   * Note: the given widget will be reparented, and also deleted when the form is destroyed .
   */
  void addChildWidget(QWidget *widget, const QString & label, const QIcon & icon = QIcon());

  /*! \brief Start internal state machines
   *
   * Will start state machine of each contained SQL widget.
   *  This method is called by mdtSqlWindow and mdtSqlDialog.
   */
  void start();

  /*! \brief Get main table controller
   *
   *  Can return a Null pointer if no controller
   *   is assigned to rquested table, or it exists,
   *   but is not of requested type T.
   */
  template<typename T>
  std::shared_ptr<T> mainTableController()
  {
    return std::dynamic_pointer_cast<T>(pvController);
  }

  /*! \brief Get a table controller
   *
   *  Can return a Null pointer if no controller
   *   is assigned to rquested table, or it exists,
   *   but is not of requested type T.
   */
  template<typename T>
  std::shared_ptr<T> tableController(const QString & tableName)
  {
    std::shared_ptr<mdtAbstractSqlTableController> c;
    if(pvController->tableName() == tableName){
      return std::dynamic_pointer_cast<T>(pvController);
    }
    return pvController->childController<T>(tableName);
  }

  /*! Get instance of the stored SQL table widget related to a table name
   */
  mdtSqlTableWidget *sqlTableWidget(const QString &tableName);

  /*! \brief Get database that was given by constructor
   */
  inline QSqlDatabase database() { return pvDatabase; }

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

  /*! \brief Set a filter on main table
   *
   * Set filter based on SQL query WHERE part, without the WHERE keyword (f.ex. Id_PK = 15)
   *
   * \pre Main table must be set with setMainTable()
   * \pre Internal state machine must run (see start() ).
   */
  inline bool setMainTableFilter(const QString & filter)
  {
    return pvController->setFilter(filter);
  }

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchData Match data
   *
   * Internally, a SQL statement is generated linke: fieldName = matchData
   *
   * \pre Main table must be set with setMainTable()
   * \pre Internal state machine must run (see start() ).
   */
  inline bool setMainTableFilter(const QString & fieldName, const QVariant & matchData)
  {
    return pvController->setFilter(fieldName, matchData);
  }

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchDataList A list of match data
   *
   * Internally, a SQL statement is generated linke: fieldName IN (matchData[0], matchData[1], ...)
   *
   * \pre Main table must be set with setMainTable()
   * \pre Internal state machine must run (see start() ).
   */
  inline bool setMainTableFilter(const QString & fieldName, const QList<QVariant> & matchDataList)
  {
    return pvController->setFilter(fieldName, matchDataList);
  }

  /*! \brief Clear filter of main table
   *
   * \pre Main table must be set with setMainTable()
   * \pre Internal state machine must run (see start() ).
   */
  inline void clearMainTableFilter()
  {
    return pvController->clearFilter();
  }

  /*! \brief Get last error
   */
  mdtError lastError() const;

  /*! \brief Display last error in a message box
   */
  void displayLastError();

  /*! \brief Set the first row that matches given criteria as current row.
   *
   * This variant will act on main table controller.
   *
   * \param fieldName Field that must contain searched value
   * \param value Value that must matche
   * \return True if matching record was found
   *          (in this case, setCurrentIndex() is called on sql widget witch acts on main table to update currentRow),
   *          false else.
   */
  inline bool setCurrentRow(const QString & fieldName, const QVariant & matchData)
  {
    return pvController->setCurrentRow(fieldName, matchData);
  }

  /*! \brief Set the first row that matches given criteria as current row.
   *
   * \param tableName The controller acting on this table name is used.
   * \param fieldName Field that must contain searched value
   * \param value Value that must matche
   * \return True if matching record was found
   *          (in this case, setCurrentIndex() is called on sql widget witch acts on main table to update currentRow),
   *          false else.
   */
  bool setCurrentRow(const QString &tableName, const QString & fieldName, const QVariant & matchData);

  /*! \brief Set (update) data of current row (record) for given table and field name
   *
   * Note that SQL table controller related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related controller with tableController() ,
   *  and call his setCurrentData() method.
   */
  bool setCurrentData(const QString &tableName, const QString &fieldName, const QVariant &data, bool submit = true);

  /*! \brief Get current data for given table and field name
   *
   * Note that SQL table controller related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related controller with tableController() ,
   *  and call his currentData() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table or field not found).
   */
  inline QVariant currentData(const QString &tableName, const QString &fieldName)
  {
    bool ok;
    return currentData(tableName, fieldName, ok);
  }

  /*! \brief Get current data for given table and field name
   *
   * Note that SQL table controller related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related controller with tableController() ,
   *  and call his currentData() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table or field not found).
   */
  QVariant currentData(const QString &tableName, const QString &fieldName, bool & ok);

  /*! \brief Get data for given table , row and field name
   *
   * Note that SQL table controller related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related controller with tableController() ,
   *  and call his data() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table, row or field not found).
   */
  inline QVariant data(const QString &tableName, int row, const QString &fieldName)
  {
    bool ok;
    return data(tableName, row, fieldName, ok);
  }

  /*! \brief Get data for given table , row and field name
   *
   * Note that SQL table controller related to given tableName is searched
   *  at each request. If several access to setCurrentData is needed,
   *  it can be better to get a pointer to related controller with tableController() ,
   *  and call his data() method.
   *
   * \return Current value, or a invalid QVariant on error (f.ex. table, row or field not found).
   */
  QVariant data(const QString &tableName, int row, const QString &fieldName, bool & ok);

  /*! \brief Call insert method on main table controller
   */
  void insert() { pvController->insert(); }

  /*! \brief Setup tables
   *
   * Must be re-implemented in subclass, default implementation does nothing .
   */
  virtual bool setupTables();

  /*! \brief Check if all data are saved
   *
   * Will call allDataAreSaved() on main table controller.
   *
   * \return true if all data are saved, false if a SQL table controller has pending data.
   */
  inline bool allDataAreSaved() { return pvController->allDataAreSaved(); }

 protected:

  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtSqlForm);

  std::shared_ptr<mdtSqlDataWidgetController> pvController;
  QWidget *pvMainTableWidget;
  QTabWidget *pvChildsTabWidget;
  QList<mdtSqlTableWidget*> pvChildsTableWidgets;
  QVBoxLayout *pvMainLayout;
  QSqlDatabase pvDatabase;
};

#endif  // #ifndef MDT_SQL_FORM_H
