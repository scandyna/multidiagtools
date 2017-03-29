/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H

#include "ControllerState.h"
#include "ControllerEvent.h"
#include <QObject>
#include <memory>
#include <type_traits>

namespace Mdt{ namespace ItemEditor{

  class AbstractControllerStateTable;
  class AbstractControllerStatePermission;

  /*! \brief Synchronous state machine for a item editor controller
   *
   * ControllerStateMachine is used by AbstractController.
   *  AbstractController receives events from the GUI,
   *  but can also be driven by code.
   *  Experience showed that the latter does not work well with
   *  a asynchronous state machine, this why a existing framework
   *  (like QStateMachine) is not used here.
   *
   * ControllerStateMachine is divided in 2 parts:
   *  - Check if this state machine can accept a event,
   *     which is driven by AbstractControllerStatePermission (or a subclass of it).
   *  - Deducing next state regarding current state and the event,
   *     which is driven by AbstractControllerStateTable (or a subclass of it).
   */
  class ControllerStateMachine : public QObject
  {
   Q_OBJECT

   public:

    // Default constructor disabled
    ControllerStateMachine() = delete;

    /*! \brief Destruct this ControllerStateMachine
     */
    ~ControllerStateMachine();

    // Copy disabled
    ControllerStateMachine(const ControllerStateMachine & other) = delete;
    ControllerStateMachine & operator=(const ControllerStateMachine & other) = delete;
    // Move disabled
    ControllerStateMachine(ControllerStateMachine && other) = delete;
    ControllerStateMachine & operator=(ControllerStateMachine && other) = delete;

    /*! \brief Set if model has cache
     */
    void setModelHasCache(bool hasCache);

    /*! \brief Set if the controller handles child controllers
     */
    void setHasChildController(bool hasChild);

    /*! \brief Check if it is allowed to change current row for current state
     */
    bool canChangeCurrentRow() const;

    /*! \brief Check if it is allowed to insert for current state
     */
    bool canInsert() const;

    /*! \brief Check if it is possible to edit for current state
     */
    bool canEdit() const;

    /*! \brief Check if it is possible to submit for current state
     */
    bool canSubmit() const;

    /*! \brief Check if it is possible to revert for current state
     */
    bool canRevert() const;

    /*! \brief Check if it is possible to remove for current state
     */
    bool canRemove() const;

    /*! \brief Check if it is possible to select data from database for current state
     */
    bool canSelect() const;

    /*! \brief Check if changing current row action is enabled for current state
     */
    bool isChangeCurrentRowActionEnabled() const;

    /*! \brief Check if insert action is enabled for current state
     */
    bool isInsertActionEnabled() const;

    /*! \brief Check if submit action is enabled for current state
     */
    bool isSubmitActionEnabled() const;

    /*! \brief Check if revert action is enabled for current state
     */
    bool isRevertActionEnabled() const;

    /*! \brief Check if remove action is enabled for current state
     */
    bool isRemoveActionEnabled() const;

    /*! \brief Check if select action is enabled for current state
     */
    bool isSelectActionEnabled() const;

    /*! \brief Data edition started event
     */
    void dataEditionStarted();

    /*! \brief Data edition done event
     */
    void dataEditionDone();

    /*! \brief Submit done event
     */
    void submitDone();

    /*! \brief Revert done event
     */
    void revertDone();

    /*! \brief Insert started event
     */
    void insertStarted();

    /*! \brief Remove done event
     */
    void removeDone();

    /*! \brief Set event
     */
    void setEvent(ControllerEvent event);

//     /*! \brief Data edition started from parent controller event
//      */
//     void dataEditionStartedFromParent();

    /*! \brief Construct a ControllerStateMachine with a concrete state chain and permission implementations
     *
     * \code
     * auto stateMachine = ControllerStateMachine::make<MyStateTable, MyPermission>();
     * \endcode
     *
     * \pre TableImpl must be a subclass of AbstractControllerStateTable
     * \pre PermissionImpl must be a subclass of AbstractControllerStatePermission
     */
    template<typename TableImpl, typename PermissionImpl>
    static ControllerStateMachine *makeNew(QObject *parent = nullptr)
    {
      static_assert( std::is_base_of<AbstractControllerStateTable, TableImpl>::value, "Type TableImpl must be a subclass of Mdt::ItemEditor::AbstractControllerStateTable" );
      static_assert( std::is_base_of<AbstractControllerStatePermission, PermissionImpl>::value, "Type PermissionImpl must be a subclass of Mdt::ItemEditor::AbstractControllerStatePermission" );
      return new ControllerStateMachine( std::make_unique<TableImpl>() , std::make_unique<PermissionImpl>(), parent );
    }

    /*! \brief Get current state
     */
    ControllerState currentState() const;

    /*! \internal Force current state
     *
     * This method is used for unit tests.
     */
    void forceCurrentState(ControllerState state);

   signals:

    /*! \brief Emitted whenever current state changed
     */
    void currentStateChanged();

    /*! \brief Emitted whenever a event completed
     */
    void eventCompleted(Mdt::ItemEditor::ControllerEvent event);

  private:

    template<typename TableImpl, typename PermissionImpl>
    ControllerStateMachine(std::unique_ptr<TableImpl> tableImpl, std::unique_ptr<PermissionImpl> permission, QObject *parent)
     : QObject(parent),
       mTableImpl( std::move(tableImpl) ),
       mPermissionImpl( std::move(permission) )
    {
      createTransitionTable();
    }

    void createTransitionTable();

    std::unique_ptr<AbstractControllerStateTable> mTableImpl;
    std::unique_ptr<AbstractControllerStatePermission> mPermissionImpl;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H
