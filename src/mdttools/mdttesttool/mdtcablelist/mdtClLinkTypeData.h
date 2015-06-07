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
#ifndef MDT_CL_LINK_TYPE_DATA_H
#define MDT_CL_LINK_TYPE_DATA_H

#include <QVariant>
#include <QString>
#include <QLocale>

/*! \brief Link type
 */
enum class mdtClLinkType_t
{
  Undefined,    /*!< Undefined type (is the case for null data) */
  CableLink,    /*!< Cable link */
  InternalLink, /*!< Device internal link */
  Connection,   /*!< A link of 2 connections */
  TestLink      /*!< Test link */
};

/*! \brief Link type key data
 *
 * Refers to LinkType_tbl
 */
struct mdtClLinkTypeKeyData
{
  /*! \brief Link type code (Code_PK)
   */
  QVariant code;

  /*! \brief Get link type
   */
  mdtClLinkType_t type() const;

  /*! \brief Set link type
   */
  void setType(mdtClLinkType_t t);

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

#endif // #ifndef MDT_CL_LINK_TYPE_DATA_H
