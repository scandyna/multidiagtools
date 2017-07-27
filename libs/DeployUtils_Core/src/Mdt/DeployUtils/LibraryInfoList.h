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
#include <QVector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Container that holds a list of LibraryInfo
   */
  class LibraryInfoList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QVector<LibraryInfo>::const_iterator;

    /*! \brief Add a library info to the end of this list
     */
    void addLibrary(const LibraryInfo & library);

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

   private:

    QVector<LibraryInfo> mList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_LIST_H
