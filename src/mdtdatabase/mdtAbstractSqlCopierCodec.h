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
#ifndef MDT_ABSTRACT_SQL_COPIER_CODEC_H
#define MDT_ABSTRACT_SQL_COPIER_CODEC_H

#include "mdtError.h"
#include "mdtSqlCopierCodecSettings.h"
#include <QString>
#include <QStringList>

/*! \brief Interface to create codecs for SQL copier
 */
class mdtAbstractSqlCopierCodec
{
 public:

  /*! \brief Constructor
   */
  mdtAbstractSqlCopierCodec();

  /*! \internal Diable copy
   */
  mdtAbstractSqlCopierCodec(const mdtAbstractSqlCopierCodec &) = delete;

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlCopierCodec();

  /*! \brief Set codec settings
   *
   * \pre cs type must match the concrete class type
   */
  virtual void setSettings(const mdtSqlCopierCodecSettings & cs)
  {
    Q_ASSERT(cs.type() == mdtSqlCopierCodecSettings::UnknownCodec);
    pvSettings = cs;
  }

  /*! \brief Get codec settings
   */
  mdtSqlCopierCodecSettings settings() const
  {
    return pvSettings;
  }

  /*! \brief Open target regarding settings
   *
   * Default implementation does nothing and allways returns false.
   */
  virtual bool openTarget()
  {
    return false;
  }

  /*! \brief Get field names list
   */
  QStringList fieldNameList() const
  {
    return pvFieldNameList;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 protected:

  /*! \brief Set field names list
   */
  void setFieldNameList(const QStringList & names)
  {
    pvFieldNameList = names;
  }

  /*! \brief Codec settings
   */
  mdtSqlCopierCodecSettings pvSettings;

  /*! \brief Last error
   */
  mdtError pvLastError;

  /*! \brief Call QObject::tr()
   */
  static QString tr(const char *sourceText, const char *disambiguation = nullptr, int n = -1);

 private:

  QStringList pvFieldNameList;
};

#endif // #ifndef MDT_ABSTRACT_SQL_COPIER_CODEC_H
