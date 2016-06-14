/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_INDEX_LIST_MODEL_H
#define MDT_SQL_SCHEMA_INDEX_LIST_MODEL_H

#include "IndexList.h"
#include <QAbstractTableModel>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Qt item/view table model that presents a list of Index
   *
   * Each row in the model represents a Index and each column a attribute of this index.
   */
  class IndexListModel : public QAbstractTableModel
  {
    Q_OBJECT

   public:

    /*! \brief Column index
     */
    enum ColumnIndex
    {
      IndexNameColumn = 0,  /*!< Column index of index name */
      TableNameColumn,      /*!< Column index of table name */
      FieldNameListColumn,  /*!< Column index of field name list */
      IsUniqueColumn        /*!< Column index of of unique flag */
    };

    /*! \brief Constructor
     */
    IndexListModel(QObject* parent = nullptr);

    // Copy disabled
    IndexListModel(const IndexListModel &) = delete;
    IndexListModel & operator=(const IndexListModel &) = delete;
    // Move disabled
    IndexListModel(IndexListModel &&) = delete;
    IndexListModel & operator=(IndexListModel &&) = delete;

    /*! \brief Set index list
     */
    void setIndexList(const IndexList & list);

    /*! \brief Get row count
     *
     * Returns the number of Index
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get header data
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

   private:

    IndexList pvIndexList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_INDEX_LIST_MODEL_H
