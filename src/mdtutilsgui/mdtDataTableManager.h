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
#ifndef MDT_DATA_TABLE_MANAGER_H
#define MDT_DATA_TABLE_MANAGER_H

#include "mdtCsvFile.h"
#include "mdtFieldMap.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtSqlSchemaTable.h"
#include "mdtFieldMapField.h"
#include "mdtError.h"
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QString>
#include <QChar>
#include <QVariant>
#include <QDir>
#include <QList>
#include <QHash>
#include <QSqlField>
#include <QSqlIndex>

class QWidget;
class QAbstractTableModel;

/*! \brief Create/manage data sets
 */
class mdtDataTableManager : public QObject
{
 Q_OBJECT

 public:

  /*! \brief
   */
  mdtDataTableManager(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief
   *
   * Note: this will delete internal model.
   *  Be avare when referencing pointer returned by model().
   */
  ~mdtDataTableManager();

  /*! \brief Get last error
   */
  mdtError lastError() const;

  /*! \brief Close
   *
   * Will close database.
   */
  void close();

  /*! \brief Set dataset directory
   *
   * \param dir Path or QDir object to directory.
   * \return True if directory exists.
   * \todo Not used yet.
   */
  bool setDataSetDirectory(const QDir &dir);

  /*! \brief Set dataset directory
   *
   * \todo Not used yet.
   */
  QDir dataSetDirectory() const;

  /*! \brief Get dataset table name
   */
  const QString dataSetTableName() const;

  /*! \brief Remove unalowed chars and suffixe _tbl to name to give a table name
   */
  static QString getTableName(const QString &dataSetName);

  /*! \brief Create database and table
   *
   * Internally, Sqlite is used to store data.
   *
   * Note: this will delete internal model and create a new one.
   *  Be avare when referencing pointer returned by model().
   *
   * \param dir Path or QDir object to directory in witch to store data.
   * \param table Schema of the table to create .
   * \param name Name of the database, connection name (see QSqlDatabase) and file.
   * \param mode Behaviour to adopt during database/file creation.
   * \return True on success.
   */
  bool createDataSet(const QDir &dir, const QString &name, mdtSqlSchemaTable &table, mdtSqlDatabaseManager::createMode_t mode);

  /*! \brief Get internal database instance
   */
  QSqlDatabase database() const;

  /*! \brief Enable/disable progress dialog
   *
   * If progress dialog is enabled, some methods will show her progress in a dialog.
   */
  void enableProgressDialog(bool enable);

  /*! \brief Set CSV import/export format
   *
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " )
   * \param comment Comment (typical: # )
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   * \param eol End of line sequence. Usefull if given file was not written from running platform.
   * \param encoding Name of the encoding format, for example, UTF-8
   *                  If given encoding is not available, UTF-8 is choosen.
   */
  void setCsvFormat(const QString &separator, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol = MDT_NATIVE_EOL, const QByteArray &encoding = "UTF-8");

  /*! \brief Export data to a CSV file
   *
   * \param filePath Path to CSV file
   * \param mode Behaviour to adopt if CSV file allready exists.
   * \return True on successfull export.
   * 
   * \todo Add: separator, data protection, escape char, EOL
   * \todo Implement ...
   */
  bool exportToCsvFile(const QString &filePath, mdtSqlDatabaseManager::createMode_t mode);

  /*! \brief Import a CSV file
   *
   * Note: this will delete internal model and create a new one.
   *  Be avare when referencing pointer returned by model().
   *
   * \param csvFilePath Path to the CSV file.
   * \param mode Behaviour to adopt when data table allready exists.
   * \param dir Destination of the database file. If not defined, csvFilePath's directory is used.
   * \param pkFields List of fields to considere as part of primary key.
   *                  If list is empty, one named id_PK will be created.
   * \pre All given CSV header items to use as part of primary key must exit in CSV file (See pkFields argument).
   */
  bool importFromCsvFile(const QString &csvFilePath, mdtSqlDatabaseManager::createMode_t mode, const QString &dir = QString(), const QStringList &pkFields = QStringList());

  /*! \brief Copy a source table to a destination table
   *
   * \param sourceTableName Name of the source table .
   * \param destinationTableName Name of the destination table .
   * \param mode Behaviour to adopt if destination table allready exists .
   * \param sourceDatabase Instance of source database .
   * \param destinationDatabase Instance of destination database .
   * \return True on success, false if a error occured . Use lastError() to know what gone wrong in second case .
   */
  bool copyTable(const QString &sourceTableName, const QString & destinationTableName, mdtSqlDatabaseManager::createMode_t mode, QSqlDatabase sourceDatabase = QSqlDatabase(), QSqlDatabase destinationDatabase = QSqlDatabase());

  /*! \brief Add a mapping between CSV header part (item) and model field
   *
   * Field mapping is used for relationship between CSV header,
   *  model's field name and texts to display in model's header.
   *  Internally, mdtFieldMap class is used for this.
   *
   * During CSV import, all unspecified maps are automatically created as follow:
   *  - Data are imported as string (type: QVariant::String)
   *  - Field name is based on CSV header item, but unalowed chars are replaced by '_'
   *  - Display text is the CSV header item.
   *  - One field is created in model for one CSV header item (column).
   *
   * If some of this default behaviour is not what is needed for some fields,
   *  it's possible to add a mapping with this method befor calling importFromCsvFile().
   *
   * It's also possible to split a CSV header item to many model fields.
   *  To do this, add several mapping with same csvHeaderItem, but using
   *  each time another fieldName. To know wich part of a CSV data item must
   *  be stored in witch field, csvDataItemStartOffset and csvDataItemEndOffset
   *  must be specified.
   *
   * \param csvHeaderItem CSV heaer part (column).
   * \param fieldName Model's field name to create.
   * \param displayText Text to display in model's header.
   * \param dataType Data will be converted to this data type during import.
   * \param csvDataItemStartOffset If >= 0, only data starting at this offset will be imported from CSV data item into model's field.
   * \param csvDataItemEndOffset If >= 0, only data until this offset will be imported from CSV data item into model's field.
   * \sa mdtFieldMap
   * \sa mdtFieldMapItem
   */
  void addFieldMapping(const QString &csvHeaderItem, const QString &fieldName, const QString &displayText, QVariant::Type dataType, int csvDataItemStartOffset = -1, int csvDataItemEndOffset = -1);

  /*! \brief Clear fields mapping
   *
   * \sa addFieldMapping()
   */
  void clearFieldMap();

  /*! \brief Get the source header
   */
  QStringList sourceHeader() const;

  /*! \brief Get database header
   */
  const QStringList databaseHeader() const;

  /*! \brief Set database header to model
   */
  void setDatabaseHeaderToModel(QAbstractTableModel *model);

  /*! \brief Get display texts referenced by field names
   *
   * \return QHash with fieldName as key and displayText as value.
   */
  QHash<QString, QString> displayTextsByFieldNames() const;

 private slots:

  /*! \brief Set the abort flag
   *
   * Abort flag is used by importFromCsvFile() to know if import must be aborted
   */
  void setAbortFlag();

 private:

  /*! \brief Open a dialog box that ask the user if he wants do overwrite a existing file
   *
   * \return True if user choosed to overwrite file
   */
  static bool userChooseToOverwrite(const QDir &dir, const QString &fileName);

  /*! \brief Commit some rows of data to database
   *
   * \param rows Rows of data to commit
   * \param fields Destination fields
   * \return True on success, errors are logged with mdtError system.
   */
  bool commitRowsToDatabase(const QList<QVariantList> &rows, const QList<mdtFieldMapField> &fields, const QString & autoValuePkFieldName);

  /*! \brief Get SQL statement for insert using QSqlQuery bind values
   */
  const QString insertBindValuesPrepareStatement(const QList<mdtFieldMapField> &fields, const QString & autoValuePkFieldName) const;

  Q_DISABLE_COPY(mdtDataTableManager);

  QString pvDataSetName;
  QDir pvDataSetDirectory;
  mdtFieldMap pvFieldMap;
  mdtError pvLastError;
  // Database specific members
  mdtSqlDatabaseManager *pvDatabaseManager;
  QString pvDataSetTableName;
  QStringList pvDatabaseHeader;
  // CSV specific members
  QString pvCsvSeparator;
  QString pvCsvDataProtection;
  QChar pvCsvEscapeChar;
  QString pvCsvComment;
  QByteArray pvCsvEol;
  QByteArray pvEncoding;
  // Some GUI specific members
  bool pvProgressDialogEnabled;   // If true, some method will show a progress dialog
  bool pvAbort;
};

#endif  // #ifndef MDT_DATA_TABLE_MANAGER_H
