#include "mdtParentChildTableItem.h"
#include <iostream>

#include <QDebug>

using namespace std;

mdtParentChildTableItem::mdtParentChildTableItem(QAbstractItemModel *model, int modelRow, mdtParentChildTableItem *parent)
{
  pvModel = model;
  pvModelRow = modelRow;
  pvParentItem = parent;
}

mdtParentChildTableItem::mdtParentChildTableItem()
{
  pvModel = 0;
  pvModelRow = 0;
  pvParentItem = 0;
}

mdtParentChildTableItem::~mdtParentChildTableItem()
{
  // Delete each items in the list
  qDeleteAll(pvChildItems);
}

void mdtParentChildTableItem::appendChild(mdtParentChildTableItem *child)
{
  pvChildItems.append(child);
}

mdtParentChildTableItem *mdtParentChildTableItem::child(int row)
{
  return pvChildItems.value(row);
}

int mdtParentChildTableItem::childCount() const
{
  return pvChildItems.count();
}

int mdtParentChildTableItem::row() const
{
  if(pvParentItem == 0){
     return 0;
  }
  return pvParentItem->pvChildItems.indexOf(const_cast<mdtParentChildTableItem*>(this));
}

QAbstractItemModel *mdtParentChildTableItem::model()
{
  return pvModel;
}

int mdtParentChildTableItem::columnCount() const
{
  Q_ASSERT(pvModel != 0);
  return pvModel->columnCount();
}

QVariant mdtParentChildTableItem::data(int column, int role) const
{
  if(pvModel == 0){
    return QVariant();
  }
  return pvModel->data(pvModel->index(pvModelRow, column), role);
}

mdtParentChildTableItem *mdtParentChildTableItem::parent()
{
  Q_ASSERT(pvParentItem != 0);
  return pvParentItem;
}
