/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_GRAPH_EDGE_DATA_H
#define MDT_GRAPH_EDGE_DATA_H

#include <QVariant>
#include <QString>
#include <QMap>

/*! \brief Provide data storage for mdtGraph edge
 *
 * mdtGraph uses this class to store edge data in a flexible
 *  and simple to use way.
 *
 * Use setData() to set/add data and data() to get them.
 *  Internally, data are stored in a QMap,
 *   with field as key and data as value.
 * setData() and data() provides a default field (empty string),
 *  so that it can be used simplier for single data storage.
 */
class mdtGraphEdgeData
{
 public:

  /*! \brief Constructor
   */
  mdtGraphEdgeData();

  /*! \brief Destructor
   */
  ~mdtGraphEdgeData();

  /*! \brief Set data
   */
  void setData(const QVariant &data, const QString &field = "");

  /*! \brief Get data
   *
   * If field does not exists, a invalid QVariant is returned.
   */
  QVariant data(const QString &field = "");

 private:

  QMap<QString, QVariant> pvData;
};

#endif  // #ifndef MDT_GRAPH_EDGE_DATA_H
