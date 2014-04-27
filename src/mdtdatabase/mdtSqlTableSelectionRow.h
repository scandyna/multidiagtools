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
#ifndef MDT_SQL_TABLE_SELECTION_ROW_H
#define MDT_SQL_TABLE_SELECTION_ROW_H

#include "mdtSqlTableSelectionItem.h"
#include <QList>


/*! \brief SQL table selection row container
 *
 * Each column of the selection is represented by a mdtSqlTableSelectionItem.
 */
class mdtSqlTableSelectionRow
{
  public:

  /*! \brief Constructor
   */
  mdtSqlTableSelectionRow();

  /*! \brief Clear
   */
  void clear();

  /*! \brief Get selection column count
   */
  inline int columnCount() const { return pvItemList.size(); }

  /*! \brief Add a model index
   */
  void addIndex(const QModelIndex & index, const QString & fieldName);

  /*! \brief Get model index for given selection column
   *
   * \param internalColumnIndex Column index of selection row,
   *          and probably not matches model index column.
   *
   * \pre internalColumnIndex must be in a valid range ( 0 <= internalColumnIndex < columnCount() ).
   */
  QModelIndex index(int internalColumnIndex) const;

  /*! \brief Get model index for given field name
   *
   * If field name not exists, a invalid model index is returned.
   */
  QModelIndex index(const QString & fieldName) const;

 private:

  QList<mdtSqlTableSelectionItem> pvItemList;
};

#endif // #ifndef MDTSQLTABLESELECTIONROW_H
