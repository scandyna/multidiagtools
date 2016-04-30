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
#ifndef MDT_CABLE_LIST_PATH_GRAPH_H
#define MDT_CABLE_LIST_PATH_GRAPH_H

#include <QSqlDatabase>
#include <memory>

namespace Mdt{ namespace CableList{ namespace Path{

  namespace GraphPrivate{
    struct GraphImpl;
  }

  /*! \brief Graph of a list of links
   */
  class Graph
  {
   public:

    /*! \brief Constructor
     */
    Graph(const QSqlDatabase & db);

    // Destructor: needed by unique_ptr to destruct a fully known object
    ~Graph();

   private:

    std::unique_ptr<GraphPrivate::GraphImpl> pvGraph;
    QSqlDatabase pvDatabase;
  };

}}} // namespace Mdt{ namespace CableList{ namespace Path{

#endif // #ifndef MDT_CABLE_LIST_PATH_GRAPH_H
