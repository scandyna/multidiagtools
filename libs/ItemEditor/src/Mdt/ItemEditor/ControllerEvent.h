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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_EVENT_H
#define MDT_ITEM_EDITOR_CONTROLLER_EVENT_H

#include <QMetaType>

class QByteArray;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Event for ControllerStateMachine
   */
  enum class ControllerEvent
  {
    DataEditionStarted,       /*!< Data edition started event */
    DataEditionDone,          /*!< Data edition done event */
    SubmitDone,               /*!< Submit done event */
    RevertDone,               /*!< Revert done event */
    InsertStarted,            /*!< Instertion started event */
    RemoveDone,               /*!< Remove done event */
    EditionStartedFromParent, /*!< Parent controller stated editing (or inserting, or ...) */
    EditionDoneFromParent,    /*!< Parent controller finished editing */
    EditionStartedFromChild,  /*!< Child controller stated editing (or inserting, or ...) */
    EditionDoneFromChild      /*!< Child controller finished editing */
  };

  /*! \brief Get text version of \a event
   *
   * \sa operator<<(QDebug, Mdt::ItemEditor::ControllerEvent)
   */
  QByteArray controllerEventText(ControllerEvent event);

}} // namespace Mdt{ namespace ItemEditor{

Q_DECLARE_METATYPE(Mdt::ItemEditor::ControllerEvent)

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_EVENT_H
