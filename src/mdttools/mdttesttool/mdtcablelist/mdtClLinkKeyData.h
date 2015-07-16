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
#ifndef MDT_CL_LINK_KEY_DATA_H
#define MDT_CL_LINK_KEY_DATA_H

#include "mdtClLinkTypeData.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClArticleLinkKeyData.h"
#include <QVariant>

/*! \brief Link primary key data
 *
 * Refers to Link_tbl
 */
struct mdtClLinkPkData
{
  /*! \brief Start connection ID (UnitConnectionStart_Id_FK)
   */
  QVariant connectionStartId;

  /*! \brief End connection ID (UnitConnectionEnd_Id_FK)
   */
  QVariant connectionEndId;

  /*! \brief Check if primary key is null
   *
   * Primary key is null if one of the connection ID is null
   */
  bool isNull() const
  {
    return (connectionStartId.isNull() || connectionEndId.isNull());
  }

  /*! \brief Clear primary key
   */
  void clear()
  {
    connectionStartId.clear();
    connectionEndId.clear();
  }
};

/*! \brief Link key data
 *
 * Refers to Link_tbl
 */
struct mdtClLinkKeyData
{
 private:

  mdtClLinkPkData pvPk;
  mdtClLinkTypeKeyData pvLinkTypeFk;
  mdtClLinkDirectionKeyData pvLinkDirectionFk;
  mdtClArticleLinkPkData pvArticleLinkFk;
  QVariant pvWireId;

 public:

  /*! \brief Set PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  void setPk(const mdtClLinkPkData & pk)
  {
    pvPk = pk;
  }

  /*! \brief Get PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  mdtClLinkPkData pk() const
  {
    return pvPk;
  }

  /*! \brief Set link type (LinkType_Code_FK)
   */
  void setLinkType(mdtClLinkType_t t)
  {
    pvLinkTypeFk.setType(t);
  }

  /*! \brief Set link type code (LinkType_Code_FK)
   */
  void setLinkTypeCode(const QVariant & c)
  {
    pvLinkTypeFk.code = c;
  }

  /*! \brief Get link type FK (LinkType_Code_FK)
   */
  mdtClLinkTypeKeyData linkTypeFk() const
  {
    return pvLinkTypeFk;
  }

  /*! \brief Set link direction (LinkDirection_Code_FK)
   */
  void setLinkDirection(mdtClLinkDirection_t d)
  {
    pvLinkDirectionFk.setDirection(d);
  }

  /*! \brief Set link direction code (LinkDirection_Code_FK)
   */
  void setLinkDirectionCode(const QVariant & c)
  {
    pvLinkDirectionFk.code = c;
  }

  /*! \brief Get link direction FK (LinkDirection_Code_FK)
   */
  mdtClLinkDirectionKeyData linkDirectionFk() const
  {
    return pvLinkDirectionFk;
  }

  /*! \brief Set article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
   */
  void setArticleLinkFk(const mdtClArticleLinkPkData & fk)
  {
    pvArticleLinkFk = fk;
  }

  /*! \brief Get article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
   */
  mdtClArticleLinkPkData articleLinkFk() const
  {
    return pvArticleLinkFk;
  }

  /*! \brief Set wire ID (Wire_Id_FK)
   */
  void setWireId(const QVariant & id)
  {
    pvWireId = id;
  }

  /*! \brief Get wire ID (Wire_Id_FK)
   */
  QVariant wireId() const
  {
    return pvWireId;
  }

  /*! \brief Check if key is null
   *
   * Key is null if pk or linkTypeFk or linkDirectionFk is null
   */
  bool isNull() const
  {
    return (pvPk.isNull() || pvLinkTypeFk.isNull() || pvLinkDirectionFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pvPk.clear();
    pvLinkTypeFk.clear();
    pvLinkDirectionFk.clear();
    pvArticleLinkFk.clear();
    pvWireId.clear();
  }
};

#endif // #ifndef MDT_CL_LINK_KEY_DATA_H
