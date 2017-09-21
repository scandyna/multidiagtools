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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_LIST_H
#define MDT_DEPLOY_UTILS_LIBRARY_INFO_LIST_H

#include "LibraryInfo.h"
#include <QString>
#include <QVector>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Container that holds a list of LibraryInfo
   */
  class LibraryInfoList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QVector<LibraryInfo>::const_iterator;

    /*! \brief STL value type
     */
    using value_type = QVector<LibraryInfo>::value_type;

    /*! \brief Construct a empty library info list
     */
    LibraryInfoList() = default;

    /*! \brief Construct a library info list from initializer lists
     */
    LibraryInfoList(std::initializer_list<LibraryInfo> list);

    /*! \brief Copy construct a library info list from a other
     */
    LibraryInfoList(const LibraryInfoList &) = default;

    /*! \brief Copy assign a library info list this this one
     */
    LibraryInfoList & operator=(const LibraryInfoList &) = default;

    /*! \brief Move construct a library info list from a other
     */
    LibraryInfoList(LibraryInfoList &&) = default;

    /*! \brief Move assign a library info list this this one
     */
    LibraryInfoList & operator=(LibraryInfoList &&) = default;

    /*! \brief Add a library info to the end of this list
     *
     * If \a library allready exists,
     *  it will not be added.
     */
    void addLibrary(const LibraryInfo & library);

    /*! \brief Add a library info to the end of this list
     *
     * This method is used for STL compatibility
     *  and is the same as addLibrary()
     */
    void push_back(const LibraryInfo & library);

    /*! \brief Add a list of libraries to this list
     *
     * Will add each libraries from \a libraries
     *  that not actually exists in this list.
     */
    void addLibraries(const LibraryInfoList & libraries);

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

    /*! \brief Check if this list contains library
     */
    bool containsLibraryAbsoluteFilePath(const QString & filePath) const;

    /*! \brief Get library info list at index
     *
     * \pre \a index must be in a valid range ( 0 <= index < count() )
     */
    const LibraryInfo & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList.at(index);
    }

    /*! \brief Clear this list
     */
    void clear();

    /*! \brief Attempts to allocate memory for at least size elements
     */
    void reserve(int size);

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

   private:

    QVector<LibraryInfo> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::LibraryInfoList)

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_LIST_H
