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
#ifndef MDT_ITEM_MODEL_EXPRESSION_CONTAINER_INTERFACE_H
#define MDT_ITEM_MODEL_EXPRESSION_CONTAINER_INTERFACE_H

#include "ParentModelEvalData.h"
#include "../RelationKey.h"
#include <Qt>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Interface for container implementing a expression acting on a item model
   */
  class ContainerInterface
  {
   public:

    // Constructors / destructors
    ContainerInterface() = default;
    virtual ~ContainerInterface() = default;
    // Copy disabled
    ContainerInterface(const ContainerInterface &) = delete;
    ContainerInterface & operator=(const ContainerInterface &) = delete;
    // Move disabled
    ContainerInterface(ContainerInterface &&) = delete;
    ContainerInterface & operator=(ContainerInterface &&) = delete;

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre \a model must be a valid pointer (not null)
     * \pre \a must be in valid range ( 0 <= row < model->rowCount() )
     */
    virtual bool eval(const QAbstractItemModel * const model, int row, Qt::CaseSensitivity caseSensitivity) const = 0;

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre \a model must be a valid pointer (not null)
     * \pre \a must be in valid range ( 0 <= row < model->rowCount() )
     * \pre \a parentModelData must not be null
     */
    virtual bool eval(const QAbstractItemModel * const model, int row, const ParentModelEvalData & parentModelData , Qt::CaseSensitivity caseSensitivity) const = 0;

    /*! \brief Get a relation key that contains pais of equaly compared columns of this expression
     */
    virtual RelationKey getRelationKeyForEquality() const = 0;

    /*! \brief Get the greatest column in this expression
     */
    virtual int getGreatestColumn() const = 0;

    /*! \brief Get the greatest parent model column in this expression
     */
    virtual int getGreatestParentModelColumn() const = 0;

  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_CONTAINER_INTERFACE_H
