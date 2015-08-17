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
#ifndef MDT_SQL_TABLE_MAPPING_MODEL_H
#define MDT_SQL_TABLE_MAPPING_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVariant>
#include <QModelIndex>

/*! \brief Table model used by mdtSqlTableMappingWidget
 */
class mdtSqlTableMappingModel : public QAbstractTableModel
{
 public:

  /*! \brief Constructor
   */
  mdtSqlTableMappingModel(QObject *parent = nullptr);

  /*! \brief Get header data (for the view)
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  /*! \brief Get columns count (for the view)
   */
  int columnCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get rows count (for the view)
   */
  int rowCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get data (for the view)
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

  

 private:

  Q_DISABLE_COPY(mdtSqlTableMappingModel)

  
};

#endif // #ifndef MDT_SQL_TABLE_MAPPING_MODEL_H
