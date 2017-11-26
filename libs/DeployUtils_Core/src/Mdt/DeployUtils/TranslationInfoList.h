/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_TRANSLATION_INFO_LIST_H
#define MDT_DEPLOY_UTILS_TRANSLATION_INFO_LIST_H

#include "TranslationInfo.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>
#include <QVector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Container that contains a list of TranslationInfo
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT TranslationInfoList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QVector<TranslationInfo>::const_iterator;

    /*! \brief STL value type
     */
    using value_type = QVector<TranslationInfo>::value_type;

    /*! \brief Construct a empty translation info list
     */
    TranslationInfoList() = default;

    /*! \brief Copy construct a translation info list from a other
     */
    TranslationInfoList(const TranslationInfoList &) = default;

    /*! \brief Copy assign a translation info list this this one
     */
    TranslationInfoList & operator=(const TranslationInfoList &) = default;

    /*! \brief Move construct a translation info list from a other
     */
    TranslationInfoList(TranslationInfoList &&) = default;

    /*! \brief Move assign a translation info list this this one
     */
    TranslationInfoList & operator=(TranslationInfoList &&) = default;

    /*! \brief Add a translation info to the end of this list
     *
     * If \a translation allready exists,
     *  it will not be added.
     */
    void addTranslation(const TranslationInfo & translation);

    /*! \brief Add a list of qt translations to this list
     *
     * Will add each translation from \a translations
     *  that not actually exists in this list.
     */
    void addTranslations(const TranslationInfoList & translations);

    /*! \brief Get count of items in this list
     */
    int count() const
    {
      return mList.count();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.isEmpty();
    }

    /*! \brief Get translation at \a index
     *
     * \pre \a index must be in valid range ( 0 <= index < count() ).
     */
    const TranslationInfo & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList.at(index);
    }

    /*! \brief Get used file suffixes in this list
     *
     * \sa TranslationInfo::fileSuffix()
     */
    QStringList getUsedFileSuffixes() const;

    /*! \brief Get translations for a of file suffixe
     *
     * \a fileSuffix refers to TranslationInfo::fileSuffix()
     *
     * \pre \a fileSuffix must not be empty.
     */
    TranslationInfoList getTranslationsForFileSuffix(const QString & fileSuffix) const;

    /*! \brief Get translations for a list of file suffixes
     *
     * \a fileSuffixes refers to TranslationInfo::fileSuffix()
     *
     * \pre \a fileSuffixes must not be empty.
     * \pre no suffix must be empty in \a fileSuffixes .
     */
    TranslationInfoList getTranslationsForFileSuffixes(const QStringList & fileSuffixes) const;

    /*! \brief Returns an STL-style const iterator pointing to the first item in this list
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns an STL-style iterator pointing to the imaginary item after the last item in this list
     */
    const_iterator cend() const
    {
      return mList.cend();
    }

    /*! \brief Returns an STL-style const iterator pointing to the first item in this list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns an STL-style iterator pointing to the imaginary item after the last item in this list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

    /*! \brief Get a list of file path for this list
     */
    QStringList toQmFilePathList() const;

    /*! \brief Create a translation info list from a list of files
     *
     * \pre For each path in \a qmFilePathList ,
     *       the file must have .qm extension.
     *       (it is not checked if the file exists).
     */
    static TranslationInfoList fromQmFilePathList(const QStringList & qmFilePathList);

   private:

    QVector<TranslationInfo> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_TRANSLATION_INFO_LIST_H
