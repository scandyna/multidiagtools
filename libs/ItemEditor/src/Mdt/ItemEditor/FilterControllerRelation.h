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
#ifndef MDT_ITEM_EDITOR_FILTER_CONTROLLER_RELATION_H
#define MDT_ITEM_EDITOR_FILTER_CONTROLLER_RELATION_H

#include "AbstractControllerRelation.h"
#include "Mdt/ItemModel/RelationFilterExpression.h"
#include <QPointer>
#include <memory>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  class RelationFilterProxyModel;
  class RelationKey;

}} // namespace Mdt{ namespace ItemModel{

namespace Mdt{ namespace ItemEditor{

  /*! \brief Relation between a parent controller and a child controller
   *
   * FilterControllerRelation does not own any controller,
   *  deleting them is the responsability of the user of this class.
   */
  class FilterControllerRelation : public AbstractControllerRelation
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit FilterControllerRelation(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~FilterControllerRelation();

    /*! \brief Set relation filter
     *
     * \pre \a conditions must be a valid relation filter expression
     */
    void setRelationFilter(const Mdt::ItemModel::RelationFilterExpression & conditions);

    /*! \brief Set relation filter from parent controller's primary key and child controller's foreign key
     *
     * \pre child controller must allready been set
     * \pre parent controller must have a non null primary key set
     * \pre child controller must have a non null foreign key set
     * \pre Both primary of parent controller and foreign key of child controller must have the same count of columns, and max 4
     */
    void setRelationFilterFromPkFk();

    /*! \internal Access relation filter proxy model
     *
     * \note When this relation is deleted, the returned pointer becomes dangling.
     *       This method is used for unit tests.
     */
    Mdt::ItemModel::RelationFilterProxyModel *relationFilterModel() const
    {
      return mProxyModel.get();
    }

   private slots:

    void setParentModelMatchRow(int row);

   private:

    /*! \brief Set relation filter from relation key
     */
    void setFilterFromRelationKey(const Mdt::ItemModel::RelationKey & key);

    void parentControllerChangedEvent(AbstractController* controller) override;
    void childControllerAboutToChangeEvent(AbstractController* oldController) override;
    void childControllerChangedEvent(AbstractController* controller) override;
    void parentControllerModelChangedEvent(QAbstractItemModel* model) override;
//     void childControllerModelChangedEvent(QAbstractItemModel* model) override;

    std::unique_ptr<Mdt::ItemModel::RelationFilterProxyModel> mProxyModel;
    QMetaObject::Connection mSetParentCurrentRowConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_FILTER_CONTROLLER_RELATION_H