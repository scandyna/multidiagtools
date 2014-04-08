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
#ifndef MDT_TT_TEST_LINK_DATA_H
#define MDT_TT_TEST_LINK_DATA_H

#include "mdtSqlRecord.h"

/*! \brief Data container for test link
 */
class mdtTtTestLinkData : public mdtSqlRecord
{
  public:

  /*! \brief Construct a empty test link data
   */
  mdtTtTestLinkData();

  /*! \brief Construct a test link data based on given record
   *
   * \pre record must contain all fields from TestLink_tbl
   */
  mdtTtTestLinkData(const QSqlRecord & record);

  /*! \brief Setup test link data fields
   */
  bool setup(const QSqlDatabase & db);
};

#endif // #ifndef MDT_TT_TEST_LINK_DATA_H
