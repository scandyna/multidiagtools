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
#include "WidgetEditablePropertyMap.h"
#include <QMetaObject>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <vector>
#include <algorithm>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

struct WidgetEditablePropertyMapItem
{
  WidgetEditablePropertyMapItem(const char * const className, const char * const propertyName, bool reversed)
   : className(className),
     property(propertyName, reversed)
  {
  }

  QByteArray className;
  WidgetEditableProperty property;
};

class WidgetEditablePropertyMapImpl
{
 public:

  using const_iterator = std::vector<WidgetEditablePropertyMapItem>::const_iterator;
  using iterator = std::vector<WidgetEditablePropertyMapItem>::iterator;

  explicit WidgetEditablePropertyMapImpl();
  ~WidgetEditablePropertyMapImpl();
  void setEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed);
  void addEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed);
  WidgetEditableProperty getEditableProperty(const char*const className) const;
  WidgetEditableProperty getEditableProperty(const QWidget * const widget) const;

 private:

  const_iterator iteratorForClassName(const char * const className) const;
  iterator iteratorForClassNameW(const char * const className);

  std::vector<WidgetEditablePropertyMapItem> mItems;
};

WidgetEditablePropertyMapImpl::WidgetEditablePropertyMapImpl()
{
}

WidgetEditablePropertyMapImpl::~WidgetEditablePropertyMapImpl()
{
}

void WidgetEditablePropertyMapImpl::setEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed)
{
  auto it = iteratorForClassNameW(className);
  if(it == mItems.end()){
    addEditableProperty(className, editablePropertyName, reversed);
  }else{
    *it = WidgetEditablePropertyMapItem(className, editablePropertyName, reversed);
  }
}

void WidgetEditablePropertyMapImpl::addEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed)
{
  mItems.emplace_back(className, editablePropertyName, reversed);
}

WidgetEditableProperty WidgetEditablePropertyMapImpl::getEditableProperty(const char*const className) const
{
  const auto it = iteratorForClassName(className);
  if(it == mItems.cend()){
    return WidgetEditableProperty();
  }
  return it->property;
}

WidgetEditableProperty WidgetEditablePropertyMapImpl::getEditableProperty(const QWidget*const widget) const
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  auto it = iteratorForClassName(widget->metaObject()->className());
  auto *superClass = widget->metaObject()->superClass();
  while( (it == mItems.cend()) && (superClass != nullptr) ){
    it = iteratorForClassName(superClass->className());
    superClass = superClass->superClass();
  }
  if(it == mItems.cend()){
    return WidgetEditableProperty();
  }
  return it->property;
}

WidgetEditablePropertyMapImpl::const_iterator WidgetEditablePropertyMapImpl::iteratorForClassName(const char*const className) const
{
  const auto cn = QByteArray(className);
  const auto pred = [cn](const WidgetEditablePropertyMapItem & item){
    return item.className == cn;
  };
  return std::find_if(mItems.cbegin(), mItems.cend(), pred);
}

WidgetEditablePropertyMapImpl::iterator WidgetEditablePropertyMapImpl::iteratorForClassNameW(const char*const className)
{
  const auto cn = QByteArray(className);
  const auto pred = [cn](const WidgetEditablePropertyMapItem & item){
    return item.className == cn;
  };
  return std::find_if(mItems.begin(), mItems.end(), pred);
}

/*
 * WidgetEditableProperty
 */

WidgetEditableProperty::WidgetEditableProperty(const char*const editablePropertyName, bool reversed)
 : mPropertyName(editablePropertyName),
   mIsReversed(reversed)
{
  Q_ASSERT(!mPropertyName.isEmpty());
}

/*
 * WidgetEditablePropertyMap
 */

std::unique_ptr<WidgetEditablePropertyMapImpl> WidgetEditablePropertyMap::mImpl;
int WidgetEditablePropertyMap::mInstanceCount = 0;

WidgetEditablePropertyMap::WidgetEditablePropertyMap()
{
  if(!mImpl){
    mImpl.reset(new WidgetEditablePropertyMapImpl);
    populateWithQtWidgets();
  }
  ++mInstanceCount;
}

WidgetEditablePropertyMap::~WidgetEditablePropertyMap()
{
  if(mInstanceCount == 1){
    mImpl.reset();
  }
  --mInstanceCount;
}

WidgetEditableProperty WidgetEditablePropertyMap::getEditableProperty(const QWidget*const widget) const
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  return mImpl->getEditableProperty(widget);
}

bool WidgetEditablePropertyMap::setWidgetEditable(QWidget*const widget, bool editable) const
{
  Q_ASSERT(widget != nullptr);

  const auto property = getEditableProperty(widget);
  if(property.isNull()){
    return false;
  }
  if(property.isReversed()){
    editable = !editable;
  }

  return widget->setProperty(property.editablePropertyName(), editable);
}

void WidgetEditablePropertyMap::setEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed)
{
  Q_ASSERT(className != nullptr);
  Q_ASSERT(editablePropertyName != nullptr);

  mImpl->setEditableProperty(className, editablePropertyName, reversed);
}

void WidgetEditablePropertyMap::addEditableProperty(const char*const className, const char*const editablePropertyName, bool reversed)
{
  Q_ASSERT(className != nullptr);
  Q_ASSERT(editablePropertyName != nullptr);

  mImpl->addEditableProperty(className, editablePropertyName, reversed);
}

WidgetEditableProperty WidgetEditablePropertyMap::getEditableProperty(const char*const className) const
{
  Q_ASSERT(className != nullptr);

  return mImpl->getEditableProperty(className);
}

const char* WidgetEditablePropertyMap::classNameForMetaTypeId(int id) const
{
  const QMetaType metaType(id);
  Q_ASSERT(metaType.metaObject() != nullptr);
  return metaType.metaObject()->className();
}

void WidgetEditablePropertyMap::populateWithQtWidgets()
{
  addEditableProperty<QLineEdit>("readOnly", true);
  addEditableProperty<QAbstractSpinBox>("readOnly", true);
  addEditableProperty<QTextEdit>("readOnly", true);
  addEditableProperty<QPlainTextEdit>("readOnly", true);
  addEditableProperty<QComboBox>("editable", false);
  addEditableProperty<QCheckBox>("checkable", false);
  addEditableProperty<QRadioButton>("checkable", false);
}

}} // namespace Mdt{ namespace ItemEditor{
