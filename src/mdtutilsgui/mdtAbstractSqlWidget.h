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
#ifndef MDT_ABSTRACT_SQL_WIDGET_H
#define MDT_ABSTRACT_SQL_WIDGET_H

#include <QWidget>
#include <QSqlError>
#include <QString>
#include <QList>
#include <QSqlRecord>

class QSqlTableModel;
class QState;
class QStateMachine;
class QSqlTableModel;
class mdtSqlRelation;

/*! \brief Base class to create database table GUI
 *
 * Main goal is to provide a standard way to use
 *  GUI object for database table.
 */
class mdtAbstractSqlWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief State of edit/view widget
   *
   * This state is set by internal state machine.
   */
  enum state_t {
                Visualizing = 0,  /*!< Visualizing state */
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
   *
   * Will initialize internal state machine.
   */
  mdtAbstractSqlWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlWidget();

  /*! \brief Set the table model
   *
   * Will call doSetModel() , then start the internall state machine.
   *
   * \pre model must be a valid pointer
   * \pre Internal state machine must not running.
   */
  void setModel(QSqlTableModel *model);

  /*! \brief Get current model
   *
   * Can return a null pointer if model was not set.
   */
  QSqlTableModel *model();

  /*! \brief Get the user friendly table name
   *
   * If the user friendly table name was not set,
   *  the database table name is returned
   *
   * \pre (Parent) model must be set with setModel() before using this method.
   */
  QString userFriendlyTableName() const;

  /*! \brief Add a child (details) widget and relation
   *
   * Needed signal/slot connection between widget
   *  and relation is done internally.
   *
   * \pre (Parent) model must be set with setModel() before using this method.
   * \pre widget must be a valid pointer
   * \pre relation must be a valid pointer.
   *       Note: relation will be reparented, so it will be deleted automatically
   *             by Qt.
   */
  void addChildWidget(mdtAbstractSqlWidget *widget, mdtSqlRelation *relation);

  /*! \brief Get the current row
   *
   * Current row can be the currently selected row
   *  (case of a table view),
   *  or simply the current row that is displayed in a form view.
   *
   * Subclass must implement this method.
   */
  virtual int currentRow() const = 0;

  /*! \brief Get row count
   *
   * \pre Model must be set with setModel() before using this method.
   */
  int rowCount() const;

  /*! \brief Get current state
   */
  state_t currentState() const;

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
   *
   * This default implementation does nothing.
   */
  virtual void enableLocalNavigation();

  /*! \brief Enable local edition
   *
   * Will add insert/revert/save/delete buttons.
   *
   * In normal way, when using mdtSqlWindow,
   *  local navigation is not needed.
   *  In master/detail forms, if this widget
   *  is a details (child) widget and edition
   *  is needed, then call this method.
   *
   * This default implementation does nothing.
   */
  virtual void enableLocalEdition();

  /*! \brief Check if all data are saved
   *
   * This will check state of main SQL widget (this widget)
   *  and state of all assigned child widgets.
   *  If one of them is not in Visualizing state,
   *  some data are not saved. In this case,
   *  a message box is displayed to the user.
   *
   * This is used by mdtSqlWindow to check if it can close the window.
   *
   * \return true if all data are saved, false if a SQL widget has pending data.
   */
  bool allDataAreSaved();

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

  /*! \brief Set first record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toFirst() = 0;

  /*! \brief Set last record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toLast() = 0;

  /*! \brief Set previous record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toPrevious() = 0;

  /*! \brief Set next record as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void toNext() = 0;

  /*! \brief Set row as current record
   *
   * Subclass must implement this method.
   *  Errors and user interactions must be handled by subclass.
   */
  virtual void setCurrentIndex(int row) = 0;

 protected:

  /*! \brief Set the table model
   *
   * Subclass must implement this method.
   *
   * \pre model must be a valid pointer
   */
  virtual void doSetModel(QSqlTableModel *model) = 0;

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

  /*! \brief Insert a row row to model
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

  /*! \brief Display a message to the user regarding error
   *
   * Internally, error will be logged with mdtError system.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void displayDatabaseError(QSqlError error);

  /*! \brief Get message for the user regarding SQLite error number
   */
  QString getUserReadableTextFromSqliteError(const QSqlError &error);

  /*! \brief Get message for the user regarding MySql error number
   */
  QString getUserReadableTextFromMysqlError(const QSqlError &error);

  /*! \brief Check that child widgtes have no pending data
   */
  bool childWidgetsAreInVisaluzingState();

  /*! \brief Show a message box to the user to warn him that it should save/revert data
   */
  void warnUserAboutUnsavedRow(const QString &tableName);

  /*! \brief Restore primary key data to (parent) model
   *
   * \param previousRecord Record that contains data in witch
   *                        primary key data will be token, and sotred
   *                        again into model if they differ.
   * \pre (Parent) model must be set with setModel() before calling this method.
   */
  bool restorePrimaryKeyDataToModel(const QSqlRecord &previousRecord);

 signals:

  /*! \brief Emitted when toFirst() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toFirstEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toLast() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toLastEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toNext() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toNextEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toPrevious() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toPreviousEnabledStateChanged(bool enabled);

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
  void stateVisualizingEntered();

  /*! \brief Emitted when Visualizing state was exited
   */
  void stateVisualizingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingNewRowExited();

  /*! \brief Emitted when current row has changed
   *
   * This is usefull, for example,
   *  in master/detail forms, to update
   *  child models filter.
   *
   * Subclass must emit this signal
   *  each time the user selects a new row.
   */
  void currentRowChanged(int row);

 private slots:

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateVisualizingEntered();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateVisualizingExited();

  /*! \brief Activity after Editing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingExited();

  /*! \brief Activity after Submitting state entered
   *
   * Will call doSubmit().
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
   * Will call doInsert().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateInsertingEntered();

  /*! \brief Activity after EditingNewRow state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingNewRowEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingNewRowExited();

  /*! \brief Activity after SubmittingNewRow state entered
   *
   * Will call doSubmitNewRow().
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

  /*! \brief Disable child widgets
   */
  void disableChildWidgets();

  /*! \brief Enable child widgets
   */
  void enableChildWidgets();

 private:

  /*! \brief Build the state machine
   */
  void buildStateMachine();

  QSqlTableModel *pvModel;
  // State machine members
  QState *pvStateVisualizing;
  QState *pvStateReverting;
  QState *pvStateEditing;
  QState *pvStateSubmitting;
  QState *pvStateInserting;
  QState *pvStateEditingNewRow;
  QState *pvStateRevertingNewRow;
  QState *pvStateSubmittingNewRow;
  QState *pvStateRemoving;
  QStateMachine *pvStateMachine;
  state_t pvCurrentState;
  // Memebrs for child widgets support
  QList<mdtSqlRelation*> pvRelations;
  QList<mdtAbstractSqlWidget*> pvChildWidgets;
  // Other members
  QString pvUserFriendlyTableName;

  Q_DISABLE_COPY(mdtAbstractSqlWidget);
};

#endif  // #ifndef MDT_ABSTRACT_SQL_WIDGET_H
