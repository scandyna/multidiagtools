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
#ifndef MDT_DEPLOY_UTILS_RPATH_INFO_LIST_H
#define MDT_DEPLOY_UTILS_RPATH_INFO_LIST_H

#include "RPathInfo.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Hold a list of RPathInfo
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT RPathInfoList
  {
   public:

    /*! \brief Set a relative path
     *
     * \note Will replace existing paths with \a path
     * \pre \a path must not contains keywords, like $ORIGIN
     *               or the RPATH separator ':' .
     *               Expressing current directory with '.'
     *               or parent directory with '..' is still allowed.
     */
    void setRelativePath(const QString & path);

    /*! \brief Add a path to the end
     *
     * If \a path starts with '/' ,
     *  it is considered absolute, else relative.
     *
     * If \a path allready exists, it will not be added again
     *
     * \pre \a path must not contains keywords, like $ORIGIN
     *               or the RPATH separator ':' .
     *               Expressing current directory with '.'
     *               or parent directory with '..' is still allowed.
     * \sa fromRawRPath()
     */
    void appendPath(const QString & path);

    /*! \brief Add a path to the beginning
     *
     * If \a path starts with / ,
     *  it is considered absolute, else relative.
     *
     * If \a path allready exists, it will not be added again
     *
     * \pre \a path must not contains keywords, like $ORIGIN
     *               or the RPATH separator ':' .
     *               Expressing current directory with '.'
     *               or parent directory with '..' is still allowed.
     * \sa fromRawRPath(4)
     */
    void prpendPath(const QString & path);

    /*! \brief Add a relative path to the beginning
     *
     * If \a path allready exists, it will not be added again
     *
     * \pre \a path must not contains keywords, like $ORIGIN
     *               or the RPATH separator ':' .
     *               Expressing current directory with '.'
     *               or parent directory with '..' is still allowed.
     * \pre \a path must be relative
     */
    void prependRelativePath(const QString & path);

    /*! \brief Clear
     */
    void clear();

    /*! \brief Check if this RPATH is empty
     */
    bool isEmpty() const
    {
      return mRPath.empty();
    }

    /*! \brief Get count of path in this RPath
     */
    int count() const
    {
      return mRPath.size();
    }

    /*! \brief Get path at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < count() ).
     */
    const RPathInfo & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mRPath[index];
    }

    /*! \brief Get a string representing this RPATH
     *
     * For each path that is relative,
     *  the string will contains $ORIGIN followed by the path.
     */
    QString toStringLinux() const;

    /*! \brief Get a RPath info from a raw RPATH string
     */
    static RPathInfoList fromRawRPath(const QString & rpathString);

   private:

    static RPathInfo buildRPathInfo(const QString & path);
    static RPathInfo buildRPathInfoFromRawPath(const QString & path);
    bool containsPath(const RPathInfo & rpath);
    std::vector<RPathInfo> mRPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_INFO_LIST_H
