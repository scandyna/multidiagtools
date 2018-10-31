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
#include "MappedWidgetGeneric.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaType>
#include <QDebug>

namespace Mdt{ namespace ItemEditor{

MappedWidgetGeneric::MappedWidgetGeneric(QWidget* widget, int column)
 : MappedWidget(widget, column)
{
  Q_ASSERT(widget != nullptr);

  mEnabled = widget->isEnabled();
  if(hasWidgetUserProperty()){
    connectWidgetToEditionStartedSignal();
  }else{
    qWarning() << "Mapping a widget '" << widget << "' that is not directly handled and not having a user property";
  }
  setupWidgetEditableProperty();
}

void MappedWidgetGeneric::setWidgetValue(const QVariant& value)
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);

  if(hasWidgetUserProperty()){
    widget()->metaObject()->userProperty().write(widget(), value);
  }
}

QVariant MappedWidgetGeneric::widgetValue() const
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);

  if(hasWidgetUserProperty()){
    return widget()->metaObject()->userProperty().read(widget());
  }

  return QVariant();
}

void MappedWidgetGeneric::setWidgetEditable(bool editable)
{
  Q_ASSERT(widget() != nullptr);

  mEditable = editable;
  if(mWidgetEditableProperty.isNull()){
    widget()->setEnabled( canEnableWidget() );
    return;
  }
  if(mWidgetEditableProperty.isReversed()){
    editable = !editable;
  }

  widget()->setProperty(mWidgetEditableProperty.editablePropertyName().constData(), editable);
}

void MappedWidgetGeneric::setWidgetEnabled(bool enable)
{
  mEnabled = enable;
  widget()->setEnabled( canEnableWidget() );
}

bool MappedWidgetGeneric::hasWidgetUserProperty() const
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);

  return widget()->metaObject()->userProperty().isValid();
}

void MappedWidgetGeneric::setupWidgetEditableProperty()
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);

  bool isReversed = false;
  QMetaProperty property = getWidgetProperty("editable");
  if(!property.isValid()){
    property = getWidgetProperty("readOnly");
    if(property.isValid()){
      isReversed = true;
    }
  }
  if(!property.isValid()){
    property = getWidgetProperty("checkable");
  }
  if(!property.isValid()){
    qWarning() << "Mapping a widget '" << widget() << "' that has no editable or readOnly or checkable property";
    return;
  }

  if(static_cast<QMetaType::Type>(property.type()) != QMetaType::Bool){
    qWarning() << "Mapping a widget '" << widget() << "' that has " << property.name() << " property, but it is not a boolean type";
    return;
  }
  if(!property.isWritable()){
    qWarning() << "Mapping a widget '" << widget() << "' that has " << property.name() << " property, but it is not writable";
    return;
  }

  mWidgetEditableProperty.setEditableProperty(property.name(), isReversed);
}

void MappedWidgetGeneric::connectWidgetToEditionStartedSignal()
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);
  Q_ASSERT(metaObject() != nullptr);

  const QMetaProperty userProperty = widget()->metaObject()->userProperty();
  Q_ASSERT(userProperty.isValid());

  const QMetaMethod notifySignal = userProperty.notifySignal();
  if(!notifySignal.isValid()){
    qWarning() << "Mapping widget " << widget() << " which has a user property, but no notify signal. editionStarted() will not be emitted";
    return;
  }

  const int editionStartedSignalIndex = metaObject()->indexOfSignal("editionStarted()");
  Q_ASSERT(editionStartedSignalIndex >= 0);
  const QMetaMethod editionStartedSignal = metaObject()->method(editionStartedSignalIndex);
  Q_ASSERT(editionStartedSignal.isValid());

  connect(widget(), notifySignal, this, editionStartedSignal);
}

QMetaProperty MappedWidgetGeneric::getWidgetProperty(const char*const name)
{
  Q_ASSERT(widget() != nullptr);
  Q_ASSERT(widget()->metaObject() != nullptr);

  const int index = widget()->metaObject()->indexOfProperty(name);
  if(index < 0){
    return QMetaProperty();
  }

  return widget()->metaObject()->property(index);
}

bool MappedWidgetGeneric::canEnableWidget() const
{
  return (mEditable && mEnabled);
}

}} // namespace Mdt{ namespace ItemEditor{
