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
#ifndef MDT_DATA_TABLE_ITEM_DELEGATE_H
#define MDT_DATA_TABLE_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QWidget>
#include <QSize>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

/*! \brief Delegate to display table items in custom way
 */
class mdtDataTableItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtDataTableItemDelegate(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtDataTableItemDelegate();

  /*! \brief
   */
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  /*! \brief
   */
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif  // #ifndef MDT_DATA_TABLE_ITEM_DELEGATE_H
