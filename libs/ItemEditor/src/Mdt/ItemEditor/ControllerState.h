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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_H

#include <QMetaType>

class QByteArray;

namespace Mdt{ namespace ItemEditor{

  /*! \brief State of item editor controller
   */
  enum class ControllerState
  {
    Visualizing,  /*!< Visualizing state */
    Editing,      /*!< Editing state */
    Inserting     /*!< Inserting state */
  };

  /// \todo Add ModelNotSet (or NoModelSet) state ?

  /*! \brief Get text version of \a state
   *
   * \sa operator<<(QDebug, Mdt::ItemEditor::ControllerState)
   */
  QByteArray controllerStateText(ControllerState state);

}} // namespace Mdt{ namespace ItemEditor{

Q_DECLARE_METATYPE(Mdt::ItemEditor::ControllerState)

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_H
