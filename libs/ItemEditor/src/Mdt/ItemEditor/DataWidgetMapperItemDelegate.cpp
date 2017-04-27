/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "DataWidgetMapperItemDelegate.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QByteArray>
#include <QAbstractItemModel>
#include <QString>

#include <QLineEdit>

#include "Debug.h"

namespace Mdt{ namespace ItemEditor{

DataWidgetMapperItemDelegate::DataWidgetMapperItemDelegate(QObject* parent)
 : QStyledItemDelegate(parent)
{
}

void DataWidgetMapperItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  Q_ASSERT(editor != nullptr);
  Q_ASSERT(model != nullptr);

  const QByteArray propertyName = editor->metaObject()->userProperty().name();
  if(propertyName.isEmpty()){
    QStyledItemDelegate::setModelData(editor, model, index);
  }else{
    model->setData(index, convertData( editor->property(propertyName) ), Qt::EditRole);
  }
}

void DataWidgetMapperItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  Q_ASSERT(editor != nullptr);

  auto *le = qobject_cast<QLineEdit*>(editor);
  
  if(le != nullptr){
    //qDebug() << "DWMID: line edit text: " << le->text() << " , read only: " << le->isReadOnly() << " , enabled: " << le->isEnabled();
    qDebug() << "DWMID: line edit text: " << le->text();
  }
//   qDebug() << "DWMID: model data: " << index.data(Qt::EditRole) << " , null: " << index.data(Qt::EditRole).isNull();


//   if(le != nullptr){
//     le->clear();
//   }
  const QByteArray propertyName = editor->metaObject()->userProperty().name();
  if(propertyName.isEmpty()){
    return;
  }
  
//   qDebug() << "DWMID: editor data type: " << editor->property(propertyName).typeName();
  auto value = index.data(Qt::EditRole);
  qDebug() << "DWMID: model data: " << value << " , null: " << value.isNull();
  /*
   * If value is null, QVariant::convert() will fail,
   * and QObject::setProperty() will not update editor.
   * We have to provide a default value to display in this case.
   */
  if(value.isNull()){
    value = displayValueForNullVariant( editor->property(propertyName).userType() );
  }
  qDebug() << "DWMID: value: " << value;
  qDebug() << "DWMID: setEditorData ...";
  editor->setProperty(propertyName, value);
  /*
   * Problem seen with a QLineEdit:
   * If the edit has a text (for example 1)
   * and we update it using the setProperty() with a null QVariant,
   * the line edit was not updated.
   */
//   const auto v = index.data(Qt::EditRole);
//   
//   qDebug() << "DWMID: set: " << editor->setProperty(propertyName, v);
  
//   if(v.isNull()){
//     editor->setProperty(propertyName, QVariant());
//   }else{
//     editor->setProperty(propertyName, v);
//   }
  
//   QStyledItemDelegate::setEditorData(editor, index);
  
  if(le != nullptr){
    //qDebug() << "DWMID: line edit text: " << le->text() << " , read only: " << le->isReadOnly() << " , enabled: " << le->isEnabled();
    qDebug() << "DWMID: line edit text: " << le->text();
  }
}

QVariant DataWidgetMapperItemDelegate::convertData(const QVariant & data)
{
  switch(static_cast<QMetaType::Type>(data.type())){
    case QMetaType::QString:
      return convertFromString(data);
    default:
      break;
  }
  return data;
}

QVariant DataWidgetMapperItemDelegate::convertFromString(const QVariant & data)
{
  if(data.toString().trimmed().isEmpty()){
    return QVariant();
  }
  return data;
}

QVariant DataWidgetMapperItemDelegate::displayValueForNullVariant(int editorValueType)
{
  qDebug() << "DWMID: editor type: " << QMetaType::typeName(editorValueType);
  switch( static_cast<QMetaType::Type>(editorValueType) ){
    case QMetaType::QString:
      return QString();
  }
  return QVariant();
}

}} // namespace Mdt{ namespace ItemEditor{