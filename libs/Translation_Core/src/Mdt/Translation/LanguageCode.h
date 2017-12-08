/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_TRANSLATION_LANGUAGE_CODE_H
#define MDT_TRANSLATION_LANGUAGE_CODE_H

#include "MdtTranslation_CoreExport.h"
#include <array>

class QString;

namespace Mdt{ namespace Translation{

  /*! \brief LanguageCode is a 2 letter ISO 639-1 laguage representation
   */
  class MDT_TRANSLATION_CORE_EXPORT LanguageCode
  {
   public:

    /*! \brief Construct a null language code
     */
    explicit LanguageCode() noexcept;

    /*! \brief Construct a language code
     *
     * \pre \a code must contain exactly 2 lower case letters
     */
    explicit LanguageCode(const char * const code);

    /*! \brief Copy construct a language code from \a other
     */
    LanguageCode(const LanguageCode & other) = default;

    /*! \brief Copy assign \a other language code to this
     */
    LanguageCode & operator=(const LanguageCode & other) = default;

    /*! \brief Move construct a language code from \a other
     */
    LanguageCode(LanguageCode && other) = default;

    /*! \brief Move assign \a other language code to this
     */
    LanguageCode & operator=(LanguageCode && other) = default;

    /*! \brief Check if this language code is null
     */
    bool isNull() const noexcept
    {
      return (mCode[0] == '\0');
    }

    /*! \brief Check if language code \a a is equal to \a b
     */
    friend
    bool operator==(const LanguageCode & a, const LanguageCode & b) noexcept
    {
      return (a.mCode == b.mCode);
    }

    /*! \brief Check if language code \a a is not equal to \a b
     */
    friend
    bool operator!=(const LanguageCode & a, const LanguageCode & b) noexcept
    {
      return !(a == b);
    }

    /*! \brief Get a string version of this language code
     */
    QString toString() const;

    /*! \brief Construct a language code from a string
     *
     * \pre \a code must contain exactly 2 letters
     */
    static LanguageCode fromString(const QString & code);

    /*! \brief Construct a language code from language_COUNTRY string
     *
     * \code
     * const auto code = LanguageCode::fromLanguageCountry("fr_CH");
     * // code will contain "fr"
     * \endcode
     *
     * \pre \a languageCountry must contain at least 2 letters
     */
    static LanguageCode fromLanguageCountry(const QString & languageCountry);

   private:

    std::array<char, 2> mCode;
  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_LANGUAGE_CODE_H
