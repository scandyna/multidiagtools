/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_H

#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Relation between a parent controller and a child controller
   *
   * ControllerRelation does not own any controller,
   *  deleting them is the responsability of the user of this class.
   */
  class ControllerRelation
  {
   public:

    /*! \brief Construct a relation
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelation(AbstractController *parentController);

    /*! \brief Set child controller
     *
     * Will replace model in \a controller with a RelationFilterProxyModel.
     *
     * \pre \a controller must be a valid pointer
     * \pre \a conditions must be a valid relation filter expression
     */
    template<typename Expr>
    void setChildController(AbstractController *controller, const Expr & conditions)
    {
      Q_ASSERT(controller != nullptr);
      mChildController = controller;
      updateChildControllerModel();
      mProxyModel.setFilter(conditions);
    }

    /*! \brief Update parent controller model
     */
    void updateParentControllerModel();

    /*! \brief Update child controller model
     */
    void updateChildControllerModel();

    /*! \brief Access relation filter proxy model
     */
    Mdt::ItemModel::RelationFilterProxyModel *proxyModel() /*const*/
    {
      return &mProxyModel;
    }

   private:

    QPointer<AbstractController> mParentController;
    QPointer<AbstractController> mChildController;
    Mdt::ItemModel::RelationFilterProxyModel mProxyModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
