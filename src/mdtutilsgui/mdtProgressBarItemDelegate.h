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
#ifndef MDT_PROGRESS_BAR_ITEM_DELEGATE_H
#define MDT_PROGRESS_BAR_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

/*! \brief Item delegate that displays item as a progress bar
 *
 * This delegate displays a progress bar in percent,
 *  that implies a range from 0 to 100.
 *  To match this range, some scaling has to be done
 *  in user code.
 *  For scaling, mdtProgressValue can be used.
 *
 * Typical usage
 * \code
 * QTableView *tableView = new QTableView;
 * mdtProgressBarItemDelegate *dalagate = new mdtProgressBarItemDelegate(tableView);  // Give a parent to handle deletion of delegate
 * tableView->setModel(someUsefullModel);
 * tableView->setItemDelegateForColumn(2, delegate);  // Use progress bar delegate for column 2 only
 * \endcode
 */
class mdtProgressBarItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtProgressBarItemDelegate(QObject* parent = nullptr);

  /*! \brief Destructor
   */
  ~mdtProgressBarItemDelegate();

  /*! \internal Copy disabled
   */
  mdtProgressBarItemDelegate(const mdtProgressBarItemDelegate &) = delete;

  /*! \brief Paint reimplementation
   */
  void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

#endif // #ifndef MDT_PROGRESS_BAR_ITEM_DELEGATE_H
