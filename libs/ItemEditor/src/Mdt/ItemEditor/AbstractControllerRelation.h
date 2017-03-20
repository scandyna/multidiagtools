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
#include <QPointer>

class QAbstractItemModel;

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Relation between a parent controller and a child controller
   */
  class AbstractControllerRelation : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractControllerRelation(QObject *parent = nullptr);

    // Copy disabled
    AbstractControllerRelation(const AbstractControllerRelation &) = delete;
    AbstractControllerRelation & operator=(const AbstractControllerRelation &) = delete;
    // Move disabled
    AbstractControllerRelation(AbstractControllerRelation &&) = delete;
    AbstractControllerRelation & operator=(AbstractControllerRelation &&) = delete;

    /*! \brief Set parent controller
     *
     * \pre \a controller must be a valid pointer
     */
    void setParentController(AbstractController *controller);

    /*! \brief Set child controller
     *
     * \pre \a controller must be a valid pointer
     */
    void setChildController(AbstractController *controller);

    /*! \brief Get parent controller
     */
    AbstractController *parentController() const;

    /*! \brief Get child controller
     *
     * Returns a nullptr until child controller was set.
     */
    AbstractController *childController() const;

    /*! \brief Get parent controller's model
     *
     * Can return a nullptr if parent controller has no model set.
     */
    QAbstractItemModel *parentControllerModel() const;

    /*! \brief Get child controller's model
     * Can return a nullptr if child controller was not set,
     *  or it has no model set.
     */
    QAbstractItemModel *childControllerModel() const;

   protected:

    /*! \brief Called each time parent controller is about to leave this relation
     */
    virtual void parentControllerAboutToChangeEvent(AbstractController *oldController);

    /*! \brief Called each time parent controller changed
     */
    virtual void parentControllerChangedEvent(AbstractController *controller);

    /*! \brief Called each time child controller is about to leave this relation
     */
    virtual void childControllerAboutToChangeEvent(AbstractController *oldController);

    /*! \brief Called each time child controller changed
     */
    virtual void childControllerChangedEvent(AbstractController *controller);

    /*! \brief Called each time parent controller's model changes to a non null model
     */
    virtual void parentControllerModelChangedEvent(QAbstractItemModel *model);

    /*! \brief Called each time child controller's model changes to a non null model
     */
    virtual void childControllerModelChangedEvent(QAbstractItemModel *model);


    /*! \brief Parent controller's state changed event
     */
    virtual void parentControllerStateChangedEvent(ControllerState parentState);

    /*! \brief Child controller's state changed event
     */
    virtual void childControllerStateChangedEvent(ControllerState childState);

   private slots:

    /*! \brief Set parent controller state
     */
    void onParentControllerStateChanged(Mdt::ItemEditor::ControllerState newState);

    /*! \brief Set child controller state
     */
    void onChildControllerStateChanged(Mdt::ItemEditor::ControllerState newState);

 private:

//     bool mustUpdateParentControllerState(ControllerState state) const;

    QPointer<AbstractController> mParentController;
    QPointer<AbstractController> mChildController;
    QMetaObject::Connection mParentControllerModelChangedConnection;
    QMetaObject::Connection mChildControllerModelChangedConnection;
    QMetaObject::Connection mParentControllerStateChagedConnection;
    QMetaObject::Connection mChildControllerStateChagedConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_H
