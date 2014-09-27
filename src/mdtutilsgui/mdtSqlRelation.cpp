/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlRelation.h"
#include "mdtSqlRelationItem.h"
#include <QSqlField>

#include <QSqlError>

#include <QDebug>

mdtSqlRelation::mdtSqlRelation(QObject *parent)
 : QObject(parent)
{
  pvParentModel = 0;
  pvChildModel = 0;
  pvCurrentRow = -1;
  pvRelationType = mdtSqlRelationInfo::OneToMany;
  ///pvEditingOneToOneRelationRecord = false;
}

mdtSqlRelation::~mdtSqlRelation()
{
  clear();
}

void mdtSqlRelation::setParentModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  pvParentModel = model;
}

void mdtSqlRelation::setChildModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  pvChildModel = model;
  connect(pvChildModel, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(onChildBeforeInsert(QSqlRecord&)));
}

bool mdtSqlRelation::addRelation(const mdtSqlRelationInfoItem & relationInfoItem)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  QSqlRecord record;
  int parentFieldIndex;
  QSqlField parentField;
  int childFieldIndex;
  QSqlField childField;

  // Get parent  field index, check that it exist
  record = pvParentModel->record();
  parentFieldIndex = record.indexOf(relationInfoItem.parentFieldName);
  if(parentFieldIndex < 0){
    pvLastError.setError(tr("Field") + " '" + relationInfoItem.parentFieldName + "' " + tr("not found in parent table") + " '" + pvParentModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRelation");
    pvLastError.commit();
    return false;
  }
  parentField = record.field(parentFieldIndex);
  // Get child field index, check that it exist
  record = pvChildModel->record();
  childFieldIndex = record.indexOf(relationInfoItem.childFieldName);
  if(childFieldIndex < 0){
    pvLastError.setError(tr("Field") + " '" + relationInfoItem.childFieldName + "' " + tr("not found in child table") + " '" + pvChildModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRelation");
    pvLastError.commit();
    return false;
  }
  childField = record.field(childFieldIndex);
  // Add relation informations
  mdtSqlRelationItem *item = new mdtSqlRelationItem;
  item->setParentField(parentField);
  item->setParentFieldIndex(parentFieldIndex);
  item->setChildField(childField);
  item->setChildFieldIndex(childFieldIndex);
  item->setCopyParentToChildOnInsertion(relationInfoItem.copyParentToChildOnInsertion);
  item->setRelationOperatorWithPreviousItem(" " + relationInfoItem.relationOperatorWithPreviousItem.trimmed() + " ");
  pvRelations.append(item);
  ///pvEditingOneToOneRelationRecord = false;

  return true;
}

bool mdtSqlRelation::addRelation(const QString &parentFieldName, const QString &childFieldName, bool copyParentToChildOnInsertion, const QString &operatorWithPreviousItem)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  mdtSqlRelationInfoItem item;

  item.parentFieldName = parentFieldName;
  item.childFieldName = childFieldName;
  item.copyParentToChildOnInsertion = copyParentToChildOnInsertion;
  item.relationOperatorWithPreviousItem = operatorWithPreviousItem;

  return addRelation(item);

  /**
  QSqlRecord record;
  int parentFieldIndex;
  QSqlField parentField;
  int childFieldIndex;
  QSqlField childField;

  // Get parent  field index, check that it exist
  record = pvParentModel->record();
  parentFieldIndex = record.indexOf(parentFieldName);
  if(parentFieldIndex < 0){
    pvLastError.setError(tr("Field") + " '" + parentFieldName + "' " + tr("not found in parent table") + " '" + pvParentModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRelation");
    pvLastError.commit();
    return false;
  }
  parentField = record.field(parentFieldIndex);
  // Get child field index, check that it exist
  record = pvChildModel->record();
  childFieldIndex = record.indexOf(childFieldName);
  if(childFieldIndex < 0){
    pvLastError.setError(tr("Field") + " '" + childFieldName + "' " + tr("not found in child table") + " '" + pvChildModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRelation");
    pvLastError.commit();
    return false;
  }
  childField = record.field(childFieldIndex);
  // Add relation informations
  mdtSqlRelationItem *item = new mdtSqlRelationItem;
  item->setParentField(parentField);
  item->setParentFieldIndex(parentFieldIndex);
  item->setChildField(childField);
  item->setChildFieldIndex(childFieldIndex);
  item->setCopyParentToChildOnInsertion(copyParentToChildOnInsertion);
  item->setRelationOperatorWithPreviousItem(" " + operatorWithPreviousItem.trimmed() + " ");
  pvRelations.append(item);

  return true;
  */
}

bool mdtSqlRelation::addRelations(const mdtSqlRelationInfo & relationInfo)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  int i;

  pvRelationType = relationInfo.relationType();
  /**
  if(pvRelationType == mdtSqlRelationInfo::OneToOne){
    qDebug() << "Relation type 1-1 - connect parent beforeInsert() ...";
    connect(pvParentModel, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(insertRowInChildModel()));
  }
  */
  ///connect(pvChildModel, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(onChildBeforeInsert(QSqlRecord&)));
  for(i = 0; i < relationInfo.items().size(); ++i){
    if(!addRelation(relationInfo.items().at(i))){
      return false;
    }
  }

  return true;
}

void mdtSqlRelation::clear()
{
  qDeleteAll(pvRelations);
  pvRelations.clear();
  pvParentModel = 0;
  pvChildModel = 0;
  ///pvEditingOneToOneRelationRecord = false;
}

void mdtSqlRelation::setParentCurrentRow(int row)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  pvCurrentRow = row;
  generateChildModelRelationFilter(pvCurrentRow);
}

void mdtSqlRelation::setParentCurrentIndex(int index)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  pvCurrentRow = index;
  /**
  if(pvEditingOneToOneRelationRecord){
    return;
  }
  */
  generateChildModelRelationFilter(index);
}

void mdtSqlRelation::setParentCurrentIndex(const QModelIndex &index)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  if(index.isValid()){
    setParentCurrentIndex(index.row());
  }else{
    setParentCurrentIndex(-1);
  }
}

void mdtSqlRelation::setParentCurrentIndex(const QModelIndex &current, const QModelIndex &previous)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  setParentCurrentIndex(current);
}

// void mdtSqlRelation::onParentInsertDone()
// {
//   Q_ASSERT(pvParentModel != 0);
//   Q_ASSERT(pvChildModel != 0);
// 
//   /**
//   QSqlRecord parentRecord;
//   QSqlRecord childRecord;
// 
//   
//   // Get data record of the parent model
//   parentRecord = pvParentModel->record(pvCurrentRow);
//   // Get data record of the child model
//   Q_ASSERT(pvChildModel->rowCount() == 1);
//   childRecord = pvChildModel->record(0);
//   // Update childs
//   setChildForeingKeyValues(parentRecord, childRecord);
//   
//   qDebug() << "onParentInsertDone():\n" << " -> Parent record: " << parentRecord << "\n -> Child record: " << childRecord;
//   
//   */
//   /// \todo Provisoire (faire méthode séparée)
//   if(!pvChildModel->submitAll()){
//     qDebug() << "ERR: " << pvChildModel->lastError();
//   }
//   generateChildModelRelationFilter(pvCurrentRow);
//   // Reset edit flag
//   ///pvEditingOneToOneRelationRecord = false;
// }

/**
void mdtSqlRelation::insertRowInChildModel()
{
  Q_ASSERT(pvChildModel != 0);

  qDebug() << "Insert a row in model " << pvChildModel->tableName();
  pvChildModel->insertRows(0, 1);
  ///pvEditingOneToOneRelationRecord = true;
}
*/

void mdtSqlRelation::onChildBeforeInsert(QSqlRecord &childRecord)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  QSqlRecord parentRecord;

  // On 1-1 relation, we cannot update now
  /**
  if(pvRelationType == mdtSqlRelationInfo::OneToOne){
    return;
  }
  */
  // On invalid index, we do nothing
  if(pvCurrentRow < 0){
    return;
  }
  // Get data record of the parent model
  parentRecord = pvParentModel->record(pvCurrentRow);
  // Update childs
  setChildForeingKeyValues(parentRecord, childRecord);
  
  qDebug() << "onChildBeforeInsert():\n" << " -> Parent record: " << parentRecord << "\n -> Child record: " << childRecord;
}

void mdtSqlRelation::setChildForeingKeyValues(QSqlRecord &parentRecord, QSqlRecord &childRecord)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  int i;
  mdtSqlRelationItem *item;
  QVariant data;

  // Copy parent PK -> child FK if needed
  for(i=0; i<pvRelations.size(); ++i){
    item = pvRelations.at(i);
    Q_ASSERT(item != 0);
    if(item->copyParentToChildOnInsertion()){
      // Get parent model's data
      data = parentRecord.value(item->parentFieldIndex());
      // Put to child's field
      childRecord.setValue(item->childFieldIndex(), data);
      childRecord.setGenerated(item->childFieldIndex(), true);
    }
  }
}

void mdtSqlRelation::generateChildModelRelationFilter(int row)
{
  Q_ASSERT(pvParentModel != 0);
  Q_ASSERT(pvChildModel != 0);

  int i;
  mdtSqlRelationItem *item;
  QSqlRecord record;
  QVariant data;

  // Get data record of the parent model
  record = pvParentModel->record(row);
  // Build filter
  pvChildModelRelationFilter.clear();
  for(i=0; i<pvRelations.size(); ++i){
    item = pvRelations.at(i);
    Q_ASSERT(item != 0);
    // Get parent model's data
    if(!record.isNull(item->parentFieldIndex())){
      data = record.value(item->parentFieldIndex());
    }
    if(i>0){
      pvChildModelRelationFilter += item->relationOperatorWithPreviousItem();
    }
    pvChildModelRelationFilter += item->nameProtection() + pvChildModel->tableName() + item->nameProtection() + "." + item->nameProtection() + item->childFieldName() + item->nameProtection() + "=";
    if(data.isValid()){
      pvChildModelRelationFilter += item->dataProtection() + data.toString() + item->dataProtection();
    }else{
      pvChildModelRelationFilter += item->dataProtection() + item->valueForNoDataFilter().toString() + item->dataProtection();
    }
  }
  // Apply filter
  generateChildModelFilter();
  pvChildModel->setFilter(pvChildModelFilter);
  emit childModelFilterApplied();
  if(pvChildModel->rowCount() < 1){
    emit childModelIsEmpty();
  }
}

void mdtSqlRelation::generateChildModelFilter()
{
  pvChildModelFilter = pvChildModelRelationFilter;
}
