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
#ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H
#define MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H

#include "ExpressionMatchItemModel.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Model to access match items of related table insert expression
   */
  class RelatedTableInsertMatchItemModel : public ExpressionMatchItemModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    RelatedTableInsertMatchItemModel(const std::shared_ptr<const TableMapping> & tm, QObject *parent = nullptr);

    // Copy is disabled
    RelatedTableInsertMatchItemModel(const RelatedTableInsertMatchItemModel &) = delete;
    RelatedTableInsertMatchItemModel & operator=(const RelatedTableInsertMatchItemModel &) = delete;

  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_MATCH_ITEM_MODEL_H
