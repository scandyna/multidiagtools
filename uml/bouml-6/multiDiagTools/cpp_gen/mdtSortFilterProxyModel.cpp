
#include "mdtSortFilterProxyModel.h"

mdtSortFilterProxyModel::mdtSortFilterProxyModel(const QObject & parent) {
}

mdtSortFilterProxyModel::~mdtSortFilterProxyModel() {
}

void mdtSortFilterProxyModel::clearColumnsSortOrder() 
{
}

void mdtSortFilterProxyModel::addColumnToSortOrder(int column, const Qt::SortOrder & order) 
{
}

void mdtSortFilterProxyModel::addColumnToSortOrder(const QString & fieldName, const Qt::SortOrder & order) 
{
}

QVector mdtSortFilterProxyModel::sortedColumns() 
{
}

bool mdtSortFilterProxyModel::lessThan(const QModelIndex & left, const QModelIndex & right) {
}

