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
#ifndef MDT_CABLE_LIST_LINK_TYPE_PK_H
#define MDT_CABLE_LIST_LINK_TYPE_PK_H

#include "LinkType.h"
#include <QString>
#include <QLatin1String>
#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Link type primary key
   *
   * Refers to LinkType_tbl
   */
  class LinkTypePk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr LinkTypePk() noexcept
     : pvType(LinkType::Undefined) {}
 
    /*! \brief Construct a PK for type
     */
    explicit constexpr LinkTypePk(LinkType type) noexcept
     : pvType(type) {}

    // Declare copy noexcept
    constexpr LinkTypePk(const LinkTypePk &) noexcept = default;
    LinkTypePk & operator=(const LinkTypePk &) noexcept = default;
    // Declare move noexcept
    constexpr LinkTypePk(LinkTypePk &&) noexcept = default;
    LinkTypePk & operator=(LinkTypePk &&) noexcept = default;

    /*! \brief Set modification type
     */
    constexpr void setType(LinkType type) noexcept
    {
      pvType = type;
    }

    /*! \brief Get modification type
     */
    constexpr LinkType type() const noexcept
    {
      return pvType;
    }

    /*! \brief Get code
     */
    QString code() const
    {
      switch(pvType){
        case LinkType::CableLink:
          return QStringLiteral("CABLELINK");
        case LinkType::InternalLink:
          return QStringLiteral("INTERNLINK");
        case LinkType::Connection:
          return QStringLiteral("CONNECTION");
        case LinkType::TestLink:
          return QStringLiteral("TESTLINK");
        case LinkType::Undefined:
          return QStringLiteral("");
      }
      return QStringLiteral("");
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return (pvType == LinkType::Undefined);
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvType = LinkType::Undefined;
    }

    /*! \brief Get a PK from code
     */
    static LinkTypePk fromCode(const QString & code)
    {
      const QString c = code;

      if(c == QLatin1String("CABLELINK")){
        return LinkTypePk(LinkType::CableLink);
      }else if(c == QLatin1String("INTERNLINK")){
        return LinkTypePk(LinkType::InternalLink);
      }else if(c == QLatin1String("CONNECTION")){
        return LinkTypePk(LinkType::Connection);
      }else if(c == QLatin1String("TESTLINK")){
        return LinkTypePk(LinkType::TestLink);
      }else{
        return LinkTypePk(LinkType::Undefined);
      }
    }

    /*! \brief Get a PK from QVariant
     *
     * if code is null, a null PK is returned
     *
     * \pre If code is not null, it must be a string
     */
    static LinkTypePk fromQVariant(const QVariant & code)
    {
      if(code.isNull()){
        return LinkTypePk();
      }
      Q_ASSERT(code.canConvert<QString>());
      return fromCode(code.toString());
    }

   private:

    LinkType pvType;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_TYPE_PK_H
