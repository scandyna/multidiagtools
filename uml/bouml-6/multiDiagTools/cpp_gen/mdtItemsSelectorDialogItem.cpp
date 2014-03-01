
#include "mdtItemsSelectorDialogItem.h"

void mdtItemsSelectorDialogItem::setText(QString value) 
{
  pvText = value;
}

void mdtItemsSelectorDialogItem::setColumnIndex(int value) 
{
  pvColumnIndex = value;
}

void mdtItemsSelectorDialogItem::setFieldName(QString value) 
{
  pvFieldName = value;
}

void mdtItemsSelectorDialogItem::setSortOrder(Qt::SortOrder value) 
{
  pvSortOrder = value;
}

