#include "mdtTreeItem.h"
#include <iostream>
#include <QDebug>

using namespace std;

mdtTreeItem::mdtTreeItem(QAbstractItemModel *model, int modelRow, mdtTreeItem *parent)
{
  pvParentItem = parent;
  pvModel = model;
  pvModelRow = modelRow;
}

mdtTreeItem::~mdtTreeItem()
{
  // Delete each items in the list
  qDeleteAll(pvChildItems);
}

void mdtTreeItem::appendChild(mdtTreeItem *child)
{
  pvChildItems.append(child);
}

mdtTreeItem *mdtTreeItem::child(int row)
{
  return pvChildItems.value(row);
}

int mdtTreeItem::childCount() const
{
  return pvChildItems.count();
}

int mdtTreeItem::row() const
{
  if(pvParentItem == 0){
     return 0;
  }
  return pvParentItem->pvChildItems.indexOf(const_cast<mdtTreeItem*>(this));
}

int mdtTreeItem::columnCount() const
{
  //return pvItemData.count();
  return 2;
}

QVariant mdtTreeItem::data(int column) const
{
  if(pvModel == 0){
    return QVariant();
  }
  return pvModel->data(pvModel->index(pvModelRow, column)); /// NOTE; role
}

mdtTreeItem *mdtTreeItem::parent()
{
  return pvParentItem;
}
