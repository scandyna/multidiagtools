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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_ACTION_CONTAINER_H
#define MDT_ITEM_EDITOR_ABSTRACT_ACTION_CONTAINER_H

#include "RowState.h"
#include "ControllerState.h"
#include <QObject>
#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  class ControllerStateMachine;

  /*! \brief Base class to create action containers
   *
   * AbstractActionContainer can be subclassed to create collection of actions
   *  that will be used with item editor classes.
   *  Its main goal is to help keeping actions enable state coherent
   *  when used with AbstractController implementations.
   */
  class AbstractActionContainer : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractActionContainer(QObject* parent = nullptr);

    // Copy disabled
    AbstractActionContainer(const AbstractActionContainer &) = delete;
    AbstractActionContainer & operator=(const AbstractActionContainer &) = delete;
    // Move disabled
    AbstractActionContainer(AbstractActionContainer &&) = delete;
    AbstractActionContainer & operator=(AbstractActionContainer &&) = delete;

    /*! \brief Set controller state machine
     *
     * \pre \a stateMachine must be a valid pointer
     */
    void setControllerStateMachine(const ControllerStateMachine * const stateMachine);

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mRowState.rowCount();
    }

    /*! \brief Get current row
     */
    int currentRow() const
    {
      return mRowState.currentRow();
    }

    /*! \brief Check if row state is null
     */
    bool rowStateIsNull() const
    {
      return mRowState.isNull();
    }

    /*! \brief Check if row state is valid
     */
    bool rowStateIsValid() const
    {
      return mRowState.isValid();
    }

    /*! \brief Get controller state
     */
    ControllerState controllerState() const;

    /*! \brief Check if changing current row action is enabled for state
     */
    bool isChangeCurrentRowActionEnabled() const;

    /*! \brief Check if insert action is enabled for state
     */
    bool isInsertActionEnabled() const;

    /*! \brief Check if submit action is enabled for state
     */
    bool isSubmitActionEnabled() const;

    /*! \brief Check if revert action is enabled for state
     */
    bool isRevertActionEnabled() const;

    /*! \brief Check if remove action is enabled for state
     */
    bool isRemoveActionEnabled() const;

    /*! \brief Check if select action is enabled for state
     */
    bool isSelectActionEnabled() const;

   public slots:

    /*! \brief Set row state
     */
    void setRowState(Mdt::ItemEditor::RowState rs);

    /*! \brief Set actions disabled
     *
     * If \a disable is true, all actions will be disabled,
     *  regardless of any other state.
     *  Else, actions are enabled regarding all other states.
     */
    void setActionsDisabled(bool disable);

   protected:

    /*! \brief Update enable state
     *
     * This method is called each time a property changes.
     *
     * Subclass must implement this method to enable or disable
     *  its actions regarding current properties.
     *
     * \sa rowCount()
     * \sa currentRow()
     * \sa controllerState()
     *
     * \note This method is not called by the constructor
     */
    virtual void updateEnableState() = 0;

    /*! \brief Disable all actions regrdless of current state
     *
     * Subclass must implement this method to disable all its actions,
     *  without taking attention of current properties.
     */
    virtual void disableAllActions() = 0;

   private slots:

    /*! \brief Actions to perform when controller state changed
     */
    void onControllerStateChanged();

   private:

    bool mActionsDisabled = false;
    RowState mRowState;
    QPointer<const ControllerStateMachine> mControllerStateMachine;
    QMetaObject::Connection mControllerStateChangedConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_ACTION_CONTAINER_H
