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
#ifndef MDT_SQL_DATABASE_SCHEMA_MODEL_H
#define MDT_SQL_DATABASE_SCHEMA_MODEL_H

#include "mdtSqlDatabaseSchema.h"
#include <QAbstractItemModel>
#include <QModelIndex>

class mdtSqlDatabaseSchemaModelItem;

/*! \brief Tree model to display database some schema informations
 */
class mdtSqlDatabaseSchemaModel : public QAbstractItemModel
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseSchemaModel(QObject *parent = nullptr);

  /*! \Internal Copy disabled
   */
  mdtSqlDatabaseSchemaModel(const mdtSqlDatabaseSchemaModel & other) = delete;

  /*! \brief Destructor
   */
  ~mdtSqlDatabaseSchemaModel();

  /*! \brief Set database schema
   */
  void setSchema(const mdtSqlDatabaseSchema & s);

  /*! \brief Get index
   */
  QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get parent index
   */
  QModelIndex parent(const QModelIndex & index) const;

  /*! \brief Get row count
   */
  int rowCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get column count
   */
  int columnCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get data
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

 private:

  /*! \brief Column index
   */
  enum ColumnIndex
  {
    ObjectColumnIndex = 0   /*!< Column that display categories (Tables, Views, ..) and object names */
  };

  /*! \brief Get category data
   */
  QVariant categoryData(mdtSqlDatabaseSchemaModelItem *item, int role) const;

  /*! \brief Get category data for display role
   */
  QVariant categoryDisplayRoleData(mdtSqlDatabaseSchemaModelItem *item) const;

  /*! \brief Get table schema data
   */
  QVariant tableSchemaData(const QModelIndex & index, int role) const;

  /*! \brief Get view schema data
   */
  QVariant viewSchemaData(const QModelIndex & index, int role) const;

  /*! \brief Get table population schema data
   */
  QVariant tablePopulationSchemaData(const QModelIndex & index, int role) const;

  mdtSqlDatabaseSchemaModelItem *pvRootItem;
  mdtSqlDatabaseSchema pvSchema;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_MODEL_H
