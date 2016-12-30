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
#ifndef MDT_ITEM_EDITOR_EDITION_ACTIONS_H
#define MDT_ITEM_EDITOR_EDITION_ACTIONS_H

#include "AbstractActionContainer.h"
#include <QAction>

namespace Mdt{ namespace ItemEditor{

  /*! \brief EditionActions is a collection of actions to edit data in a item editor
   *
   * Like recommended in Qt documentation (see QObject),
   *  EditionActions objects should be created on the heap,
   *  and a parent passed in its constructor.
   *
   * For a example on how EditionActions could be used,
   *  see StandardWindow.h and StandardWindow.cpp .
   */
  class EditionActions : public AbstractActionContainer
  {
   Q_OBJECT

   public:
 
    /*! \brief Constructor
     */
    explicit EditionActions(QObject* parent);

    // Copy disabled
    EditionActions(const EditionActions &) = delete;
    EditionActions & operator=(const EditionActions &) = delete;
    // Move disabled
    EditionActions(EditionActions &&) = delete;
    EditionActions & operator=(EditionActions &&) = delete;

    /*! \brief Get submit action
     */
    QAction *submitAction() const
    {
      return mSubmitAction;
    }

    /*! \brief Get revert action
     */
    QAction *revertAction() const
    {
      return mRevertAction;
    }

   signals:

    /*! \brief Emitted when submit action was triggered
     */
    void submitTriggered();

    /*! \brief Emitted when revert action was triggered
     */
    void revertTriggered();

   private:

    /*! \brief Update actions enable states
     */
    void updateEnableState() override;

    /*! \brief Disable all actions
     */
    void disableAllActions() override;

    QAction *mSubmitAction;
    QAction *mRevertAction;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_EDITION_ACTIONS_H
