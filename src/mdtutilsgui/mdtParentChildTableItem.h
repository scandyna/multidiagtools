#ifndef MDT_PARENT_CHILD_ITEM_H
#define MDT_PARENT_CHILD_ITEM_H

#include <QList>
#include <QVariant>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStringList>

class mdtParentChildTableItem
{
 public:

  /*! \brief Table item constructor
   * 
   * \param parent Set the parent reference of this object
   * \param model The table model to reference (let it null when creating the root object)
   * \param modelRow Set the row from the model that must be referenced
   */
  mdtParentChildTableItem(QAbstractItemModel *model, int modelRow, mdtParentChildTableItem *parent);

  /*! \brief Table item constructor
   * Use this constructor to create the root item of the model
   */
  mdtParentChildTableItem();

  ~mdtParentChildTableItem();

  // Append a new child in the treeItem
  void appendChild(mdtParentChildTableItem *child);

  // Get child for given row
  mdtParentChildTableItem *child(int row);
  
  int childCount() const;
  
  mdtParentChildTableItem *parent();
  
  /*! \brief Columns count of the stored model
   * \return Columns count of the stored model
   */
  int columnCount() const;
  
  /*! \brief Returns the data of the stored model at stored row
   * The row is set by constructor
   * \sa mdtParentChildTableItem()
   * \param column Column in witch to get the data
   * \param role Role of the data (Display, Edit, ...)
   */
  QVariant data(int column, int role) const;
  
  // Returns the row position for this instance
  int row() const;

  QAbstractItemModel *model();
  
 private:

  // Tree structure members
  QList<mdtParentChildTableItem*> pvChildItems;
  mdtParentChildTableItem *pvParentItem;

  // Referenced table and index
  QAbstractItemModel *pvModel;
  int pvModelRow;
};

#endif  // #ifdef MDT_PARENT_CHILD_ITEM_H
