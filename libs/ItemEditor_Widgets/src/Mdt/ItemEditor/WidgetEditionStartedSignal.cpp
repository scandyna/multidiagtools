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
#include "WidgetEditionStartedSignal.h"
#include <QMetaProperty>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>

namespace Mdt{ namespace ItemEditor{

WidgetEditionStartedSignal::WidgetEditionStartedSignal(QObject* parent)
 : QObject(parent)
{
}

void WidgetEditionStartedSignal::addWidget(const QWidget*const widget)
{
  Q_ASSERT(widget != nullptr);

  connectWidgetEditionStartedSignal(widget);
}

void WidgetEditionStartedSignal::removeWidget(const QWidget*const widget)
{
  Q_ASSERT(widget != nullptr);

  disconnectWidgetEditionStartedSignal(widget);
}

QMetaMethod WidgetEditionStartedSignal::getEditionStartedSignal(const QWidget * const widget) const
{
  Q_ASSERT(widget != nullptr);

  const QWidget *edit = qobject_cast<const QLineEdit*>(widget);
  if(edit != nullptr){
    return QMetaMethod::fromSignal(&QLineEdit::textEdited);
  }
  edit = qobject_cast<const QComboBox*>(widget);
  if(edit != nullptr){
    auto *cb = qobject_cast<const QComboBox*>(widget);
    qDebug() << "CB edit: " << cb->lineEdit();
    return QMetaMethod::fromSignal( QOverload<int>::of(&QComboBox::activated) );
  }

  return getEditionStartedSignalFromUserProperty(widget);
}

void WidgetEditionStartedSignal::connectWidgetEditionStartedSignal(const QWidget*const widget)
{
  Q_ASSERT(widget != nullptr);

  auto *lineEdit = qobject_cast<const QLineEdit*>(widget);
  if(lineEdit != nullptr){
    connectQLineEditEditionStartedSignal(lineEdit);
    return;
  }
  auto *comboBox = qobject_cast<const QComboBox*>(widget);
  if(comboBox != nullptr){
    connectQComboboxEditionStartedSignal(comboBox);
    return;
  }

  QMetaMethod signalMethod = getEditionStartedSignalFromUserProperty(widget);
  if(!signalMethod.isValid()){
    const QString message = tr("Could not find a signal to tell edition started for widget '%1'.")
    .arg( QString::fromLatin1(widget->metaObject()->className()) );
    qWarning() << "WidgetEditionStartedSignal: " << message;
    return;
  }
  QMetaMethod slotMethod = getOnDataEditionStartedMethod();
  Q_ASSERT(slotMethod.isValid());

  connect(widget, signalMethod, this, slotMethod);
}

void WidgetEditionStartedSignal::disconnectWidgetEditionStartedSignal(const QWidget*const widget)
{
  Q_ASSERT(widget != nullptr);

  auto *lineEdit = qobject_cast<const QLineEdit*>(widget);
  if(lineEdit != nullptr){
    disconnectQLineEditEditionStartedSignal(lineEdit);
    return;
  }
  auto *comboBox = qobject_cast<const QComboBox*>(widget);
  if(comboBox != nullptr){
    disconnectQComboboxEditionStartedSignal(comboBox);
    return;
  }

  QMetaMethod signalMethod = getEditionStartedSignalFromUserProperty(widget);
  if(!signalMethod.isValid()){
    return;
  }
  QMetaMethod slotMethod = getOnDataEditionStartedMethod();
  Q_ASSERT(slotMethod.isValid());

  disconnect(widget, signalMethod, this, slotMethod);
}

void WidgetEditionStartedSignal::onWidgetEditionStarted()
{
  emit editionStarted();
}

void WidgetEditionStartedSignal::connectQLineEditEditionStartedSignal(const QLineEdit*const lineEdit)
{
  Q_ASSERT(lineEdit != nullptr);

  connect(lineEdit, &QLineEdit::textEdited, this, &WidgetEditionStartedSignal::onWidgetEditionStarted);
}

void WidgetEditionStartedSignal::disconnectQLineEditEditionStartedSignal(const QLineEdit*const lineEdit)
{
  Q_ASSERT(lineEdit != nullptr);

  disconnect(lineEdit, &QLineEdit::textEdited, this, &WidgetEditionStartedSignal::onWidgetEditionStarted);
}

void WidgetEditionStartedSignal::connectQComboboxEditionStartedSignal(const QComboBox*const comboBox)
{
  Q_ASSERT(comboBox != nullptr);

  connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, &WidgetEditionStartedSignal::onWidgetEditionStarted);

  qDebug() << "Connect CB..";

  auto *lineEdit = comboBox->lineEdit();
  if(lineEdit != nullptr){
    qDebug() << "Connect LE..";
    connectQLineEditEditionStartedSignal(lineEdit);
  }
}

void WidgetEditionStartedSignal::disconnectQComboboxEditionStartedSignal(const QComboBox*const comboBox)
{
  Q_ASSERT(comboBox != nullptr);

  auto *lineEdit = comboBox->lineEdit();
  if(lineEdit != nullptr){
    disconnectQLineEditEditionStartedSignal(lineEdit);
  }

  disconnect(comboBox, QOverload<int>::of(&QComboBox::activated), this, &WidgetEditionStartedSignal::onWidgetEditionStarted);
}


QMetaMethod WidgetEditionStartedSignal::getEditionStartedSignalFromUserProperty(const QWidget*const widget) const
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  QMetaProperty userProperty = widget->metaObject()->userProperty();
  if(!userProperty.hasNotifySignal()){
    return QMetaMethod();
  }

  return userProperty.notifySignal();
}

QMetaMethod WidgetEditionStartedSignal::getOnDataEditionStartedMethod() const
{
  Q_ASSERT(metaObject() != nullptr);

  const int slotIndex = metaObject()->indexOfSlot("onWidgetEditionStarted()");
  Q_ASSERT(slotIndex >= 0);

  return metaObject()->method(slotIndex);
}

}} // namespace Mdt{ namespace ItemEditor{
