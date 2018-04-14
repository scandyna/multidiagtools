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
#ifndef MDT_SQL_SCHEMA_LOCALE_H
#define MDT_SQL_SCHEMA_LOCALE_H

#include <QLocale>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL Schema locale
   */
  class Locale
  {
   public:

    /*! \brief Contruct a null locale
     */
    constexpr Locale() noexcept
     : pvWasEdited(false),
       pvCountry(QLocale::AnyCountry),
       pvLanguage(QLocale::AnyLanguage)
    {
    }

    /*! \brief Set country
     */
    constexpr void setCountry(QLocale::Country country) noexcept
    {
      pvCountry = country;
      pvWasEdited = true;
    }

    /*! \brief Get country
     */
    constexpr QLocale::Country country() const noexcept
    {
      return pvCountry;
    }

    /*! \brief Set language
     */
    constexpr void setLanguage(QLocale::Language language) noexcept
    {
      pvLanguage = language;
      pvWasEdited = true;
    }

    /*! \brief Get language
     */
    constexpr QLocale::Language language() const noexcept
    {
      return pvLanguage;
    }

    /*! \brief Check if null
     *
     * locale is null after default construction,
     *  or a call of clear(),
     *  until a attribute is set.
     */
    constexpr bool isNull() const noexcept
    {
      return !pvWasEdited;
    }

    /*! \brief Clear
     */
    constexpr void clear() noexcept
    {
      pvCountry = QLocale::AnyCountry;
      pvLanguage = QLocale::AnyLanguage;
      pvWasEdited = false;
    }

   private:

    bool pvWasEdited;
    QLocale::Country pvCountry;
    QLocale::Language pvLanguage;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_LOCALE_H
