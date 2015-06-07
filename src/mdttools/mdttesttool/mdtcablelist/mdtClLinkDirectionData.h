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
#ifndef MDT_CL_LINK_DIRECTION_DATA_H
#define MDT_CL_LINK_DIRECTION_DATA_H

#include <QVariant>
#include <QString>
#include <QLocale>

/*! \brief Link direction type
 */
enum class mdtClLinkDirection_t
{
  Undefined,      /*!< Undefined direction (is the case for null data) */
  Bidirectional,  /*!< Bidirectional direction (is the case for null data) */
  StartToEnd,     /*!< Start -> end direction */
  EndToStart      /*!< End -> start direction */
};

/*! \brief Link direction key data
 *
 * Refers to LinkDirection_tbl
 */
struct mdtClLinkDirectionKeyData
{
  /*! \brief Link direction code (Code_PK)
   */
  QVariant code;

  /*! \brief Get link direction
   */
  mdtClLinkDirection_t direction() const;

  /*! \brief Set link direction
   */
  void setDirection(mdtClLinkDirection_t d);

  /*! \brief Check if key data is null
   */
  inline bool isNull() const
  {
    return code.isNull();
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    code.clear();
  }
};

#endif // MDT_CL_LINK_DIRECTION_DATA_H