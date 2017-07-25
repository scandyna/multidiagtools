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
#ifndef MDT_DEPLOY_UTILS_PATH_LIST_H
#define MDT_DEPLOY_UTILS_PATH_LIST_H

#include <QString>
#include <QStringList>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief PathList contains a list of paths
   */
  class PathList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = QStringList::const_iterator;

    /*! \brief Construct a empty path list
     */
    PathList() = default;

    /*! \brief Construct a path list from \a list
     */
    PathList(std::initializer_list<QString> list)
     : mList(list)
    {
    }

    /*! \brief Copy construct a path list from a other
     */
    PathList(const PathList &) = default;

    /*! \brief Assign a path list to this one by copy
     */
    PathList & operator=(const PathList &) = default;

    /*! \brief Move construct a path list from a other
     */
    PathList(PathList &&) = default;

    /*! \brief Assign a path list to this one by move
     */
    PathList & operator=(PathList &&) = default;

    /*! \brief Add a path to the end of this list
     *
     * If \a path allready exists in this list,
     *  it will be moved to the end.
     *
     * \pre \a path must not be a empty string
     */
    void appendPath(const QString & path);

    /*! \brief Add a path to the end of this list
     *
     * If \a path allready exists in this list,
     *  it will be moved to the beginning.
     *
     * \pre \a path must not be a empty string
     */
    void prependPath(const QString & path);

    /*! \brief Get a list of strings for this list
     */
    QStringList toStringList() const
    {
      return mList;
    }

    /*! \brief Returns a const STL-style iterator pointing to the first item in the list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns a const STL-style iterator pointing to the first item in the list
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Returns a const STL-style iterator pointing to the last item in the list
     */
    const_iterator end() const
    {
      return mList.end();
    }

    /*! \brief Returns a const STL-style iterator pointing to the last item in the list
     */
    const_iterator cend() const
    {
      return mList.end();
    }

   private:

    QStringList mList;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::PathList)

#endif // #ifndef MDT_DEPLOY_UTILS_PATH_LIST_H
