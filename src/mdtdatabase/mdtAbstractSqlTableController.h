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
#ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
#define MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H

#include "mdtError.h"
#include "mdtStateMachine.h"
#include "mdtUiMessageHandler.h"
#include "mdtSqlDataValidator.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSortFilterProxyModel.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QList>
#include <memory>

class mdtState;
class mdtAbstractSqlTableController;
class mdtSqlRelation;
class mdtSortFilterProxyModel;
class QSqlField;

#include <QDebug>

/*
 * Container for internal use
 */
struct mdtAbstractSqlTableControllerContainer
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;
  std::shared_ptr<mdtSqlRelation> relation;
};

/*! \brief Base class for SQL table controllers
 *
 * In MVC pattern, this class is the controller part for database GUI.
 *  \todo Documnent correctly
 *
 * When creating a GUI for database table edition,
 *  this class can be used as controller between your GUI and QSqlTableModel.
 *  To keep coherence of common operation, such as insert, remove, submit and remove,
 *  some signals are available:
 *   - insertEnabledStateChanged()
 *   - removeEnabledStateChanged()
 *   - submitEnabledStateChanged()
 *   - removeEnabledStateChanged()
 *
 * This abstract class can not be used directly, but must be subclassed.
 *  Use existing one, or create your own if needed.
 *
 * Typicall usage:
 *  - Make needed signal/slot connections
 *  - Set table with setTableName() or setModel()
 *  - Start inetrnal state machine with start()
 *  - Load data with select()
 */
class mdtAbstractSqlTableController : public QObject
{
  Q_OBJECT

 public:

  /*! \brief State of edit/view widget
   *
   * This state is set by internal state machine.
   */
  enum State_t {
                Stopped = 0,      /*!< Stopped state (internall state machine not running, see start() ) */
                Selecting ,       /*!< Selecting state */
                Visualizing ,     /*!< Visualizing state */
                Reverting,        /*!< Reverting state */
                Editing,          /*!< Editing state */
                Submitting,       /*!< Submitting state */
                Inserting,        /*!< Inserting state */
                EditingNewRow,    /*!< Editing a new row */
                RevertingNewRow,  /*!< RevertingNewRow state */
                SubmittingNewRow, /*!< SubmittingNewRow state */
                Removing          /*!< Removing state */
               };

  /*! \brief Constructor
   */
  mdtAbstractSqlTableController(QObject * parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlTableController();

  /*! \brief Set message handler
   *
   * Will also set this message handler to each contained data validator (see addDataValidator() ).
   */
  void setMessageHandler(std::shared_ptr<mdtUiMessageHandler> handler);

  /*! \brief Check if a message handler was allready set
   */
  bool hasMessageHandler() const;

  /*! \brief Set table name
   *
   * Will replace internal model if allready set.
   */
  void setTableName(const QString & tableName, QSqlDatabase db, const QString & userFriendlyTableName = QString());

  /*! \brief Set table model
   *
   * Will replace internal model if allready set.
   */
  void setModel(std::shared_ptr<QSqlTableModel> m, const QString & userFriendlyTableName = QString());

  /*! \brief Get index of field with fieldName
   *
   * Returns a value < 0 if fieldName was not found.
   */
  int fieldIndex(const QString & fieldName) const;

  /*! \brief Set a user friendly name for a column
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void setHeaderData(const QString &fieldName, const QString &data);

  /*! \brief Get header data of given section
   */
  inline QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    return pvProxyModel->headerData(section, orientation, role);
  }

  /*! \brief Get header data of given column
   */
  inline QVariant headerData(int column, int role = Qt::DisplayRole) const
  {
    return pvProxyModel->headerData(column, Qt::Horizontal, role);
  }

  /*! \brief Get header data of given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant headerData(const QString & fieldName, int role = Qt::DisplayRole) const;

  /*! \brief Add a child controller
   *
   * Will create a controller of type T, needed relations (mdtSqlRelation objects)
   *  and add controller to childs.
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param db QSqlDatabase object to use
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  template<typename T>
  bool addChildController(const mdtSqlRelationInfo & relationInfo, QSqlDatabase db, const QString & userFriendlyChildTableName = QString())
  {
    Q_ASSERT(pvModel);
    std::shared_ptr<T> controller(new T);
    return setupAndAddChildController(controller, relationInfo, db, userFriendlyChildTableName);
  }

  /*! \brief Add a child controller
   *
   * Will create a controller of type T, needed relations (mdtSqlRelation objects)
   *  and add controller to childs.
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  template<typename T>
  bool addChildController(const mdtSqlRelationInfo & relationInfo, const QString & userFriendlyChildTableName = QString())
  {
    Q_ASSERT(pvModel);
    return addChildController<T>(relationInfo, pvModel->database(), userFriendlyChildTableName);
  }

  /*! \brief Get a child controller
   *
   *  Can return a Null pointer if no controller
   *   is assigned to rquested table, or it exists,
   *   but is not requested type T.
   */
  template<typename T>
  std::shared_ptr<T> childController(const QString & tableName)
  {
    int i;
    std::shared_ptr<mdtAbstractSqlTableController> c;
    for(i = 0; i < pvChildControllerContainers.size(); ++i){
      c = pvChildControllerContainers.at(i).controller;
      Q_ASSERT(c);
      if(c->tableName() == tableName){
        return std::dynamic_pointer_cast<T>(c);
      }
    }
    return std::shared_ptr<T>();
  }

  /*! \brief Select data in main table
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool select();

  /*! \brief Set a filter on main table
   *
   * Set filter based on SQL query WHERE part, without the WHERE keyword (f.ex. Id_PK = 15)
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & filter);

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchData Match data
   *
   * Internally, a SQL statement is generated linke: fieldName = matchData
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & fieldName, const QVariant & matchData);

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchDataList A list of match data
   *
   * Internally, a SQL statement is generated linke: fieldName IN (matchData[0], matchData[1], ...)
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & fieldName, const QList<QVariant> & matchDataList);

  /*! \brief Clear filter
   */
  void clearFilter();

  /*! \brief Get model (read only access)
   */
  inline const std::shared_ptr<QSqlTableModel> model() const { return pvModel; }

  /*! \brief Get model (write access)
   *
   * \todo If not needed, make it protected and adapt tests
   */
  inline std::shared_ptr<QSqlTableModel> model() { return pvModel; }

  /*! \brief Get (technical) table name
   */
  QString tableName() const;

  /*! \brief Set user friendly table name
   */
  void setUserFriendlyTableName(const QString & name) { pvUserFriendlyTableName = name; }

  /*! \brief Get user friendly table name
   */
  QString userFriendlyTableName() const { return pvUserFriendlyTableName; }

  /*! \brief Start internal state machine
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void start();

  /*! \brief Stop internal state machine
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void stop();

  /*! \brief Get current state
   */
  inline State_t currentState() const {
    if(!pvStateMachine.isRunning()){
      return Stopped;
    }
    return static_cast<State_t>(pvStateMachine.currentState());
  }

  /*! \brief Add a data validator
   *
   * \param validator A instance of mdtSqlDataValidator or subclass.
   * \param putAtTopPriority If true, validator will be called before allready stored ones.
   *                          (validators are put into a queue).
   * \param setInternalMessageHandler If true, message handler set with setMessageHandler() will be set to validator.
   *
   * \pre validator must be a valid pointer.
   */
  void addDataValidator(std::shared_ptr<mdtSqlDataValidator> validator, bool putAtTopPriority = false, bool setInternalMessageHandler = true);

  /*! \brief Add a column to columns sort order
   *
   * Columns sort order is similar meaning
   *  than SQL ORDER BY clause.
   *
   * Internally, mdtSortFilterProxyModel is used, witch provide a natural sort for strings.
   *
   * For example, to sort columns in order "Id_PK", "FirstName", "LastName", all ascending, call:
   *  - clearColumnsSortOrder();
   *  - addColumnToSortOrder("Id_PK", Qt::AscendingOrder);
   *  - addColumnToSortOrder("FirstName", Qt::AscendingOrder);
   *  - addColumnToSortOrder("LastName", Qt::AscendingOrder);
   *
   * Note: if given field not exists, it will simply be ignored.
   *
   * Note: to apply sorting, call sort()
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void addColumnToSortOrder(const QString & fieldName, Qt::SortOrder order = Qt::AscendingOrder);

  /*! \brief Clear columns sort order
   *
   * \sa addColumnToSortOrder()
   */
  void clearColumnsSortOrder();

  /*! \brief Sort data
   *
   * \sa addColumnToSortOrder()
   */
  void sort();

  /*! \brief Set current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentData(const QString &fieldName, const QVariant &data, bool submit = true);

  /*! \brief Set data for given row and field name
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setData(int row, const QString &fieldName, const QVariant &data, bool submit = true);

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName)
  {
    Q_ASSERT(pvModel);
    return data(currentRow(), fieldName);
  }

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName, bool & ok)
  {
    Q_ASSERT(pvModel);
    return data(currentRow(), fieldName, ok);
  }

  /*! \brief Get formated current value for SQL query
   */
  inline QString currentFormatedValue(const QString & fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return currentFormatedValue(fieldName, ok);
  }

  /*! \brief Get formated current value for SQL query
   */
  inline QString currentFormatedValue(const QString & fieldName, bool & ok)
  {
    Q_ASSERT(pvModel);
    return formatedValue(currentRow(), fieldName, ok);
  }

  /*! \brief Get data for given index
   *
   * Note: index is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(const QModelIndex & index)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(index, ok);
  }

  /*! \brief Get data for given index
   *
   * Note: index is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant data(const QModelIndex & index, bool & ok);

  /*! \brief Get data for given row and column
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, int column)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(row, column, ok);
  }

  /*! \brief Get data for given row and column
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, int column, bool & ok)
  {
    Q_ASSERT(pvModel);
    return data(pvProxyModel->index(row, column), ok);
  }

  /*! \brief Get data for given row and field name
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, const QString &fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(row, fieldName, ok);
  }

  /*! \brief Get data for given row and field name
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant data(int row, const QString &fieldName, bool & ok);

  /*! \brief Get a list of data for given field name
   *
   * If fetchAll is true, all available data will be fetched from database
   *  (regarding applied filter), else only cached data are returned
   *  (see QSqlQueryModel::fetchMore() for details).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QList<QVariant> dataList(const QString &fieldName, bool fetchAll = true)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return dataList(fieldName, ok, fetchAll);
  }

  /*! \brief Get a list of data for given field name
   *
   * If fetchAll is true, all available data will be fetched from database
   *  (regarding applied filter), else only cached data are returned
   *  (see QSqlQueryModel::fetchMore() for details).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QList<QVariant> dataList(const QString &fieldName, bool & ok, bool fetchAll = true);

  /*! \brief Get formated value for SQL query
   */
  inline QString formatedValue(int row, const QString & fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return formatedValue(row, fieldName, ok);
  }

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatedValue(int row, const QString & fieldName, bool & ok);

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  inline QString formatValue(const QString & fieldName, const QVariant & value)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return formatValue(fieldName, value, ok);
  }

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatValue(const QString & fieldName, const QVariant & value, bool & ok);

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatValue(const QSqlField & field);

  /*! \brief Get column count
   */
  inline int columnCount() const { return pvProxyModel->columnCount(); }

  /*! \brief Get row count
   *
   * \param fetchAll If true, all rows will be fetched in database (see QSqlTableModel::fetchMore() )
   */
  int rowCount(bool fetchAll = true) const;

  /*! \brief Set current row
   *
   * If not all data are saved for current row, this method will return false.
   *  If row is > model's cached rowCount-1, data will be fetched
   *   in database. If request row could not be found, false is returned.
   *
   * \param row Row to witch to go. Must be in range [-1;rowCount()-1]
   *            Note: row is relative to sorted data model (proxyModel),
   *             not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentRow(int row);

  /*! \brief Set current row
   *
   * Will search the first record that contains
   *  match data for given field name.
   *
   * \sa setCurrentRow(int).
   */
  bool setCurrentRow(const QString & fieldName, const QVariant & matchData);

  /*! \brief Get current row
   */
  virtual int currentRow() const = 0;

  /*! \brief Check if all data are saved
   *
   * This will check state of current table controller
   *  and state of all assigned child controllers.
   *  If one of them is not in Visualizing, Selecting or Stopped state,
   *  some data are not saved.
   *
   * \return true if all data are saved, false if there are pending data.
   */
  bool allDataAreSaved();

  /*! \brief Get last error
   */
  mdtError lastError() const { return pvLastError; }

  /*! \brief Submit and wait until success or failure
   *
   *  Note: will not freeze Qt's event loop.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool submitAndWait();

 public slots:

  /*! \brief Submit current record to model
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   *
   * Internally, submitTriggered() signal will be emitted.
   */
  void submit();

  /*! \brief Revert current record from model
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   *
   * Internally, revertTriggered() signal will be emitted.
   */
  void revert();

  /*! \brief Insert a new row to model
   *
   * Internally, insertTriggered() signal will be emitted.
   */
  void insert();

  /*! \brief Remove a row from model
   *
   * Internally, removeTriggered() signal will be emitted.
   */
  void remove();

 protected:

  /*! \brief Last error (for write access)
   */
  mdtError pvLastError;

  /*! \brief Get message handler
   *
   * Can return a Null pointer if no message handler was set (witch is acceptable case).
   */
  std::shared_ptr<mdtUiMessageHandler> messageHandler() { return pvMessageHandler; }

  /*! \brief Get internal proxy model
   */
  inline std::shared_ptr<mdtSortFilterProxyModel> proxyModel() { return pvProxyModel; }

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool commitTransaction();

  /*! \brief Table model set event
   *
   * Cann be re-implemented in subclass
   *  if some things must be done once a table model was set.
   */
  virtual void modelSetEvent() {}

  /*! \brief Current row changed event
   */
  virtual void currentRowChangedEvent(int row) = 0;

  /*! \brief Set current row - For internal use
   *
   * If row is > model's cached rowCount-1, data will be fetched
   *  in database. If request row could not be found, false is returned.
   *
   * \param row Row to witch to go. Must be in range [-1;rowCount()-1]
   * \param forceSendCurrentRowChanedEvent If true, currentRowChangedEvent() will be called, not regarding if
   *                                        row has changed (usefull when filter was applyed)
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentRowPv(int row, bool forceSendCurrentRowChanedEvent);

  /*! \brief Submit current row to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   */
  virtual bool doSubmit() = 0;

  /*! \brief Revert current row from model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   *
   * Depending on subclass choosen EditStrategy,
   *  this method has potentially no effect.
   */
  virtual bool doRevert() = 0;

  /*! \brief Insert a new row to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doInsert() = 0;

  /*! \brief Submit new row to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doSubmitNewRow() = 0;

  /*! \brief Revert new row
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doRevertNewRow() = 0;

  /*! \brief Remove current row from model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doRemove() = 0;

 signals:

  /*! \brief Emitted when current row has changed
   */
  void currentRowChanged(int row);

  /*! \brief Emitted when the entier widget enable state changes
   */
  void mainWidgetEnableStateChanged(bool enabled);

  /*! \brief Emitted when the entier widget enable state changes for a child widget
   */
  void childWidgetEnableStateChanged(bool enabled);

  /*! \brief Emitted when submit() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void submitEnabledStateChanged(bool enabled);

  /*! \brief Emitted when revert() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void revertEnabledStateChanged(bool enabled);

  /*! \brief Emitted when insert() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void insertEnabledStateChanged(bool enabled);

  /*! \brief Emitted when remove() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void removeEnabledStateChanged(bool enabled);

  /*! \brief Emitted when select() or setFilter() was called
   */
  void selectTriggered();

  /*! \brief Emitted when submit() was called
   */
  void submitTriggered();

  /*! \brief Emitted when revert() was called
   */
  void revertTriggered();

  /*! \brief Emitted when insert() was called
   */
  void insertTriggered();

  /*! \brief Emitted when remove() was called
   */
  void removeTriggered();

  /*! \brief Emitted when a error occured
   */
  void errorOccured();

  /*! \brief Emitted when a operation succeed
   */
  void operationSucceed();

  /*! \brief Emitted when data was edited
   */
  void dataEdited();

  /*! \brief Emitted when Visualizing state was entered
   */
  ///void stateVisualizingEntered();

  /*! \brief Emitted when Visualizing state was exited
   */
  ///void stateVisualizingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingNewRowExited();

 protected slots:

  /*! \brief Activity after Selecting state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSelectingEntered();

  /*! \brief Called by mdtSqlRelation whenn it applied filter
   */
  void onRelationFilterApplied();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateVisualizingEntered();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateVisualizingExited();

  /*! \brief Activity after Editing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingExited();

  /*! \brief Activity after Submitting state entered
   *
   * Will call checkBeforeSubmit() and, on success, doSubmit().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSubmittingEntered();

  /*! \brief Activity after Reverting state entered
   *
   * Will call doRevert().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRevertingEntered();

  /*! \brief Activity after Inserting state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateInsertingEntered();

  /*! \brief Activity after EditingNewRow state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingNewRowEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingNewRowExited();

  /*! \brief Activity after SubmittingNewRow state entered
   *
   * Will call checkBeforeSubmit() and, on success, doSubmitNewRow().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSubmittingNewRowEntered();

  /*! \brief Activity after RevertingNewRow state entered
   *
   * Will call doRevertNewRow().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRevertingNewRowEntered();

  /*! \brief Activity after Removing state entered
   *
   * Will call doRemove().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRemovingEntered();

 private:

  /*! \brief Setup and add child controller
   */
  bool setupAndAddChildController(std::shared_ptr<mdtAbstractSqlTableController> controller, const mdtSqlRelationInfo & relationInfo, QSqlDatabase db, const QString & userFriendlyChildTableName);

  /*! \brief Call checkBeforeSubmit() for each installed validators
   *
   * \sa addDataValidator()
   */
  bool checkBeforeSubmit();

  /*! \brief Wait until a operation completes
   *
   * Checks pvOperationComplete (don't forget to reset this flag before call)
   */
  void waitOperationComplete();

  /*! \brief Build the state machine
   */
  void buildStateMachine();

  Q_DISABLE_COPY(mdtAbstractSqlTableController);

  std::shared_ptr<QSqlTableModel> pvModel;
  std::shared_ptr<mdtSortFilterProxyModel> pvProxyModel;
  QList<std::shared_ptr<mdtSqlDataValidator> > pvDataValidators;
  std::shared_ptr<mdtUiMessageHandler> pvMessageHandler;
  QString pvUserFriendlyTableName;
  bool pvOperationComplete;
  ///QList<std::shared_ptr<mdtAbstractSqlTableController> > pvChildControllers;
  QList<mdtAbstractSqlTableControllerContainer> pvChildControllerContainers;
  // State machine members
  mdtState *pvStateSelecting;
  mdtState *pvStateVisualizing;
  mdtState *pvStateReverting;
  mdtState *pvStateEditing;
  mdtState *pvStateSubmitting;
  mdtState *pvStateInserting;
  mdtState *pvStateEditingNewRow;
  mdtState *pvStateRevertingNewRow;
  mdtState *pvStateSubmittingNewRow;
  mdtState *pvStateRemoving;
  mdtStateMachine pvStateMachine;
};

#endif  // #ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
