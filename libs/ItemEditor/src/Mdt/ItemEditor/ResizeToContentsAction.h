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
#ifndef MDT_ITEM_EDITOR_RESIZE_TO_CONTENTS_ACTION_H
#define MDT_ITEM_EDITOR_RESIZE_TO_CONTENTS_ACTION_H

#include "AbstractActionContainer.h"
#include <QAction>

namespace Mdt{ namespace ItemEditor{

  /*! \brief ResizeToContentsAction is a action to resize a view to its contents
   *
   * Like recommended in Qt documentation (see QObject),
   *  ResizeToContentsAction objects should be created on the heap,
   *  and a parent passed in its constructor.
   *
   * For a example on how ResizeToContentsAction could be used,
   *  see AbstractTableViewWidget.h and AbstractTableViewWidget.cpp .
   */
  class ResizeToContentsAction : public AbstractActionContainer
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit ResizeToContentsAction(QObject* parent);

    // Copy disabled
    ResizeToContentsAction(const ResizeToContentsAction &) = delete;
    ResizeToContentsAction & operator=(const ResizeToContentsAction &) = delete;
    // Move disabled
    ResizeToContentsAction(ResizeToContentsAction &&) = delete;
    ResizeToContentsAction & operator=(ResizeToContentsAction &&) = delete;

    /*! \brief Get the action
     */
    QAction *resizeToContentsAction() const
    {
      return mResizeToContentsAction;
    }

   signals:

    /*! \brief Emitted when resiteToContents action was triggered
     */
    void resizeToContentsTriggered();

   private:

    /*! \brief Update actions enable states
     */
    void updateEnableState() override;

    /*! \brief Disable all actions
     */
    void disableAllActions() override;

    QAction *mResizeToContentsAction;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_RESIZE_TO_CONTENTS_ACTION_H
