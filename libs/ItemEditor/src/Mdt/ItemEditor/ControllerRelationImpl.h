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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_H

#include "ControllerRelation.h"
#include "Mdt/ItemModel/RelationFilterExpression.h"
#include <QObject>
#include <QPointer>
#include <memory>

namespace Mdt{ namespace ItemModel{

  class RelationFilterProxyModel;
  class RelationKeyCopier;

}} // namespace Mdt{ namespace ItemModel{

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Relation between a parent controller and a child controller
   *
   * ControllerRelationImpl does not own any controller,
   *  deleting them is the responsability of the user of this class.
   */
  class ControllerRelationImpl : public ControllerRelation<AbstractController, ControllerRelationImpl>
  {
   public:

    /*! \brief Construct a relation
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelationImpl(AbstractController *parentController);

    /*! \brief Destructor
     */
    ~ControllerRelationImpl();

    /*! \brief Register child controller
     *
     * \pre \a conditions must be a valid relation filter expression
     */
    void registerChildController(const Mdt::ItemModel::RelationFilterExpression & conditions);

    /*! \brief Unregister child controller
     */
    void unregisterChildController();

    /*! \brief Set parent controller's model to child controller
     */
    void setParentControllerModelToChildController();

    /*! \internal Access relation filter proxy model
     *
     * \note When this relation is deleted, the returned pointer becomes dangling.
     *       This method is used for unit tests.
     */
    Mdt::ItemModel::RelationFilterProxyModel *relationFilterModel() const
    {
      return mProxyModel.get();
    }

   private:

    std::unique_ptr<Mdt::ItemModel::RelationFilterProxyModel> mProxyModel;
    std::unique_ptr<Mdt::ItemModel::RelationKeyCopier> mKeyCopier;
    QMetaObject::Connection mParentModelCurrentRowChangedConnection;
    QMetaObject::Connection mChildSourceModelChangedConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_IMPL_H
