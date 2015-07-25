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
#ifndef MDT_CL_LINK_DATA_H
#define MDT_CL_LINK_DATA_H

#include "mdtClLinkKeyData.h"
#include "mdtValue.h"
#include <QVariant>

/*! \brief Data container class for link data
 *
 * Refers to Link_tbl.
 */
struct mdtClLinkData
{
 private:

  mdtClLinkKeyData pvKeyData;

 public:

  /*! \brief Link identification (Identification)
   */
  QVariant identification;

  /*! \brief Link resistance (Resistance)
   */
  mdtValueDouble resistance;

  /*! \brief Link legth (Length)
   */
  mdtValueDouble length;

  /*! \brief Remarks of link (Remarks)
   */
  QVariant remarks;

  /*! \brief Set PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  void setPk(const mdtClLinkPkData & pk)
  {
    pvKeyData.setPk(pk);
  }

  /*! \brief Set link type (LinkType_Code_FK)
   */
  void setLinkType(mdtClLinkType_t t)
  {
    pvKeyData.setLinkType(t);
  }

  /*! \brief Set link type code (LinkType_Code_FK)
   */
  void setLinkTypeCode(const QVariant & c)
  {
    pvKeyData.setLinkTypeCode(c);
  }

  /*! \brief Set link direction (LinkDirection_Code_FK)
   */
  void setLinkDirection(mdtClLinkDirection_t d)
  {
    pvKeyData.setLinkDirection(d);
  }

  /*! \brief Set link direction code (LinkDirection_Code_FK)
   */
  void setLinkDirectionCode(const QVariant & c)
  {
    pvKeyData.setLinkDirectionCode(c);
  }

  /*! \brief Set article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
   */
  void setArticleLinkFk(const mdtClArticleLinkPkData & fk)
  {
    pvKeyData.setArticleLinkFk(fk);
  }

  /*! \brief Set wire ID (Wire_Id_FK)
   */
  void setWireId(const QVariant & id)
  {
    pvKeyData.setWireId(id);
  }

  /*! \brief Get key data
   */
  mdtClLinkKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Get PK data
   */
  mdtClLinkPkData pk() const
  {
    return pvKeyData.pk();
  }

  /*! \brief Check if link data is null
   *
   * \sa mdtClLinkKeyData::isNull()
   */
  bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear();
};

#endif // #ifndef MDT_CL_LINK_DATA_H
