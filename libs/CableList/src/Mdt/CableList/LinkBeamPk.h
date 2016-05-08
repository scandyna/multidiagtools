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
#ifndef MDT_CABLE_LIST_LINK_BEAM_PK_H
#define MDT_CABLE_LIST_LINK_BEAM_PK_H

#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Link beam Primary Key (PK)
   *
   * Refers to LinkBeam_tbl , field Id_PK
   */
  class LinkBeamPk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr LinkBeamPk() noexcept
     : pvIsNull(true), pvId(0) {}

    /*! \brief Contruct PK
     */
    explicit constexpr LinkBeamPk(qlonglong id) noexcept
     : pvIsNull(false), pvId(id) {}

    // Declare copy noexcept
    constexpr LinkBeamPk(const LinkBeamPk &) noexcept = default;
    LinkBeamPk & operator=(const LinkBeamPk &) noexcept = default;
    // Declare move noexcept
    constexpr LinkBeamPk(LinkBeamPk &&) noexcept = default;
    LinkBeamPk & operator=(LinkBeamPk &&) noexcept = default;

    /*! \brief Set id (Id_PK)
     */
    constexpr void setId(qlonglong id) noexcept
    {
      pvIsNull = false;
      pvId = id;
    }

    /*! \brief Get id (Id_PK)
     */
    constexpr qlonglong id() const noexcept
    {
      return pvId;
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return pvIsNull;
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvIsNull = true;
      pvId = 0;
    }

    /*! \brief Get a link beam PK from a QVariant id
     *
     * If id is null, a null PK is returned
     *
     * \pre If ID has a value (is not null), it must be convertible to qlonglong
     */
    static LinkBeamPk fromQVariant(const QVariant & id)
    {
      LinkBeamPk pk;

      if(id.isNull()){
        return pk;
      }
      Q_ASSERT(id.canConvert<qlonglong>());
      pk.setId(id.toLongLong());

      return pk;
    }

  private:

    bool pvIsNull;
    qlonglong pvId;
  };

}} // namespace Mdt{ namespace CableList{
#endif // #ifndef MDT_CABLE_LIST_LINK_BEAM_PK_H

