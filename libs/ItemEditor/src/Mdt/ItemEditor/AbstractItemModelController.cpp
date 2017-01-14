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
#include "AbstractItemModelController.h"

namespace ItemModel = Mdt::ItemModel;
using ItemModel::PrimaryKey;
using ItemModel::ForeignKey;

namespace Mdt{ namespace ItemEditor{

AbstractItemModelController::AbstractItemModelController(QObject* parent)
 : AbstractController(parent)
{
}

void AbstractItemModelController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
  registerModel(model);
}

void AbstractItemModelController::setPrimaryKey(const ItemModel::PrimaryKey & pk)
{
  AbstractController::setPrimaryKey(pk);
}

void AbstractItemModelController::setPrimaryKey(std::initializer_list<int> list)
{
  setPrimaryKey(PrimaryKey(list));
}

void AbstractItemModelController::setForeignKey(const ItemModel::ForeignKey & fk)
{
  AbstractController::setForeignKey(fk);
}

void AbstractItemModelController::setForeignKey(std::initializer_list<int> list)
{
  setForeignKey(ForeignKey(list));
}

}} // namespace Mdt{ namespace ItemEditor{
