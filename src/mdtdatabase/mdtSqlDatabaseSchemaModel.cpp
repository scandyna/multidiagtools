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
#include <QColor>

#include <QDebug>

/*
 * mdtSqlDatabaseSchemaModelItem implementation
 */

/*! \internal Data structure to hold position in the tree
 *
 * Note: this class mostly only contains
 *  informations about the tree structure,
 *  no data related to database schema is stored here.
 */
class mdtSqlDatabaseSchemaModelItem
{
 public:

  /*! \internal Item type
   */
  enum Type{
    Root,
    ObjectCategory,
    ObjectName
  };

  /*! \internal Constructor
   */
  mdtSqlDatabaseSchemaModelItem(Type t, mdtSqlDatabaseSchemaModel::ObjectCategory c, mdtSqlDatabaseSchemaModelItem *parent)
   : pvType(t),
     pvCategory(c),
     pvParentItem(parent),
     pvProgress(0),
     pvStatus(mdtSqlDatabaseSchemaModel::mdtSqlDatabaseSchemaModel::StatusUnknown)
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

  /*! \internal Get item category
   */
  mdtSqlDatabaseSchemaModel::ObjectCategory category() const
  {
    return pvCategory;
  }

  /*! \brief Set status
   */
  void setStatus(mdtSqlDatabaseSchemaModel::Status s){
    pvStatus = s;
  }

  /*! \brief Get status
   */
  mdtSqlDatabaseSchemaModel::Status status() const
  {
    return pvStatus;
  }

  /*! \brief Set status text
   */
  void setStatusText(const QString & text)
  {
    pvStatusText = text;
  }

  /*! \brief Get status text
   */
  QString statusText() const
  {
    return pvStatusText;
  }

  /*! \internal Add a child of type t and category c
   */
  mdtSqlDatabaseSchemaModelItem *addChild(Type t, mdtSqlDatabaseSchemaModel::ObjectCategory c)
  {
    mdtSqlDatabaseSchemaModelItem *item = new mdtSqlDatabaseSchemaModelItem(t, c, this);
    pvChildItems.append(item);
    return item;
  }

  /*! \internal Add n children of type t and category c
   */
  void addChildren(int n, Type t, mdtSqlDatabaseSchemaModel::ObjectCategory c)
  {
    for(int i = 0; i < n; ++i){
      pvChildItems.append(new mdtSqlDatabaseSchemaModelItem(t, c, this));
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

  /*! \brief Set progress
   */
  void setProgress(int x)
  {
    pvProgress = x;
  }

  /*! \brief Get progress
   */
  int progress() const
  {
    return pvProgress;
  }

 private:

  Type pvType;
  mdtSqlDatabaseSchemaModel::ObjectCategory pvCategory;
  mdtSqlDatabaseSchemaModelItem *pvParentItem;
  int pvProgress;
  mdtSqlDatabaseSchemaModel::Status pvStatus;
  QString pvStatusText;
  QVector<mdtSqlDatabaseSchemaModelItem*> pvChildItems;
};

/*
 * mdtSqlDatabaseSchemaModel
 */

mdtSqlDatabaseSchemaModel::mdtSqlDatabaseSchemaModel(QObject* parent)
 : QAbstractItemModel(parent)
{
  pvRootItem = new mdtSqlDatabaseSchemaModelItem(mdtSqlDatabaseSchemaModelItem::Root, Invalid, nullptr);
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
  auto *tables = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ObjectCategory, Table);
  tables->addChildren(s.tableCount(), mdtSqlDatabaseSchemaModelItem::ObjectName, Table);
  auto *views = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ObjectCategory, View);
  views->addChildren(s.viewCount(), mdtSqlDatabaseSchemaModelItem::ObjectName, View);
  auto *tps = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ObjectCategory, TablePopulation);
  tps->addChildren(s.tablePopulationCount(), mdtSqlDatabaseSchemaModelItem::ObjectName, TablePopulation);
  auto *triggers = pvRootItem->addChild(mdtSqlDatabaseSchemaModelItem::ObjectCategory, Trigger);
  triggers->addChildren(s.triggerCount(), mdtSqlDatabaseSchemaModelItem::ObjectName, Trigger);
  endResetModel();
}

QModelIndex mdtSqlDatabaseSchemaModel::index(int row, int column, const QModelIndex & parent) const
{
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

  return QModelIndex();
}

QModelIndex mdtSqlDatabaseSchemaModel::parent(const QModelIndex & index) const
{
  if(!index.isValid()){
    return QModelIndex();
  }
  // Get item from given index and its parent item
  auto *item = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(index.internalPointer());
  Q_ASSERT(item != nullptr);
  auto parentItem = item->parentItem();
  // We not let root item accessible from view
  if(item == pvRootItem){
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int mdtSqlDatabaseSchemaModel::rowCount(const QModelIndex & parent) const
{
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
    return 3;
  }
  return 1;
}

QVariant mdtSqlDatabaseSchemaModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  // Get item referenced by index
  auto *item = reinterpret_cast<mdtSqlDatabaseSchemaModelItem*>(index.internalPointer());
  Q_ASSERT(item != nullptr);
  // Return data regarding column
  int col = index.column();
  if(col == ObjectColumnIndex){
    // Return data regarding item's type
    switch(item->type()){
      case mdtSqlDatabaseSchemaModelItem::Root:
        return QVariant();
      case mdtSqlDatabaseSchemaModelItem::ObjectCategory:
        return categoryData(item, role);
      case mdtSqlDatabaseSchemaModelItem::ObjectName:
        return nameData(index, item->category(), role);
    }
  }else if(col == ProgressColumnIndex){
    if(role != Qt::DisplayRole){
      return QVariant();
    }
    return item->progress();
  }else if(col == StatusColunIndex){
    return statusData(item, role);
  }

  return QVariant();
}

// void mdtSqlDatabaseSchemaModel::setProgress(mdtSqlDatabaseSchemaModel::ObjectCategory objectCategory, int value)
// {
//   auto *item = getItem(objectCategory);
// 
//   if(item != nullptr){
//     item->setProgress(value);
//     auto index = createIndex(item->row(), ProgressColumnIndex, item);
//     emit dataChanged(index, index);
//   }
// }

void mdtSqlDatabaseSchemaModel::setProgress(mdtSqlDatabaseSchemaModel::ObjectCategory objectCategory, const QString& objectName, int value)
{
  mdtSqlDatabaseSchemaModelItem *item;

  if(objectName.isEmpty()){
    item = getItem(objectCategory);
  }else{
    item = getItem(objectCategory, objectName);
  }
  if(item == nullptr){
    return;
  }
  item->setProgress(value);
  auto index = createIndex(item->row(), ProgressColumnIndex, item);
  emit dataChanged(index, index);
}

void mdtSqlDatabaseSchemaModel::setStatus(mdtSqlDatabaseSchemaModel::ObjectCategory objectCategory, const QString& objectName,
                                          mdtSqlDatabaseSchemaModel::Status status, const QString & statusText)
{
  mdtSqlDatabaseSchemaModelItem *item;

  if(objectName.isEmpty()){
    item = getItem(objectCategory);
  }else{
    item = getItem(objectCategory, objectName);
  }
  if(item == nullptr){
    return;
  }
  item->setStatus(status);
  item->setStatusText(statusText);
  auto index = createIndex(item->row(), StatusColunIndex, item);
  emit dataChanged(index, index);
}

void mdtSqlDatabaseSchemaModel::clearStatusAndProgress()
{
  setStatusAndProgressRecursiv(pvRootItem, mdtSqlDatabaseSchemaModel::StatusUnknown, 0);
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

  switch(item->category()){
    case Table:
      return tr("Tables");
    case View:
      return tr("Views");
    case TablePopulation:
      return tr("Table population");
    case Trigger:
      return tr("Triggers");
    default:
      return QVariant();
  }

  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::nameData(const QModelIndex & index, ObjectCategory oc, int role) const
{
  Q_ASSERT(index.isValid());

  switch(oc){
    case Invalid:
      return QVariant();
    case Table:
      return tableSchemaData(index, role);
    case View:
      return viewSchemaData(index, role);
    case TablePopulation:
      return tablePopulationSchemaData(index, role);
    case Trigger:
      return triggerSchemaData(index, role);
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

QVariant mdtSqlDatabaseSchemaModel::triggerSchemaData(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.row() < pvSchema.triggerCount());

  if(role == Qt::DisplayRole){
    return pvSchema.triggerName(index.row());
  }
  return QVariant();
}

QVariant mdtSqlDatabaseSchemaModel::statusData(mdtSqlDatabaseSchemaModelItem* item, int role) const
{
  switch(item->status()){
    case mdtSqlDatabaseSchemaModel::StatusUnknown:
      return QVariant();
    case mdtSqlDatabaseSchemaModel::StatusOk:
      switch(role){
        case Qt::DisplayRole:
          return tr("Ok");
        case Qt::DecorationRole:
          return QColor(Qt::green);
        default:
          return QVariant();
      }
    case mdtSqlDatabaseSchemaModel::StatusError:
      switch(role){
        case Qt::DisplayRole:
          return tr("Error");
        case Qt::DecorationRole:
          return QColor(Qt::red);
        case Qt::ToolTipRole:
          return item->statusText();
        default:
          return QVariant();
      }
  }

  return QVariant();
}

mdtSqlDatabaseSchemaModelItem* mdtSqlDatabaseSchemaModel::getItem(mdtSqlDatabaseSchemaModel::ObjectCategory oc) const
{
  /*
   * The root item's children are category items.
   */
  for(int row = 0; row < pvRootItem->childCount(); ++row){
    auto *item = pvRootItem->childItem(row);
    Q_ASSERT(item != nullptr);
    if(item->category() == oc){
      return item;
    }
  }

  return nullptr;
}

mdtSqlDatabaseSchemaModelItem* mdtSqlDatabaseSchemaModel::getItem(mdtSqlDatabaseSchemaModel::ObjectCategory oc, const QString & objectName) const
{
  auto *categoryItem = getItem(oc);
  if(categoryItem == nullptr){
    return nullptr;
  }
  for(int row = 0; row < categoryItem->childCount(); ++row){
    auto *item = categoryItem->childItem(row);
    Q_ASSERT(item != nullptr);
    auto index = createIndex(item->row(), 0, item);
    if(data(index).toString() == objectName){
      return item;
    }
  }

  return nullptr;
}

void mdtSqlDatabaseSchemaModel::setStatusAndProgressRecursiv(mdtSqlDatabaseSchemaModelItem* rootItem, mdtSqlDatabaseSchemaModel::Status status, int progress)
{
  Q_ASSERT(rootItem != nullptr);

  for(int row = 0; row < rootItem->childCount(); ++row){
    auto *item = rootItem->childItem(row);
    Q_ASSERT(item != nullptr);
    item->setStatus(status);
    item->setProgress(progress);
    /*
     * We signal the change for each conecrend index.
     * It could be faster to make 1 beginResetModel() then 1 endResetModel(),
     * but this resets also the view, and the user has to resize alls stuff -> bad.
     */
    auto index1 = createIndex(item->row(), StatusColunIndex, item);
    auto index2 = createIndex(item->row(), ProgressColumnIndex, item);
    emit dataChanged(index1, index2);
    setStatusAndProgressRecursiv(item, status, progress);
  }
}
