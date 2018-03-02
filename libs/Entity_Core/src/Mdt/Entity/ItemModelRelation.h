/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_ITEM_MODEL_RELATION_H
#define MDT_ENTITY_ITEM_MODEL_RELATION_H

#include "Relation.h"
#include "FieldAt.h"
#include "PrimaryKey.h"
// #include "ForeignKey.h"
#include "Mdt/ItemModel/RelationKey.h"
#include "MdtEntity_CoreExport.h"
#include <initializer_list>

#include <QDebug>

namespace Mdt{ namespace Entity{

  namespace Impl{ namespace ItemModelRelation{

    void addColumnPairToKeyImpl(Mdt::ItemModel::RelationKey &);

    template<typename PrimaryEntityDef, typename ForeignEntityDef, typename ForeignEntityField, typename ...ForeignEntityFields>
    void addColumnPairToKeyImpl(Mdt::ItemModel::RelationKey & imKey, const Mdt::Entity::PrimaryKey & pk)
    {
      const auto pkIndex = imKey.columnPairCount();
      Q_ASSERT(pkIndex < pk.fieldCount());

      imKey.addColumnPair( pk.fieldAt(pkIndex).fieldIndex(), fieldIndex<ForeignEntityDef, ForeignEntityField>() );
    }

    template<typename PrimaryEntity, typename ForeignEntity, typename ...ForeignEntityFields>
    void addColumnPairToKey(const Mdt::Entity::Relation<PrimaryEntity, ForeignEntity, ForeignEntityFields...> &, Mdt::ItemModel::RelationKey & imKey)
    {
      using primary_entity_def_type = typename PrimaryEntity::def_type;
      using foreign_entity_def_type = typename ForeignEntity::def_type;

      const auto pk = Mdt::Entity::PrimaryKey::fromEntity<PrimaryEntity>();
      Q_ASSERT(sizeof...(ForeignEntityFields) == pk.fieldCount());

      (void)std::initializer_list<int>{ (addColumnPairToKeyImpl<primary_entity_def_type, foreign_entity_def_type, ForeignEntityFields>(imKey, pk) ,0)... };
    }

  }} // namespace Impl{ namespace ItemModelRelation{

  /*! \brief Helper class to get a item model relation from a entity relation
   *
   * If a relation was defined using Relation,
   *  a basic detail view of a master / detail view can be created using Mdt::ItemModel:RelationFilterProxyModel .
   * \code
   * #include "TeamEmployeeRelation.h"  // Defines TeamEmployeeRelation using Relation
   * #include "TeamModel.h"             // Defines the Team model using ReadOnlyTableModel
   * #include "EmployeeModel.h"         // Defines the Employee model using ReadOnlyTableModel
   * #include <Mdt/ItemModel/RelationFilterProxyModel.h>
   * #include <Mdt/Entity/ItemModelRelation.h>
   * #include <QTableView>
   * #include <QApplication>
   *
   * using namespace Mdt::ItemModel;
   *
   * int main(int argc, char **argv)
   * {
   *   using Mdt::Entity::ItemModelRelation;
   *
   *   QApplication app(argc, argv);
   *   QTableView employeeView;
   *   auto *teamModel = new TeamModel(&employeeView);
   *   auto *employeeModel = new EmployeeModel(&employeeView);
   *   auto *proxyModel = new RelationFilterProxyModel(&employeeView);
   *
   *   // Fill the models with some data
   *   populateModels(teamModel, employeeModel);
   *
   *   // Setup the employee view
   *   proxyModel->setParentModel(teamModel);
   *   proxyModel->setSourceModel(employeeModel);
   *   proxyModel->setFilter( ItemModelRelation::keyFromRelation<TeamEmployeeRelation>() );
   *   proxyModel->setParentModelMatchRow(0);
   *   employeeView.setModel(proxyModel);
   *   employeeView.show();
   *
   *   return app.exec();
   * }
   * \endcode
   * To create a real master / detail view,
   *  some row changed event must be catched from the master view
   *  to update the detail view, which cannot be part of any core library.
   */
  class MDT_ENTITY_CORE_EXPORT ItemModelRelation
  {
   public:

    /*! \brief Get a item model relation key from a entity relation
     */
    template<typename EntityRelation>
    static Mdt::ItemModel::RelationKey keyFromRelation()
    {
      Mdt::ItemModel::RelationKey key;

      Impl::ItemModelRelation::addColumnPairToKey(EntityRelation{}, key);

      return key;
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ITEM_MODEL_RELATION_H
