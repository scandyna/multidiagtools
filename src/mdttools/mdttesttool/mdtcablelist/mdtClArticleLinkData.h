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
#ifndef MDT_CL_ARTICLE_LINK_DATA_H
#define MDT_CL_ARTICLE_LINK_DATA_H

#include "mdtClArticleLinkKeyData.h"
#include <QVariant>

/*! \brief Article link key data
 *
 * Refers to ArticleLink_tbl
 */
struct mdtClArticleLinkData
{
 private:

  mdtClArticleLinkKeyData pvKeyData;

 public:

  /*! \brief Set key data
   */
  void setKeyData(const mdtClArticleLinkKeyData & key)
  {
    pvKeyData = key;
  }

  /*! \brief Get key data
   */
  inline mdtClArticleLinkKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Check if data is null
   *
   * Data is null if keyData is null
   */
  inline bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Identification
   */
  QVariant indetification;

  /*! \brief SinceVersion
   */
  QVariant sinceVersion;

  /*! \brief Modification
   */
  QVariant modification;

  /*! \brief Resistance
   */
  QVariant resistance;

  /*! \brief Clear data
   */
  void clear()
  {
    pvKeyData.clear();
    indetification.clear();
    sinceVersion.clear();
    modification.clear();
    resistance.clear();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_DATA_H

