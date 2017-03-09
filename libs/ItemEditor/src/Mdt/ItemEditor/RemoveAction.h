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
#ifndef MDT_ITEM_EDITOR_REMOVE_ACTION_H
#define MDT_ITEM_EDITOR_REMOVE_ACTION_H

#include "AbstractActionContainer.h"
#include <QAction>

namespace Mdt{ namespace ItemEditor{

  /*! \brief RemoveAction is a collection of actions to edit data in a item editor
   *
   * Like recommended in Qt documentation (see QObject),
   *  RemoveAction objects should be created on the heap,
   *  and a parent passed in its constructor.
   *
   * For a example on how RemoveAction could be used,
   *  see StandardWindow.h and StandardWindow.cpp .
   */
  class RemoveAction : public AbstractActionContainer
  {
   Q_OBJECT

   public:
 
    /*! \brief Constructor
     */
    explicit RemoveAction(QObject* parent);

    // Copy disabled
    RemoveAction(const RemoveAction &) = delete;
    RemoveAction & operator=(const RemoveAction &) = delete;
    // Move disabled
    RemoveAction(RemoveAction &&) = delete;
    RemoveAction & operator=(RemoveAction &&) = delete;

    /*! \brief Get remove action
     */
    QAction *removeAction() const
    {
      return mRemoveAction;
    }

   signals:

    /*! \brief Emitted when remove action was triggered
     */
    void removeTriggered();

   private:

    /*! \brief Update actions enable states
     */
    void updateEnableState() override;

    /*! \brief Disable all actions
     */
    void disableAllActions() override;

    QAction *mRemoveAction;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_REMOVE_ACTION_H
