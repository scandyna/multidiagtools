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
#ifndef MDT_CABLE_LIST_MODIFICATION_PK_H
#define MDT_CABLE_LIST_MODIFICATION_PK_H

#include "ModificationType.h"
#include <QString>
#include <QVariant>

namespace Mdt{ namespace CableList{

  /*! \brief Modification primary key
   *
   * Refers to Modification_tbl
   */
  class ModificationPk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr ModificationPk() noexcept
     : pvType(ModificationType::Undefined) {}

    /*! \brief Construct a PK for type
     */
    explicit constexpr ModificationPk(ModificationType type) noexcept
     : pvType(type) {}

    // Declare copy noexcept
    constexpr ModificationPk(const ModificationPk &) noexcept = default;
    ModificationPk & operator=(const ModificationPk &) noexcept = default;
    // Declare move noexcept
    constexpr ModificationPk(ModificationPk &&) noexcept = default;
    ModificationPk & operator=(ModificationPk &&) noexcept = default;

    /*! \brief Set modification type
     */
    constexpr void setType(ModificationType type) noexcept
    {
      pvType = type;
    }

    /*! \brief Get modification type
     */
    constexpr ModificationType type() const noexcept
    {
      return pvType;
    }

    /*! \brief Get code
     */
    QString code() const
    {
      switch(pvType){
        case ModificationType::New:
          return QStringLiteral("NEW");
        case ModificationType::Rem:
          return QStringLiteral("REM");
        case ModificationType::ModNew:
          return QStringLiteral("MODNEW");
        case ModificationType::ModRem:
          return QStringLiteral("MODREM");
        case ModificationType::Exists:
          return QStringLiteral("EXISTS");
        case ModificationType::Undefined:
          return QStringLiteral("");
      }
      return QStringLiteral("");
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return (pvType == ModificationType::Undefined);
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvType = ModificationType::Undefined;
    }

    /*! \brief Get a PK from code
     */
    static ModificationPk fromCode(const QString & code)
    {
      QString c = code;

      if(c == "NEW"){
        return ModificationPk(ModificationType::New);
      }else if(c == "REM"){
        return ModificationPk(ModificationType::Rem);
      }else if(c == "MODNEW"){
        return ModificationPk(ModificationType::ModNew);
      }else if(c == "MODREM"){
        return ModificationPk(ModificationType::ModRem);
      }else if(c == "EXISTS"){
        return ModificationPk(ModificationType::Exists);
      }else{
        return ModificationPk(ModificationType::Undefined);
      }
    }

    /*! \brief Get a PK from QVariant
     *
     * if code is null, a null PK is returned
     *
     * \pre If code is not null, it must be a string
     */
    static ModificationPk fromQVariant(const QVariant & code)
    {
      if(code.isNull()){
        return ModificationPk();
      }
      Q_ASSERT(code.canConvert<QString>());
      return fromCode(code.toString());
    }

   private:

    ModificationType pvType;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_MODIFICATION_PK_H
