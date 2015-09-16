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
#ifndef MDT_SQL_COPIER_CODEC_SETTINGS_H
#define MDT_SQL_COPIER_CODEC_SETTINGS_H

#include <QString>

/*! \brief SQL copier settings
 */
class mdtSqlCopierCodecSettings
{
 public:

  /*! \brief SQL copier codec type
   */
  enum CodecType
  {
    UnknownCodec,   /*!< Unknown codec */
    SqliteCodec,    /*!< SQLite codec */
    CsvCodec,       /*!< CSV file codec */
    ClipboardCodec  /*!< Clipboard codec */
  };

  /*! \brief Construct a codec settings of given type
   */
  mdtSqlCopierCodecSettings(CodecType type = UnknownCodec)
   : pvType(type)
  {
  }

  /*! \brief Set codec type
   */
  void setCodecType(CodecType type)
  {
    pvType = type;
  }

  /*! \brief Get codec type
   */
  CodecType type() const
  {
    return pvType;
  }

  /*! \brief Check if codec settings is null
   *
   * Codec settings is null if type is UnknownCodec
   */
  bool isNull() const
  {
    return (pvType == UnknownCodec);
  }

  /*! \brief Clear
   *
   * Clear all properties and set codec type to UnknownCodec
   */
  void clear()
  {
    pvType = UnknownCodec;
    pvConnectionName.clear();
    pvFilePath.clear();
    pvTableName.clear();
  }

  /*! \brief Set database connection name
   *
   * Connection name refers to QSqlDatabase connection name.
   *  This property has only sense for codecs that act on a database.
   */
  [[deprecated]]
  void setConnectionName(const QString & cn)
  {
    pvConnectionName = cn;
  }

  /*! \brief Get database connection name
   *
   * \sa setConnectionName()
   */
  [[deprecated]]
  QString connectionName() const
  {
    return pvConnectionName;
  }

  /*! \brief Set file path
   *
   *  Note: for SqliteCodec, this is the same as QSqlDatabase's databaseName.
   */
  void setFilePath(const QString & path)
  {
    pvFilePath = path;
  }

  /*! \brief Get file path
   *
   * \sa setFilePath()
   */
  QString filePath() const
  {
    return pvFilePath;
  }

  /*! \brief Set table name
   *
   * This property has only sense for codecs that act on a database.
   */
  void setTableName(const QString & name)
  {
    pvTableName = name;
  }

  /*! \brief Get table name
   *
   * \sa setTableName()
   */
  QString tableName() const
  {
    return pvTableName;
  }

 private:

  CodecType pvType;
  QString pvConnectionName;
  QString pvFilePath;
  QString pvTableName;
};

#endif // #ifndef MDT_SQL_COPIER_CODEC_SETTINGS_H
