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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_H

#include "ControllerState.h"
#include <QObject>

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Relation between a parent controller and a child controller
   */
  class AbstractControllerRelation : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit AbstractControllerRelation(AbstractController *parentController, QObject *parent = nullptr);

    // Copy disabled
    AbstractControllerRelation(const AbstractControllerRelation &) = delete;
    AbstractControllerRelation & operator=(const AbstractControllerRelation &) = delete;
    // Move disabled
    AbstractControllerRelation(AbstractControllerRelation &&) = delete;
    AbstractControllerRelation & operator=(AbstractControllerRelation &&) = delete;

   protected:

    /*! \brief Get parent controller
     */
    virtual AbstractController *abstractParentController() const = 0;

    /*! \brief Get child controller
     */
    virtual AbstractController *abstractChildController() const = 0;

    /*! \brief Register child controller
     */
    void registerAbstractChildController(AbstractController *controller);

    /*! \brief Parent controller's state changed event
     */
    virtual void parentControllerStateChangedEvent(ControllerState newState);

    /*! \brief Child controller's state changed event
     */
    virtual void childControllerStateChangedEvent(ControllerState newState);

   private slots:

    /*! \brief Set parent controller state
     */
    void setParentControllerState(Mdt::ItemEditor::ControllerState newState);

    /*! \brief Set child controller state
     */
    void setChildControllerState(Mdt::ItemEditor::ControllerState newState);

 private:

    QMetaObject::Connection mChildControllerStateChagedConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_H
