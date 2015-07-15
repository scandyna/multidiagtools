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
#ifndef MDT_CL_LINK_MODIFICATION_KEY_DATA_H
#define MDT_CL_LINK_MODIFICATION_KEY_DATA_H

#include "mdtClLinkKeyData.h"
#include "mdtClLinkVersionKeyData.h"
#include "mdtClModificationKeyData.h"

/*! \brief Link modification key data
 *
 * Refers to LinkModification_tbl
 */
struct mdtClLinkModificationKeyData
{
 private:

  mdtClLinkPkData pvLinkFk;
  mdtClLinkVersionPkData pvLinkVersionFk;
  mdtClModificationPkData pvModificationFk;

 public:

  /*! \brief Set link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  void setLinkFk(const mdtClLinkPkData & fk)
  {
    pvLinkFk = fk;
  }

  /*! \brief Get link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  mdtClLinkPkData linkFk() const
  {
    return pvLinkFk;
  }

  /*! \brief Set link version FK (Version_FK)
   */
  void setLinkVersionFk(const mdtClLinkVersionPkData & fk)
  {
    pvLinkVersionFk = fk;
  }

  /*! \brief Get link version FK (Version_FK)
   */
  mdtClLinkVersionPkData linkVersionFk() const
  {
    return pvLinkVersionFk;
  }

  /*! \brief Set modification FK (Modification_Code_FK)
   */
  void setModificationFk(const mdtClModificationPkData & fk)
  {
    pvModificationFk = fk;
  }

  /*! \brief Get modification FK (Modification_Code_FK)
   */
  mdtClModificationPkData modificationFk() const
  {
    return pvModificationFk;
  }

  /*! \brief Check if key data is null
   *
   * Key is null if linkFk or linkVersionFk or modificationFk is null
   */
  bool isNull() const
  {
    return (pvLinkFk.isNull() || pvLinkVersionFk.isNull() || pvModificationFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pvLinkFk.clear();
    pvLinkVersionFk.clear();
    pvModificationFk.clear();
  }
};

#endif // #ifndef MDT_CL_LINK_MODIFICATION_KEY_DATA_H
