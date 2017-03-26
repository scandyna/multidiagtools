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
#ifndef MDT_ITEM_EDITOR_DEBUG_H
#define MDT_ITEM_EDITOR_DEBUG_H

#include "ControllerState.h"
#include "ControllerEvent.h"
#include <QDebug>

namespace Mdt{ namespace ItemEditor{

}} // namespace Mdt{ namespace ItemEditor{

/*! \brief Make Mdt::ItemEditor::ControllerState usable with qDebug()
 */
QDebug operator<<(QDebug debug, Mdt::ItemEditor::ControllerState state);

/*! \brief Make Mdt::ItemEditor::ControllerEvent usable with qDebug()
 */
QDebug operator<<(QDebug debug, Mdt::ItemEditor::ControllerEvent event);

#endif // #ifndef MDT_ITEM_EDITOR_DEBUG_H
