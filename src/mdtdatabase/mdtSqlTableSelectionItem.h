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
#ifndef MDT_SQL_TABLE_SELECTION_ITEM_H
#define MDT_SQL_TABLE_SELECTION_ITEM_H

#include <QModelIndex>
#include <QString>

/*! \brief SQL table selection item container
 */
class mdtSqlTableSelectionItem
{
 public:

  /*! \brief Constructor
   */
  mdtSqlTableSelectionItem(const QModelIndex & index, const QString & fieldName);

  /*! \brief Get index
   */
  inline  QModelIndex index() const { return pvIndex; }

  /*! \brief Get field name
   */
  inline QString fieldName() const { return pvFieldName; }

 private:

  QModelIndex pvIndex;
  QString pvFieldName;
};

#endif // #ifndef MDT_SQL_TABLE_SELECTION_ITEM_H
