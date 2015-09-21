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

/*! \brief Tree model to display some database schema informations
 */
class mdtSqlDatabaseSchemaModel : public QAbstractItemModel
{
 Q_OBJECT

 public:

  /*! \brief Object category
   */
  enum ObjectCategory
  {
    Invalid,          /*!< Not a valid object */
    Table,            /*!< Object refers to a table schema */
    View,             /*!< Object refers to a view schema */
    TablePopulation   /*!< Object refers to a table population schema */
  };

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

  /*! \brief Set progress value
   *
   * \param objectCategory Category of target object
   * \param value Progress (0 <= value <= 100)
   */
  void setProgress(mdtSqlDatabaseSchemaModel::ObjectCategory objectCategory, int value);

  /*! \brief Set progress value
   *
   * \param objectCategory Category of target object
   * \param objectName Name of target object
   * \param value Progress (0 <= value <= 100)
   *
   * For example, if we would set progress for a table named Clien_tbl,
   *  we also call setProgress(mdtSqlDatabaseSchemaModel::Table, "Clien_tbl")
   */
  void setProgress(mdtSqlDatabaseSchemaModel::ObjectCategory objectCategory, const QString & objectName, int value);

 private:

  /*! \brief Column index
   */
  enum ColumnIndex
  {
    ObjectColumnIndex = 0,    /*!< Column that display categories (Tables, Views, ..) and object names */
    ProgressColumnIndex = 1   /*!< Column that display progress */
  };

  /*! \brief Get object category data
   */
  QVariant categoryData(mdtSqlDatabaseSchemaModelItem *item, int role) const;

  /*! \brief Get category data for display role
   */
  QVariant categoryDisplayRoleData(mdtSqlDatabaseSchemaModelItem *item) const;

  /*! \brief Get object name data
   */
  QVariant nameData(const QModelIndex & index, ObjectCategory oc, int role) const;

  /*! \brief Get table schema data
   */
  QVariant tableSchemaData(const QModelIndex & index, int role) const;

  /*! \brief Get view schema data
   */
  QVariant viewSchemaData(const QModelIndex & index, int role) const;

  /*! \brief Get table population schema data
   */
  QVariant tablePopulationSchemaData(const QModelIndex & index, int role) const;

  /*! \brief Get the index of given object category
   */
  QModelIndex indexOf(ObjectCategory oc) const;

  /*! \brief Get the index of given object category and name
   */
  QModelIndex indexOf(ObjectCategory oc, const QString & objectName) const;

  mdtSqlDatabaseSchemaModelItem *pvRootItem;
  mdtSqlDatabaseSchema pvSchema;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_MODEL_H
