#ifndef MDT_TREE_ITEM_H
#define MDT_TREE_ITEM_H

#include <QList>
#include <QVariant>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStringList>

class mdtTreeItem
{
 public:

  mdtTreeItem(QAbstractItemModel *model = 0, int modelRow = 0, mdtTreeItem *parent = 0);
  ~mdtTreeItem();

  // Append a new child in the treeItem
  void appendChild(mdtTreeItem *child);

  // Get child for given row
  mdtTreeItem *child(int row);
  
  int childCount() const;
  
  mdtTreeItem *parent();
  
  int columnCount() const;
  
  QVariant data(int column) const;
  
  // Returns the row position for this instance
  int row() const;

 private:

  // Tree structure members
  QList<mdtTreeItem*> pvChildItems;
  mdtTreeItem *pvParentItem;

  // Referenced table and index
  QAbstractItemModel *pvModel;
  int pvModelRow;
};

#endif
