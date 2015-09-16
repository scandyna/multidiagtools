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
#ifndef MDT_SQL_COPIER_CODEC_H
#define MDT_SQL_COPIER_CODEC_H

#include "mdtSqlCopierCodecSettings.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>
#include <memory>

class mdtAbstractSqlCopierCodec;

/*! \brief SQL copier codec
 */
class mdtSqlCopierCodec
{
 public:

  /*! \brief Constructor
   */
  mdtSqlCopierCodec();

  /*! \brief Destructor
   */
  ~mdtSqlCopierCodec();

  /*! \internal Disable copy
   */
  mdtSqlCopierCodec(const mdtSqlCopierCodec &) = delete;

  /*! \brief Set codec settings
   *
   * Internally, a instance of the concrete class, regarding cs type, is created if needed.
   */
  void setSettings(const mdtSqlCopierCodecSettings & cs);

  /*! \brief Get settings
   */
  mdtSqlCopierCodecSettings settings() const;

  /*! \brief Open target regarding settings
   */
  bool openTarget();

  /*! \brief Close target and clear cached data
   *
   * After call of this function, fieldNameList() will return a empty result.
   */
  void close();

  /*! \brief Get field names list
   */
  QStringList fieldNameList() const;

  /*! \brief Get last error
   */
  mdtError lastError() const;

 private:

  std::unique_ptr<mdtAbstractSqlCopierCodec> pvImpl;
};

#endif // #ifndef MDT_SQL_COPIER_CODEC_H
