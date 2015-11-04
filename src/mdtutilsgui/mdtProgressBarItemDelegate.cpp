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
#include "mdtProgressBarItemDelegate.h"
#include <QStyleOptionProgressBar>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>
#include <QString>
#include <QApplication>

mdtProgressBarItemDelegate::mdtProgressBarItemDelegate(QObject* parent)
 : QStyledItemDelegate(parent)
//    pvMinimum(0),
//    pvMaximum(0)
{
}

mdtProgressBarItemDelegate::~mdtProgressBarItemDelegate()
{
}

void mdtProgressBarItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  int progress = index.data().toInt();
//   int min = pvMinimum;
//   int max = pvMaximum;

  QStyleOptionProgressBar progressBarOption;
  progressBarOption.rect = option.rect;
  progressBarOption.minimum = 0;
  progressBarOption.maximum = 100;
  progressBarOption.progress = progress;
  progressBarOption.text = QString::number(progress) + "%";
  progressBarOption.textVisible = true;
//   if( (min == 0) && (max == 0) ){
//     /// \todo The progress should be animated. This not works currently because we not repaint.
//     progressBarOption.progress = 0;
//     progressBarOption.textVisible = false;
//   }else{
//     progressBarOption.progress = progress;
//     progressBarOption.text = QString::number(progress) + "%";
//     progressBarOption.textVisible = true;
//   }

  QApplication::style()->drawControl(QStyle::QStyle::CE_ProgressBar, &progressBarOption, painter);
}

// void mdtProgressBarItemDelegate::setRange(int minimum, int maximum)
// {
//   pvMinimum = minimum;
//   pvMaximum = maximum;
// }
