/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_LINK_DATA_H
#define MDT_CABLE_LIST_LINK_DATA_H

#include "LinkKey.h"
#include "Mdt/Numeric/Resistance.h"
#include "Mdt/Numeric/Length.h"
#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Data container class for link data
   *
   * Refers to Link_tbl.
   */
  class LinkData
  {
   public:

    /*! \brief Set key
     */
    void setKey(const LinkKey & key) noexcept
    {
      pvKey = key;
    }

    /*! \brief Get key
     */
    LinkKey key() const noexcept
    {
      return pvKey;
    }

    /*! \brief Set identification
     */
    void setIdentification(const QVariant & identification)
    {
      pvIdentification = identification;
    }

    /*! \brief Get identification
     */
    QVariant identification() const
    {
      return pvIdentification;
    }

    /*! \brief Set resistance
     */
    void setResistance(Mdt::Numeric::Resistance r)
    {
      pvResistance = r;
    }

    /*! \brief Get resistance
     */
    Mdt::Numeric::Resistance resistance() const
    {
      return pvResistance;
    }

    /*! \brief Set length
     */
    void setLength(Mdt::Numeric::Length l)
    {
      pvLength = l;
    }

    /*! \brief Get length
     */
    Mdt::Numeric::Length length() const
    {
      return pvLength;
    }

    /*! \brief Check if link data is null
     *
     * Link data is null if its key is null
     *
     * \sa LinkKey::isNull()
     */
    bool isNull() const noexcept
    {
      return pvKey.isNull();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvKey.clear();
      pvIdentification.clear();
      pvResistance.clear();
      pvLength.clear();
    }

   private:

    LinkKey pvKey;
    QVariant pvIdentification;
    Mdt::Numeric::Resistance pvResistance;
    Mdt::Numeric::Length pvLength;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_DATA_H
