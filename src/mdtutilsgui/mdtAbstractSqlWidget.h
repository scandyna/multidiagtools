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

class QSqlTableModel;
class QState;
class QStateMachine;

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

  /*! \brief Get the current row
   *
   * Current row can be the currently selected row
   *  (case of a table view),
   *  or simply the current row that is displayed in a form view.
   *
   * Subclass must implement this method.
   */
  virtual int currentRow() const = 0;

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

 signals:

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

  /*! \brief Emitted when Editing state was exited
   */
  void onStateEditingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void onStateEditingNewRowExited();

 private slots:

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateVisualizingEntered();

  /*! \brief Activity after Editing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateEditingEntered();

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

 private:

  /*! \brief Build the state machine
   */
  void buildStateMachine();

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

  Q_DISABLE_COPY(mdtAbstractSqlWidget);
};

#endif  // #ifndef MDT_ABSTRACT_SQL_WIDGET_H
