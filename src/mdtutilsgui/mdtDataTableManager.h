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
#ifndef MDT_DATA_TABLE_MANAGER_H
#define MDT_DATA_TABLE_MANAGER_H

#include "mdtCsvFile.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QString>
#include <QChar>
#include <QStringList>
#include <QVariant>
#include <QDir>
#include <QList>
#include <QSqlField>
#include <QSqlIndex>

/*! \brief Create/manage data sets
 */
class mdtDataTableManager : public QObject
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
  mdtDataTableManager(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief
   */
  ~mdtDataTableManager();

  /*! \brief Set dataset directory
   *
   * \param dir Path or QDir object to directory.
   * \return True if directory exists.
   */
  bool setDataSetDirectory(const QDir &dir);

  /*! \brief Set dataset directory
   */
  QDir dataSetDirectory() const;

  /*! \brief Remove unalowed chars and suffixe _tbl to name to give a table name
   */
  static QString getTableName(const QString &dataSetName);

  /*! \brief Create database and table
   *
   * Internally, Sqlite is used to store data.
   *
   * \param dir Path or QDir object to directory in witch to store data.
   * \param name Name of the database, connection name (see QSqlDatabase) and file.
   * \param primaryKey Contains fields that are part of the primary key.
   *                    If a name is set with QSqlIndex::setName(), it will be used,
   *                    else a name (tableName_PK) is generated.
   * \param fields List of fields, excluding primary key, to create in table.
   *                Note that only a fiew parameters of QSqlField are supported:
   *                 - type: map QVariant type to Sqlite type
   *                 - name: field's name
   * \param mode Behaviour to adopt during database/file creation.
   * \return A valid and open database object on success. See QSqlDatabase::isOpen() for details.
   * \todo Obselete !
   */
  ///static QSqlDatabase createDataSet(const QDir &dir, const QString &name, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, create_mode_t mode);

  /*! \brief Create database and table
   *
   * Internally, Sqlite is used to store data.
   *
   * \param dir Path or QDir object to directory in witch to store data.
   * \param name Name of the database, connection name (see QSqlDatabase) and file.
   * \param primaryKey Contains fields that are part of the primary key.
   *                    If a name is set with QSqlIndex::setName(), it will be used,
   *                    else a name (tableName_PK) is generated.
   * \param fields List of fields, excluding primary key, to create in table.
   *                Note that only a fiew parameters of QSqlField are supported:
   *                 - type: map QVariant type to Sqlite type
   *                 - name: field's name
   * \param mode Behaviour to adopt during database/file creation.
   * \return True on success.
   */
  bool createDataSet(const QDir &dir, const QString &name, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, create_mode_t mode);

  /*! \brief Get internal database instance
   */
  QSqlDatabase database() const;

  /*! \brief Set CSV import/export format
   *
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " )
   * \param comment Comment (typical: # )
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   * \param eol End of line sequence. Usefull if given file was not written from running platform.
   */
  void setCsvFormat(const QString &separator, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol = MDT_NATIVE_EOL);

  /*! \brief Export data to a CSV file
   *
   * \param filePath Path to CSV file
   * \param mode Behaviour to adopt if CSV file allready exists.
   * \return True on successfull export.
   * 
   * \todo Add: separator, data protection, escape char, EOL
   */
  bool exportToCsvFile(const QString &filePath, create_mode_t mode);

  /*! \brief Import a CSV file
   *
   * \param csvFilePath Path to the CSV file.
   * \param mode Behaviour to adopt when data table allready exists.
   * \param dir Destination of the database file. If not defined, csvFilePath's directory is used.
   */
  bool importFromCsvFile(const QString &csvFilePath, create_mode_t mode, const QString &dir = QString());

 private:

  /*! \brief Create database table
   *
   * \see createDataSet()
   * \pre db must be valid and open
   */
  ///static bool createDatabaseTable(const QString &tableName, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, const QSqlDatabase &db);

  /*! \brief Create database table
   *
   * \see createDataSet()
   * \pre db must be valid and open
   */
  bool createDatabaseTable(const QString &tableName, const QSqlIndex &primaryKey, const QList<QSqlField> &fields);

  /*! \brief Drop database table
   *
   * \pre db must be valid and open
   */
  ///static bool dropDatabaseTable(const QString &tableName, const QSqlDatabase &db);

  /*! \brief Drop database table
   *
   * \pre db must be valid and open
   */
  bool dropDatabaseTable(const QString &tableName);

  /*! \brief Open a dialog box that ask the user if he wants do overwrite a existing file
   *
   * \return True if user choosed to overwrite file
   */
  static bool userChooseToOverwrite(const QDir &dir, const QString &fileName);

  QString pvDataSetName;
  QDir pvDataSetDirectory;
  // Database specific members
  QSqlDatabase pvDb;
  // CSV specific members
  QString pvCsvSeparator;
  QString pvCsvDataProtection;
  QChar pvCsvEscapeChar;
  QString pvCsvComment;
  QByteArray pvCsvEol;
};

#endif  // #ifndef MDT_DATA_TABLE_MANAGER_H
