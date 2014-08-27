/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_MODEL_ITEM_DATA_H
#define MDT_TT_TEST_MODEL_ITEM_DATA_H

#include "mdtSqlRecord.h"
#include "mdtTtTestModelItemRouteData.h"
#include <QSqlDatabase>
#include <QList>

/*! \brief Container for test model item data
 */
class mdtTtTestModelItemData : public mdtSqlRecord
{
 public:

  /*! \brief Construct a empty mdtTtTestModelItemData
   */
  mdtTtTestModelItemData();

  /*! \brief Setup
   *
   * Will add all fields from TestModelItem_tbl
   */
  bool setup(const QSqlDatabase & db);

  /*! \brief Clear
   *
   * Remove data and fields
   */
  void clear();

  /*! \brief Clear values
   *
   * Remove data
   */
  void clearValues();

  /*! \brief Set Id_PK
   *
   * Will also update stored route data list
   */
  void setId(const QVariant & id);

  /*! \brief Add route data
   *
   * Will update testModelItemId in data to internal Id_PK
   */
  void addRouteData(mdtTtTestModelItemRouteData data);

  /*! \brief Get route data list
   */
  inline const QList<mdtTtTestModelItemRouteData> & routeDataList() const { return pvRouteDataList; }

  /*! \brief Get route data list
   */
  inline QList<mdtTtTestModelItemRouteData> & routeDataList() { return pvRouteDataList; }

 private:

  QList<mdtTtTestModelItemRouteData> pvRouteDataList;
};

#endif // #ifndef MDT_TT_TEST_MODEL_ITEM_DATA_H
