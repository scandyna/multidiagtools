/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlDatabaseSchemaModel.h"
#include <QVector>

#include <QDebug>

/*
 * mdtSqlDatabaseSchemaModelItem implementation
 */

/*! \internal Data structure to hold position in the tree
 *
 * Note: this class only contains
 *  informations about the tree structure,
 *  no data is stored here.
 */
class mdtSqlDatabaseSchemaModelItem
{
 public:

  /*! \internal Item type
   */
  enum Type{
    Root,
    TablesCategory,
    TableData,
    ViewsCategory,
    ViewData,
    TablePopulationsCategory,
    TablePopulationData
  };

  /*! \brief Internal Constructor
   */
  mdtSqlDatabaseSchemaModelItem(Type t, mdtSqlDatabaseSchemaModelItem *parent)
   : pvType(t),
     pvParentItem(parent)
  {
    Q_ASSERT( ((t == Root) && (parent == nullptr)) || ((t != Root) && (parent != nullptr)) );
  }

  /*! \internal Destructor
   */
  ~mdtSqlDatabaseSchemaModelItem()
  {
    qDeleteAll(pvChildItems);
  }

  /*! \internal Get item type
   */
  Type type() const
  {
    return pvType;
  }

  /*! \internal Add a child of type t
   */
  mdtSqlDatabaseSchemaModelItem *addChild(Type t)
  {
    mdtSqlDatabaseSchemaModelItem *item = new mdtSqlDatabaseSchemaModelItem(t, this);
    pvChildItems.append(item);
    return item;
  }

  /*! \internal Add n children of type t
   */
  void addChildren(int n, Type t)
  {
    for(int i = 0; i < n; ++i){
      pvChildItems.append(new mdtSqlDatabaseSchemaModelItem(t, this));
    }
  }

  /*! \internal Get child count
   */
  int childCount() const
  {
    return pvChildItems.size();
  }

  /*! \internal Get child item for given row - can also return a nullptr
   */
  mdtSqlDatabaseSchemaModelItem *childItem(int row)
  {
    return pvChildItems.value(row, nullptr);
  }

  /// \todo define if needed, and update columnCount() in model
  int columnCount() const
  {
    return 1;
  }

  /*! \internal Get the row position of this item in its parent
   */
  int row() const
  {
    if(pvParentItem != nullptr){
      return pvParentItem->pvChildItems.indexOf(const_cast<mdtSqlDatabaseSchemaModelItem*>(this));
    }
    return 0;
  }

  /*! \internal Get parent item - Can be a nullptr (case of root item)
   */
  mdtSqlDatabaseSchemaModelItem *parentItem()
  {
    return pvParentItem;
  }

  /*! \brief Clear - will destroy all child items
   */
  void clear()
  {
    qDeleteAll(pvChildItems);
    pvChildItems.clear();
  }

 private:

  Type pvType;
  QVector<mdtSqlDatabaseSchemaModelItem*> pvChildItems;
  mdtSqlDatabaseSchemaModelItem *pvParentItem;
};

/*
 * mdtSqlDatabaseSchemaModel
 */

mdtSqlDatabaseSchemaModel::mdtSqlDatabaseSchemaModel(QObject* parent)
 : QAbstractItemModel(parent)
{
  /*
   * Build some tree:
   * root
   *  |- Tables
   *  |    |- 0
   *  |    |- 1
   *  |- Views
   *       |- 0
   */
  pvRootItem = new mdtSqlDatabaseSchemaModelItem(mdtSqlDatabaseSchemaModelItem::Root, nullptr);
  
//   mdtSqlDatabaseSchemaModelItem *tables = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::TablesCategory);
//   tables->addChild(mdtSqlDatabaseSchemaModelItem::TableData);
//   tables->addChild(mdtSqlDatabaseSchemaModelItem::TableData);
//   mdtSqlDatabaseSchemaModelItem *views = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ViewsCategory);
//   views->addChild(mdtSqlDatabaseSchemaModelItem::ViewData);
}

mdtSqlDatabaseSchemaModel::~mdtSqlDatabaseSchemaModel()
{
  delete pvRootItem;
}

void mdtSqlDatabaseSchemaModel::setSchema(const mdtSqlDatabaseSchema & s)
{
  beginResetModel();
  pvSchema = s;
  pvRootItem->clear();
  auto *tables = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::TablesCategory);
  tables->addChildren(s.tableCount(), mdtSqlDatabaseSchemaModelItem::TableData);
  auto *views = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ViewsCategory);
  views->addChildren(s.viewCount(), mdtSqlDatabaseSchemaModelItem::ViewData);
  auto *tps = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::TablePopulationsCategory);
  tps->addChildren(s.tablePopulationCount(), mdtSqlDatabaseSchemaModelItem::TablePopulationData);
  endResetModel();
}

QModelIndex mdtSqlDatabaseSchemaModel::index(int row, int column, const QModelIndex & parent) const
{
  qDebug() << "REQ index for row " << row << " col " << column << " parent " << parent;

  // Get parent item
  mdtSqlDatabaseSchemaModelItem *parentItem;
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(parent.internalPointer());
  }
  Q_ASSERT(parentItem != nullptr);
  // Get item for given row, from parent item
  auto *item = parentItem->childItem(row);
  Q_ASSERT(item != pvRootItem);
  if(item != nullptr){
    return createIndex(row, column, item);
  }
  qDebug() << " -> No item found";
  return QModelIndex();
}

QModelIndex mdtSqlDatabaseSchemaModel::parent(const QModelIndex & index) const
{
  qDebug() << "REQ parent for index " << index;

  if(!index.isValid()){
    return QModelIndex();
  }
  // Get item from given index and its parent item
  auto *item = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(index.internalPointer());
  Q_ASSERT(item != nullptr);
  auto parentItem = item->parentItem();
  ///Q_ASSERT(parentItem != nullptr);
  // We not let root item accessible from view
  if(item == pvRootItem){
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int mdtSqlDatabaseSchemaModel::rowCount(const QModelIndex & parent) const
{
  qDebug() << "REQ rowCount for parent " << parent;

  /// \todo Check what to do with parent's column
  if(parent.column() > 0){
    return 0;
  }
  // Get parent item
  mdtSqlDatabaseSchemaModelItem *parentItem;
  if(!parent.isValid()){
    parentItem = pvRootItem;
  }else{
    parentItem = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(parent.internalPointer());
  }
  Q_ASSERT(parentItem != nullptr);

  return parentItem->childCount();
}

int mdtSqlDatabaseSchemaModel::columnCount(const QModelIndex & parent) const
{
  /*
   * It's no really clear that should be done here.
   * Assume that:
   *  - parent index is not valid (case of a top level index), we return number of columns
   *  - parent index is valid, we declare 1 column
   */
  if(!parent.isValid()){
    return 2;
  }
  return 1;
}

QVariant mdtSqlDatabaseSchemaModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if(index.column() != ObjectColumnIndex){
    return QVariant();
  }
  // Get item referenced by index
  auto *item = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(index.internalPointer());
  Q_ASSERT(item != nullptr);
  // Return data regarding item's type
  switch(item->type()){
    case mdtSqlDatabaseSchemaModelItem::Root:
      return QVariant();
    case mdtSqlDatabaseSchemaModelItem::TablesCategory:
    case mdtSqlDatabaseSchemaModelItem::ViewsCategory:
    case mdtSqlDatabaseSchemaModelItem::TablePopulationsCategory:
      return categoryData(item, role);
    case mdtSqlDatabaseSchemaModelItem::TableData:
      return tableSchemaData(index, role);
    case mdtSqlDatabaseSchemaModelItem::ViewData:
      return viewSchemaData(index, role);
    case mdtSqlDatabaseSchemaModelItem::TablePopulationData:
      return tablePopulationSchemaData(index, role);
  }

  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::categoryData(mdtSqlDatabaseSchemaModelItem* item, int role) const
{
  Q_ASSERT(item != nullptr);

  switch(role){
    case Qt::DisplayRole:
      return categoryDisplayRoleData(item);
    default:
      return QVariant();
  }
  return QVariant();

}

QVariant mdtSqlDatabaseSchemaModel::categoryDisplayRoleData(mdtSqlDatabaseSchemaModelItem* item) const
{
  Q_ASSERT(item != nullptr);

  switch(item->type()){
    case mdtSqlDatabaseSchemaModelItem::TablesCategory:
      return tr("Tables");
    case mdtSqlDatabaseSchemaModelItem::ViewsCategory:
      return tr("Views");
    case mdtSqlDatabaseSchemaModelItem::TablePopulationsCategory:
      return tr("Table population");
    default:
      return QVariant();
  }

  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::tableSchemaData(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.row() < pvSchema.tableCount());

  if(role == Qt::DisplayRole){
    return pvSchema.tableName(index.row());
  }

  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::viewSchemaData(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.row() < pvSchema.viewCount());

  if(role == Qt::DisplayRole){
    return pvSchema.viewName(index.row());
  }
  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::tablePopulationSchemaData(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.row() < pvSchema.tablePopulationCount());

  if(role == Qt::DisplayRole){
    return pvSchema.tablePopulationName(index.row());
  }
  return QVariant();
}
