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
#ifndef MDT_TRANSLATION_LANGUAGE_LIST_CODE_H
#define MDT_TRANSLATION_LANGUAGE_LIST_CODE_H

#include "LanguageCode.h"
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace Translation{

  /*! \brief LanguageCodeList holds a list of LanguageCode
   */
  class LanguageCodeList
  {
   public:

    /*! \brief Value type
     */
    using value_type = std::vector<LanguageCode>::value_type;

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<LanguageCode>::const_iterator;

    /*! \brief Construct a empty list of language codes
     */
    LanguageCodeList() = default;

    /*! \brief Copy construct a language code list from \a other
     */
    LanguageCodeList(const LanguageCodeList & other) = default;

    /*! \brief Copy assign \a other language code list to this
     */
    LanguageCodeList & operator=(const LanguageCodeList & other) = default;

    /*! \brief Move construct a language code list from \a other
     */
    LanguageCodeList(LanguageCodeList && other) = default;

    /*! \brief Move assign \a other language code list to this
     */
    LanguageCodeList & operator=(LanguageCodeList && other) = default;

    /*! \brief Add a language code to this list
     *
     * If \a languageCode allready exists in this list,
     *  it will not be added again.
     */
    void addLanguageCode(const LanguageCode & languageCode);

    /*! \brief Get count of language codes in this list
     */
    int count() const noexcept
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief Access laguage code at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < count() )
     */
    const LanguageCode & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList[index];
    }

    /*! \brief Check if this list contains \a languageCode
     */
    bool containsLanguageCode(const LanguageCode & languageCode) const noexcept;

   private:

    std::vector<LanguageCode> mList;
  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_LANGUAGE_LIST_CODE_H
