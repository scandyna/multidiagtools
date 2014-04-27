
#include "mdtSqlTableSelection.h"
#include "mdtSqlTableSelectionRow.h"

mdtSqlTableSelection::mdtSqlTableSelection() 
{
}

void mdtSqlTableSelection::clear() 
{
}

void mdtSqlTableSelection::addIndex(const QModelIndex & index, const QSqlTableModel & model) 
{
}

void mdtSqlTableSelection::addIndexList(const QModelIndex & indexList, const QStringList & fieldList, const QSqlTableModel & model) 
{
}

void mdtSqlTableSelection::setIndexList(const QModelIndex & indexList, const QStringList & fieldList, const QSqlTableModel & model) 
{
}

int mdtSqlTableSelection::rowCount() const 
{
}

QVariant mdtSqlTableSelection::data(int row, const QString & fieldName) const 
{
}

QVariant mdtSqlTableSelection::dataList(const QString & fieldName) const 
{
}

