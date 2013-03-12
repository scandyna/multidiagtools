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
#ifndef MDT_DATA_TABLE_MODEL_H
#define MDT_DATA_TABLE_MODEL_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDir>
#include <QList>
#include <QSqlField>
#include <QSqlIndex>

/*! \brief
 */
class mdtDataTableModel : public QSqlTableModel
{
 Q_OBJECT

 public:

  /*! \brief Dataset creation mode
   */
  enum create_mode_t{
                    OverwriteExisting,  /*!< If file/database allready exists, it will be overwritten */
                    KeepExisting,       /*!< If file/database allready exists, it will simply be open */
                    FailIfExists,       /*!< If file/database allready exists, create method will fail */
                    AskUserIfExists     /*!< If file/database allready exists, 
                                              a dialog box will ask the user if it must be overwritten or not */
  };

  /*! \brief
   */
  mdtDataTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief
   */
  ~mdtDataTableModel();

  /*! \brief Set dataset directory
   *
   * \param dir Path or QDir object to directory.
   * \return True if directory exists.
   */
  bool setDataSetDirectory(const QDir &dir);

  /*! \brief Set dataset directory
   */
  QDir dataSetDirectory() const;

  /*! \brief Create database and table
   *
   * Internally, Sqlite is used to store data.
   *
   * \param dir Path or QDir object to directory in witch to store data.
   * \param name Name of the database, connection name (see QSqlDatabase) and file.
   * \param fields List of fields, excluding primary key, to create in table.
   *                Note that only a fiew parameters of QSqlField are supported:
   *                 - isAutoValue: will enable auto increment
   *                 - type: map QVariant type to Sqlite type
   *                 - name: field's name
   * \param primaryKey Contains fields ................................................................
   * \param mode Behaviour to adopt during database/file creation.
   * \return A valid and open database object on success. See QSqlDatabase::isOpen() for details.
   */
  ///bool createDataSet(const QString &name, const QStringList &fields, create_mode_t mode);
  static QSqlDatabase createDataSet(const QDir &dir, const QString &name, const QList<QSqlField> &fields, const QSqlIndex &primaryKey, create_mode_t mode);

  /*! \brief Add a row of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   *
   * \param data A map with field name as key and associate data to add.
   * \param role See QSqlTableModel.
   * \return True on success. See QSqlTableModel for known errors.
   */
  bool addRow(const QMap<QString,QVariant> &data, int role = Qt::EditRole);

  /*! \brief Set data for a given index in model
   *
   * This is a helper method wich uses QSqlTableModel's setData methods.
   *
   * \param row Row to update.
   * \param column Column to update.
   * \param value Value to set at given index.
   * \param role See QSqlTableModel.
   * \return True on success. If row or column are out of bound, false is returned.
   *          See QSqlTableModel for details.
   */
  bool setData(int row, int column, const QVariant &value, int role = Qt::EditRole);

  /*! \brief Set data for a given index in model
   *
   * This is a helper method wich uses QSqlTableModel's setData methods.
   *
   * \param row Row to update.
   * \param field Name of field to update.
   * \param value Value to set at given index.
   * \param role See QSqlTableModel.
   * \return True on success. If row is out of bound or field not exists, false is returned.
   *          See QSqlTableModel for details.
   */
  bool setData(int row, const QString &field, const QVariant &value, int role = Qt::EditRole);

  /*! \brief
   * 
   *  mode.....
   */
  bool exportToCsvFile(const QString &filePath);

  /*! \brief
   */
  bool importFromCsvFile(const QString &filePath);

 private:

  /*! \brief Remove unalowed chars and suffixe _tbl to name to give a table name
   */
  static QString getTableName(const QString &dataSetName);

  /*! \brief Create database table
   *
   * \see createDataSet()
   * \pre db must be valid and open
   */
  static bool createDatabaseTable(const QString &tableName, const QList<QSqlField> &fields, const QSqlDatabase &db);

  /*! \brief Drop database table
   *
   * \pre db must be valid and open
   */
  static bool dropDatabaseTable(const QString &tableName, const QSqlDatabase &db);

  
  
  QString pvDataSetName;
  QDir pvDataSetDirectory;
};

#endif  // #ifndef MDT_DATA_TABLE_MODEL_H
