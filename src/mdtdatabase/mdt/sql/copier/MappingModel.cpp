/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "MappingModel.h"
#include <QColor>

namespace mdt{ namespace sql{ namespace copier{

MappingModel::MappingModel(QObject *parent)
 : QAbstractTableModel(parent)
{
  connect(this, &QAbstractTableModel::modelReset, this, &MappingModel::resetCopyStatusAndProgress);
}

void MappingModel::setTableCopyProgress(int row, int progress)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyProgress.size());

  pvRowCopyProgress[row] = progress;
  auto idx = index(row, TableCopyProgressIndex);
  emit dataChanged(idx, idx);
}

void MappingModel::setTableCopyStatus(int row, int status)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyError.size());

  pvRowCopyStatus[row] = static_cast<CopyStatus>(status);
  pvRowCopyError[row] = mdtError();
  auto idx = index(row, TableCopyStatusIndex);
  emit dataChanged(idx, idx);
}

void MappingModel::setTableCopyError(int row, mdtError error)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyError.size());

  pvRowCopyStatus[row] = CopyStatusError;
  pvRowCopyError[row] = error;
  auto idx = index(row, TableCopyStatusIndex);
  emit dataChanged(idx, idx);
}

void MappingModel::clearCopyStatusAndProgress()
{
  for(int row = 0; row < rowCount(); ++row){
    pvRowCopyProgress[row] = 0;
    pvRowCopyStatus[row] = CopyStatusUnknown;
    pvRowCopyError[row].clear();
    auto idx1 = index(row, TableCopyProgressIndex);
    auto idx2 = index(row, TableCopyStatusIndex);
    emit dataChanged(idx1, idx2);
  }
}

int MappingModel::columnCount(const QModelIndex &/* parent*/) const
{
  return 5;
}

QVariant MappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex>(section)){
    case SourceTableNameIndex:
      return tr("Source");
    case DestinationTableNameIndex:
      return tr("Destination");
    case TableMappingStateIndex:
      return tr("Mapping state");
    case TableCopyProgressIndex:
      return tr("Copy progress");
    case TableCopyStatusIndex:
      return tr("Copy status");
  }

  return section;
}

QVariant MappingModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  const int row = index.row();
  Q_ASSERT( (row >= 0) && (row < rowCount()) );
  const auto column = static_cast<ColumnIndex>(index.column());
  switch(column){
    case SourceTableNameIndex:
      return sourceTableData(row, role);
    case DestinationTableNameIndex:
      return destinationTableData(row, role);
    case TableMappingStateIndex:
      return tableMappingStateData(row, role);
    case TableCopyProgressIndex:
      return tableCopyProgress(row, role);
    case TableCopyStatusIndex:
      return tableCopyStatusData(row, role);
  }

  return QVariant();
}

void MappingModel::resetCopyStatusAndProgress()
{
  pvRowCopyProgress.assign(rowCount(), 0);
  pvRowCopyStatus.assign(rowCount(), CopyStatusUnknown);
  pvRowCopyError.assign(rowCount(), mdtError());
}

QVariant MappingModel::sourceTableData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      // Return text informations
      return sourceTableName(row);
    default:
      return QVariant();
  }
}

QVariant MappingModel::destinationTableData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      // Return text informations
      return destinationTableName(row);
    default:
      return QVariant();
  }
}

QVariant MappingModel::tableMappingStateData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return tableMappingStateText(tableMappingState(row));
    case Qt::DecorationRole:
      return tableMappingStateDecoration(tableMappingState(row));
    default:
      return QVariant();
  }
}

QString MappingModel::tableMappingStateText(TableMapping::MappingState state) const
{
  switch(state){
    case TableMapping::MappingNotSet:
      return "";
    case TableMapping::MappingComplete:
      return tr("Ok");
    case TableMapping::MappingPartial:
      return tr("Partial");
    case TableMapping::MappingError:
      return tr("Error");
  }
  return "";
}

QVariant MappingModel::tableMappingStateDecoration(TableMapping::MappingState state) const
{
  switch(state){
    case TableMapping::MappingNotSet:
      return QVariant();
    case TableMapping::MappingComplete:
      return QColor(Qt::green);
    case TableMapping::MappingPartial:
      return QColor(Qt::yellow);
    case TableMapping::MappingError:
      return QColor(Qt::red);
  }
  return QVariant();
}

QVariant MappingModel::tableCopyProgress(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyProgress.size());

  if(role != Qt::DisplayRole){
    return QVariant();
  }

  return pvRowCopyProgress[row];
}

QVariant MappingModel::tableCopyStatusData(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyError.size());

  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return tableCopyStatusText(pvRowCopyStatus[row]);
    case Qt::ToolTipRole:
      return pvRowCopyError[row].text();
    case Qt::DecorationRole:
      return tableCopyStatusDecoration(pvRowCopyStatus[row]);
    default:
      return QVariant();
  }
}

QString MappingModel::tableCopyStatusText(MappingModel::CopyStatus status) const
{
  switch(status){
    case CopyStatusUnknown:
      return "";
    case CopyStatusOk:
      return tr("Ok");
    case CopyStatusError:
      return tr("Error");
  }
  return "";
}

QVariant MappingModel::tableCopyStatusDecoration(MappingModel::CopyStatus status) const
{
  switch(status){
    case CopyStatusUnknown:
      return QVariant();
    case CopyStatusOk:
      return QColor(Qt::green);
    case CopyStatusError:
      return QColor(Qt::red);
  }
  return QVariant();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
