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
#ifndef MDT_CSV_SOURCE_INFO_H
#define MDT_CSV_SOURCE_INFO_H

#include "mdtCsvData.h"
#include "mdtCsvRecordFormat.h"
#include <QString>

/*! \brief Provides some informations about a CSV source
 */
class mdtCsvSourceInfo
{
 public:

  /*! \brief Set format
   *
   * \pre format must contain same count of fields that fieldCount()
   */
  void setFormat(const mdtCsvRecordFormat & format);

  /*! \brief Set source name
   */
  void setSourceName(const QString & name);

  /*! \brief Get source name
   */
  QString sourceName() const
  {
    return pvSourceName;
  }

  /*! \brief Get field count
   */
  int fieldCount() const
  {
    return pvHeader.count();
  }

  /*! \brief Get field name
   *
   * \pre index must be in valid range
   */
  QString fieldName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvHeader.count());
    return pvHeader.columnDataList.at(index).toString();
  }

  /*! \brief Get (user friendly) field type name
   *
   * \pre index must be in valid range
   */
  QString fieldTypeName(int index) const;

 protected:

  /*! \brief Set header
   */
  void setHeader(const mdtCsvRecord & hdr);

 private:

  mdtCsvRecord pvHeader;
  mdtCsvRecordFormat pvRecordFormat;
  QString pvSourceName;
};

#endif // #ifndef MDT_CSV_SOURCE_INFO_H
