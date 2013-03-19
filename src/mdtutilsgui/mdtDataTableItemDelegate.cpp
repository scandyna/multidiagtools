/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtDataTableItemDelegate.h"
#include <QColor>
#include <QPen>

#include <QDebug>

mdtDataTableItemDelegate::mdtDataTableItemDelegate(QWidget *parent)
 : QStyledItemDelegate(parent)
{
  qDebug() << "mdtDataTableItemDelegate::mdtDataTableItemDelegate() ...";
}

mdtDataTableItemDelegate::~mdtDataTableItemDelegate()
{
}

void mdtDataTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  ///qDebug() << "index: " << index;
  if(index.isValid() && (index.column() == 4)){
    ///Q_ASSERT(index.model() != 0);
    painter->save();
    ///if(index.model()->data(index.model()->index(index.row(), 1)) == "Test 1"){
    if(index.data() == "Ok"){
      painter->setPen(QColor(Qt::white));
      painter->fillRect(option.rect, QColor(Qt::green));
      painter->drawText(option.rect, Qt::AlignHCenter | Qt::AlignVCenter , index.data().toString());
    }else{
      painter->setPen(QColor(Qt::white));
      painter->fillRect(option.rect, QColor(Qt::red));
      painter->drawText(option.rect, Qt::AlignHCenter | Qt::AlignVCenter , index.data().toString());
    }
    painter->restore();
  }else{
    QStyledItemDelegate::paint(painter, option, index);
  }
}

QSize mdtDataTableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
}
