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
#ifndef MDT_CABLE_LIST_LINK_VERSION_PK_H
#define MDT_CABLE_LIST_LINK_VERSION_PK_H

#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Linkversion Primary Key (PK)
   *
   * Refers to LinkVersion_tbl
   */
  class LinkVersionPk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr LinkVersionPk() noexcept
     : pvIsNull(true), pvVersion(0) {}

    /*! \brief Contruct PK
     */
    explicit constexpr LinkVersionPk(qlonglong version) noexcept
     : pvIsNull(false), pvVersion(version) {}

    // Declare copy noexcept
    constexpr LinkVersionPk(const LinkVersionPk &) noexcept = default;
    LinkVersionPk & operator=(const LinkVersionPk &) noexcept = default;
    // Declare move noexcept
    constexpr LinkVersionPk(LinkVersionPk &&) noexcept = default;
    LinkVersionPk & operator=(LinkVersionPk &&) noexcept = default;

    /*! \brief Set version (Version_PK)
     */
    constexpr void setVersion(qlonglong v) noexcept
    {
      pvIsNull = false;
      pvVersion = v;
    }

    /*! \brief Get version (Version_PK)
     */
    constexpr qlonglong version() const noexcept
    {
      return pvVersion;
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
      pvVersion = 0;
    }

    /*! \brief Get a link version PK from a QVariant version
     *
     * If version is null, a null PK is returned
     *
     * \pre If version has a value (is not null), it must be convertible to qlonglong
     */
    static LinkVersionPk fromQVariant(const QVariant & version)
    {
      LinkVersionPk pk;

      if(version.isNull()){
        return pk;
      }
      Q_ASSERT(version.canConvert<qlonglong>());
      pk.setVersion(version.toLongLong());

      return pk;
    }

   private:

    bool pvIsNull;
    qlonglong pvVersion;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_VERSION_PK_H
