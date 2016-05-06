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
#ifndef MDT_CABLE_LIST_LINK_DIRECTION_PK_H
#define MDT_CABLE_LIST_LINK_DIRECTION_PK_H

#include "LinkDirectionType.h"
#include <QString>
#include <QLatin1String>
#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Link direction primary key
   *
   * Refers to LinkDirection_tbl
   */
  class LinkDirectionPk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr LinkDirectionPk() noexcept
     : pvDirection(LinkDirectionType::Undefined) {}

    /*! \brief Construct a PK for direction
     */
    explicit constexpr LinkDirectionPk(LinkDirectionType d) noexcept
     : pvDirection(d) {}

    // Declare copy noexcept
    constexpr LinkDirectionPk(const LinkDirectionPk &) noexcept = default;
    LinkDirectionPk & operator=(const LinkDirectionPk &) noexcept = default;
    // Declare move noexcept
    constexpr LinkDirectionPk(LinkDirectionPk &&) noexcept = default;
    LinkDirectionPk & operator=(LinkDirectionPk &&) noexcept = default;

    /*! \brief Set direction
     */
    constexpr void setDirection(LinkDirectionType d) noexcept
    {
      pvDirection = d;
    }

    /*! \brief Get direction
     */
    constexpr LinkDirectionType direction() const noexcept
    {
      return pvDirection;
    }

    /*! \brief Get code
     */
    QString code() const
    {
      switch(pvDirection){
        case LinkDirectionType::Bidirectional:
          return QStringLiteral("BID");
        case LinkDirectionType::StartToEnd:
          return QStringLiteral("STE");
        case LinkDirectionType::EndToStart:
          return QStringLiteral("ETS");
        case LinkDirectionType::Undefined:
          return QStringLiteral("");
      }
      return QStringLiteral("");
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return (pvDirection == LinkDirectionType::Undefined);
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvDirection = LinkDirectionType::Undefined;
    }

    /*! \brief Get a PK from code
     */
    static LinkDirectionPk fromCode(const QString & code)
    {
      const QString c = code;

      if(c == QLatin1String("BID")){
        return LinkDirectionPk(LinkDirectionType::Bidirectional);
      }else if(c == QLatin1String("STE")){
        return LinkDirectionPk(LinkDirectionType::StartToEnd);
      }else if(c == QLatin1String("ETS")){
        return LinkDirectionPk(LinkDirectionType::EndToStart);
      }else{
        return LinkDirectionPk(LinkDirectionType::Undefined);
      }
    }

    /*! \brief Get a PK from QVariant
     *
     * if code is null, a null PK is returned
     *
     * \pre If code is not null, it must be a string
     */
    static LinkDirectionPk fromQVariant(const QVariant & code)
    {
      if(code.isNull()){
        return LinkDirectionPk();
      }
      Q_ASSERT(code.canConvert<QString>());
      return fromCode(code.toString());
    }

   private:

    LinkDirectionType pvDirection;
  };


}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_DIRECTION_PK_H
