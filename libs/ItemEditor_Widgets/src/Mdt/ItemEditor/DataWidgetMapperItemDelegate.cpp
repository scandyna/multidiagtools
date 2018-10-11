/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include <QMetaType>
#include <QMetaObject>
#include <QMetaProperty>
#include <QByteArray>
#include <QAbstractItemModel>
#include <QString>

// #include "Debug.h"

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
    model->setData(index, convertData( editor->property(propertyName.constData()) ), Qt::EditRole);
  }
}

void DataWidgetMapperItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  Q_ASSERT(editor != nullptr);

  const QByteArray propertyName = editor->metaObject()->userProperty().name();
  if(propertyName.isEmpty()){
    return;
  }
  auto value = index.data(Qt::EditRole);
  /*
   * If value is null, QVariant::convert() will fail,
   * and QObject::setProperty() will not update editor.
   * We have to provide a default value to display in this case.
   */
  if(value.isNull()){
    value = displayValueForNullVariant( editor->property(propertyName.constData()).userType() );
  }
  editor->setProperty(propertyName.constData(), value);
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
//   qDebug() << "DWMID: editor type: " << QMetaType::typeName(editorValueType);
  switch( static_cast<QMetaType::Type>(editorValueType) ){
    case QMetaType::QString:
      return QString();
  }
  return QVariant();
}

}} // namespace Mdt{ namespace ItemEditor{
