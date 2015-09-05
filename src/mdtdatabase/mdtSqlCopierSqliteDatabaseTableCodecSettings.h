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
#ifndef MDT_SQL_COPIER_SQLITE_DATABASE_TABLE_CODEC_SETTINGS_H
#define MDT_SQL_COPIER_SQLITE_DATABASE_TABLE_CODEC_SETTINGS_H

#include "mdtSqlCopierCodecSettings.h"

/*! \brief SQL copier database table codec settings
 */
class mdtSqlCopierSqliteDatabaseTableCodecSettings : public mdtSqlCopierCodecSettings
{
 public:

  /*! \brief Constructor
   */
  mdtSqlCopierSqliteDatabaseTableCodecSettings()
   : mdtSqlCopierCodecSettings(mdtSqlCopierCodecSettings::SqliteCodec)
  {
  }

  /*! \brief Set database connection name
   */
  void setConnectionName(const QString & cn)
  {
    pvConnectionName = cn;
  }

  /*! \brief Get database connection name
   */
  QString connectionName() const
  {
    return pvConnectionName;
  }

  /*! \brief Set file path
   *
   *  For SQLite, file path is the QSqlDatabase's database name.
   */
  void setFilePath(const QString & path)
  {
    pvDatabaseName = path;
  }

  /*! \brief Get file path
   *
   * \sa setFilePath()
   */
  QString filePath() const
  {
    return pvDatabaseName;
  }

  /*! \brief Set table name
   */
  void setTableName(const QString & name)
  {
    pvTableName = name;
  }

  /*! \brief Get table name
   */
  QString tableName() const
  {
    return pvTableName;
  }

 private:

  QString pvConnectionName;
  QString pvDatabaseName;
  QString pvTableName;
};

#endif // #ifndef MDT_SQL_COPIER_SQLITE_DATABASE_TABLE_CODEC_SETTINGS_H
