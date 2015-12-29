/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtComboBoxItemDelegate.h"
#include <QStyleOptionComboBox>
#include <QApplication>
#include <QStyle>
#include <QComboBox>

#include <QDebug>

mdtComboBoxItemDelegate::mdtComboBoxItemDelegate(QObject* parent)
 : QStyledItemDelegate(parent),
   pvComboBox(new QComboBox)
{
//   connect(pvComboBox, &QComboBox::destroyed, this, &mdtComboBoxItemDelegate::resetPvComboBoxPointer);
}

mdtComboBoxItemDelegate::~mdtComboBoxItemDelegate()
{
  /*
   * If createEditor() was never called, pvComboBox has no parent, and we leek if we not delete it here
   */
  if( (pvComboBox != nullptr) && (pvComboBox->parent() == nullptr) ){
    delete pvComboBox;
  }
}

void mdtComboBoxItemDelegate::addItem(const QString& text, const QVariant& userData)
{
  Q_ASSERT(pvComboBox != nullptr);
  pvComboBox->addItem(text, userData);
}

void mdtComboBoxItemDelegate::addItem(const QIcon& icon, const QString& text, const QVariant& userData)
{
  Q_ASSERT(pvComboBox != nullptr);
  pvComboBox->addItem(icon, text, userData);
}

void mdtComboBoxItemDelegate::addItems(const QStringList& texts)
{
  Q_ASSERT(pvComboBox != nullptr);
  pvComboBox->addItems(texts);
}

void mdtComboBoxItemDelegate::setModel(QAbstractItemModel* model, int modelColumn)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(pvComboBox != nullptr);
  pvComboBox->setModel(model);
  pvComboBox->setModelColumn(modelColumn);
}

void mdtComboBoxItemDelegate::clear()
{
  pvComboBox->clear();
}

void mdtComboBoxItemDelegate::setCurrentIndex(int index)
{
  Q_ASSERT(pvComboBox != nullptr);
  pvComboBox->setCurrentIndex(index);
}

int mdtComboBoxItemDelegate::currentIndex() const
{
  Q_ASSERT(pvComboBox != nullptr);
  return pvComboBox->currentIndex();
}

// void mdtComboBoxItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
// {
//   QStyleOptionComboBox cbOption;
// 
//   cbOption.init( (QWidget*)parent() );
//   cbOption.rect = option.rect;
//   cbOption.state = option.state;
//   cbOption.currentText = "Test";
//   
// ///  QWidget *w = qobject_cast<QWidget*>(option.styleObject);
//   
//   QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &cbOption, painter, (QWidget*)parent());
//   ///QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &option, painter);
// }

QWidget* mdtComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
  Q_ASSERT(pvComboBox != nullptr);

  // Enshure that pvComboBox will be displayed on the right widget
  if(pvComboBox->parent() != parent){
    pvComboBox->setParent(parent);
  }
  return pvComboBox;
}

void mdtComboBoxItemDelegate::destroyEditor(QWidget*, const QModelIndex &) const
{
  // We do nothing here. pvComboBox is destroyed by its parent, or our destructor.
}

void mdtComboBoxItemDelegate::setEditorData(QWidget*, const QModelIndex & index) const
{
  Q_ASSERT(index.model() != nullptr);
  Q_ASSERT(pvComboBox != nullptr);

  QString text = index.model()->data(index, Qt::DisplayRole).toString();

  qDebug() << "setEditorData() for index " << index << ":";
  qDebug() << "-> CB current index: " << pvComboBox->currentIndex();
  qDebug() << "-> Setting text " << text << " ...";
  
  pvComboBox->setCurrentText(text);
  qDebug() << "-> CB current index: " << pvComboBox->currentIndex();
}

void mdtComboBoxItemDelegate::setModelData(QWidget*, QAbstractItemModel* model, const QModelIndex& index) const
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(pvComboBox != nullptr);

  qDebug() << "setModelData() for index " << index << ":";
  qDebug() << "-> CB text: " << pvComboBox->currentText() << " , data: " << pvComboBox->currentData();
  qDebug() << "-> Updating model ...";
  /*
   * Updating model will also recall setEditorData().
   * For this, we must get a copy of combobox data before updating
   */
  auto data = pvComboBox->currentData();
  auto text = pvComboBox->currentText();
  model->setData(index, data, Qt::EditRole);
  model->setData(index, text, Qt::DisplayRole);
  qDebug() << "-> DisplayRole data: " << model->data(index, Qt::DisplayRole);
  qDebug() << "-> EditRole data: " << model->data(index, Qt::EditRole);
}

void mdtComboBoxItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem & option, const QModelIndex &) const
{
  Q_ASSERT(editor != nullptr);

  editor->setGeometry(option.rect);
}

// void mdtComboBoxItemDelegate::resetPvComboBoxPointer(QObject *)
// {
//   qDebug() << "Reset CB..";
//   pvComboBox = nullptr;
// }
