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

  /*! \brief Default constructor
   */
  mdtSqlCopierCodecSettings()
   : pvType(UnknownCodec)
  {
  }

  /*! \brief Destructor
   */
  virtual ~mdtSqlCopierCodecSettings()
  {
  }

  /*! \brief Get codec type
   */
  CodecType type() const
  {
    return pvType;
  }

  /*! \brief Set database connection name
   *
   * Connection name refers to QSqlDatabase connection name.
   *  This property has only sense for codecs that act on a database.
   *  Default implementation does nothing.
   */
  virtual void setConnectionName(const QString &)
  {
  }

  /*! \brief Get database connection name
   *
   * Default implementation returns allways a empty string.
   * \sa setConnectionName()
   */
  virtual QString connectionName() const
  {
    return QString();
  }

  /*! \brief Set file path
   *
   *  Default implementation does nothing.
   */
  virtual void setFilePath(const QString &)
  {
  }

  /*! \brief Get file path
   *
   * Default implementation returns allways a empty string.
   * \sa setFilePath()
   */
  virtual QString filePath() const
  {
    return QString();
  }

 protected:

  /*! \brief Constructor
   */
  mdtSqlCopierCodecSettings(CodecType type)
   : pvType(type)
  {
  }

 private:

  CodecType pvType;
};

#endif // #ifndef MDT_SQL_COPIER_CODEC_SETTINGS_H
