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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_H
#define MDT_SQL_COPIER_TABLE_MAPPING_H

#include "mdtSqlCopierCodecSettings.h"
#include "mdtSqlCopierFieldMapping.h"
#include <QVector>

/*! \brief Table mapping informations for SQL copier
 */
class mdtSqlCopierTableMapping
{
 public:

  /*! \brief Set source
   */
  void setSource(const mdtSqlCopierCodecSettings & cs)
  {
    pvSource = cs;
  }

  /*! \brief Get source
   */
  mdtSqlCopierCodecSettings source() const
  {
    return pvSource;
  }

  /*! \brief Set destination
   */
  void setDestination(const mdtSqlCopierCodecSettings & cs){
    pvDestination = cs;
  }

  /*! \brief Get destination
   */
  mdtSqlCopierCodecSettings destination() const
  {
    return pvDestination;
  }

  /*! \brief Set field mapping list
   */
  void setFieldMappingList(const QVector<mdtSqlCopierFieldMapping> & fmList)
  {
    pvFieldMappingList = fmList;
  }

 private:

  mdtSqlCopierCodecSettings pvSource;
  mdtSqlCopierCodecSettings pvDestination;
  QVector<mdtSqlCopierFieldMapping> pvFieldMappingList;
};

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_H
