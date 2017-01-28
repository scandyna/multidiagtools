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
//   class RelationKeyCopier;

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

    /*! \brief Register child controller
     *
     * \pre \a conditions must be a valid relation filter expression
     */
    void registerChildController(const Mdt::ItemModel::RelationFilterExpression & conditions);

    /*! \brief Unregister child controller
     */
    void unregisterChildController();

   /*! \brief Actions to perform in a specific state of the parent controller
    */
   void onParentControllerStateChaged(Mdt::ItemEditor::ControllerState newState);

   /*! \brief Actions to perform in a specific state of the child controller
    */
   void onChildControllerStateChaged(Mdt::ItemEditor::ControllerState newState);

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

    /*! \internal Access relation key copier
     *
     * \note When this relation is deleted, the returned pointer becomes dangling.
     *       This method is used for unit tests.
     */
//     Mdt::ItemModel::RelationKeyCopier *relationKeyCopier() const
//     {
//       return mKeyCopier.get();
//     }

   private:

    void onParentControllerModelChanged(QAbstractItemModel *model);

    std::unique_ptr<Mdt::ItemModel::RelationFilterProxyModel> mProxyModel;
//     std::unique_ptr<Mdt::ItemModel::RelationKeyCopier> mKeyCopier;
//     QMetaObject::Connection mParentModelCurrentRowChangedConnection1;
//     QMetaObject::Connection mParentModelCurrentRowChangedConnection2;
    QMetaObject::Connection mChildSourceModelChangedConnection1;
    QMetaObject::Connection mChildSourceModelChangedConnection2;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_H
