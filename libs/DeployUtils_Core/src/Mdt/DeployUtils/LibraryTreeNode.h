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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_TREE_NODE_H
#define MDT_DEPLOY_UTILS_LIBRARY_TREE_NODE_H

#include "MdtDeployUtils_CoreExport.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Node identifier used in LibraryTree
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT LibraryTreeNode
  {
   public:

    /*! \brief Construct a null tree node
     */
    constexpr LibraryTreeNode() noexcept = default;

    /*! \brief Construct a tree node
     */
    constexpr LibraryTreeNode(int id) noexcept
     : mId(id)
    {
    }

    /*! \brief Get node id
     */
    constexpr int id() const noexcept
    {
      return mId;
    }

    /*! \brief Check if this tree node is null
     *
     * A tree noode is null if its id is 0
     */
    constexpr bool isNull() const noexcept
    {
      return ( mId == 0 );
    }

   private:

    int mId = 0;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_TREE_NODE_H
