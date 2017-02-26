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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_H

#include "ControllerRelationImplBase.h"
#include "Mdt/ItemModel/RelationFilterExpression.h"
#include <QObject>
#include <QPointer>
#include <memory>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  class RelationFilterProxyModel;

}} // namespace Mdt{ namespace ItemModel{

namespace Mdt{ namespace ItemEditor{

  class AbstractController;

  /*! \brief Relation between a parent controller and a child controller
   *
   * ControllerRelationImpl does not own any controller,
   *  deleting them is the responsability of the user of this class.
   */
  class ControllerRelation : public ControllerRelationImplBase<AbstractController, ControllerRelation>
  {
   public:

    /*! \brief Construct a relation
     *
     * \pre \a parentController must be a valid pointer
     */
    explicit ControllerRelation(AbstractController *parentController);

    /*! \brief Destructor
     */
    ~ControllerRelation();

    /*! \brief Set relation filter
     *
     * \pre \a conditions must be a valid relation filter expression
     */
    void setSpecificRelationFilter(const Mdt::ItemModel::RelationFilterExpression & conditions);

    /*! \brief Set parent controller's model to child controller
     *
     * \todo Possibly obselete
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

    /*! \brief Register child controller
     */
    void registerChildController() override;

    /*! \brief Unregister child controller
     */
    void unregisterChildController() override;

    /*! \brief Set relation filter from relation key
     */
    void setFilterFromRelationKey(const Mdt::ItemModel::RelationKey & key) override;

    void onParentControllerModelChanged(QAbstractItemModel *model);

    std::unique_ptr<Mdt::ItemModel::RelationFilterProxyModel> mProxyModel;
    QMetaObject::Connection mChildSourceModelChangedConnection1;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
