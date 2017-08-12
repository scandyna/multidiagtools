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
#ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_VERTEX_H
#define MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_VERTEX_H

#include <QString>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

  struct VertexData
  {
    VertexData() = default;

    VertexData(const QString & name)
     : name(name)
    {
    }

    VertexData(const VertexData &) = default;
    VertexData & operator=(const VertexData &) = default;

    VertexData(VertexData &&) = default;
    VertexData & operator=(VertexData &&) = default;

    QString name;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_VERTEX_H
