/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SORT_FILTER_PROXY_MODEL_H
#define MDT_SORT_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QList>
#include <QVector>
#include <QPair>
#include <QString>

/*! \brief Provide natural sort for Qt model/view
 */
class mdtSortFilterProxyModel : public QSortFilterProxyModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSortFilterProxyModel(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSortFilterProxyModel();

  /*! \brief Clear columns sort order
   *
   * \sa addColumnToSortOrder()
   */
  void clearColumnsSortOrder();

  /*! \brief Add a column to columns sort order
   *
   * Columns sort order is similar meaning
   *  than SQL ORDER BY clause.
   *
   * For example, to sort columns in order 0, 2, 1
   *  all ascending, call:
   *  - clearColumnsSortOrder();
   *  - addColumnToSortOrder(0, Qt::AscendingOrder);
   *  - addColumnToSortOrder(2, Qt::AscendingOrder);
   *  - addColumnToSortOrder(1, Qt::AscendingOrder);
   *
   * Note: if given column is out of bound, it will simply be ignored.
   */
  void addColumnToSortOrder(int column, Qt::SortOrder order = Qt::AscendingOrder);

  /*! \brief Add a field to columns sort order
   *
   * Here it's possible to define a field name.
   *  Note that this method does nothing if 
   *  sourceModel is not based on QSqlQueryModel.
   */
  void addColumnToSortOrder(const QString &fieldName, Qt::SortOrder order = Qt::AscendingOrder);

  /*! \brief Check if some coulumn was set to sort order
   */
  bool hasColumnToSort() const;

  /*! \brief Get list of indexes of columns that are in sort order
   *
   * Note: each call of this method will rebuild the list.
   */
  QVector<int> sortedColumns() const;

  /*! \brief Get sort order for given column
   *
   * If column is not in sorted columns list,
   *  Qt::AscendingOrder is returnded
   */
  Qt::SortOrder sortOrder(int column) const;

 private:

  /*! \brief Overloads QSortFilterProxyModel to provide natural sort
   *
   * If both data type of left and right are string types,
   *  the natural compare is used, else QSortFilterProxyModel::lessThan() is called.
   */
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

  /*! \brief Overloads QSortFilterProxyModel to provide multi-columns sort
   *
   * If columns where added with addColumnToSortOrder(),
   *  sorting is given for each of defined columns.
   *  Else, only given column is sorted.
   */
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

  QList<QPair<int, Qt::SortOrder> >pvColumnsSortOrder;

  Q_DISABLE_COPY(mdtSortFilterProxyModel);
};

#endif  // #ifndef MDT_SORT_FILTER_PROXY_MODEL_H
