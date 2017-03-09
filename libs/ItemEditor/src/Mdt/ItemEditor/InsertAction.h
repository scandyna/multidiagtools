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
#ifndef MDT_ITEM_EDITOR_INSERT_ACTION_H
#define MDT_ITEM_EDITOR_INSERT_ACTION_H

#include "AbstractActionContainer.h"
#include <QAction>

namespace Mdt{ namespace ItemEditor{

  /*! \brief InsertAction is a action to insert data in a item editor
   *
   * Like recommended in Qt documentation (see QObject),
   *  InsertAction objects should be created on the heap,
   *  and a parent passed in its constructor.
   *
   * For a example on how InsertAction could be used,
   *  see StandardWindow.h and StandardWindow.cpp .
   */
  class InsertAction : public AbstractActionContainer
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit InsertAction(QObject* parent);

    // Copy disabled
    InsertAction(const InsertAction &) = delete;
    InsertAction & operator=(const InsertAction &) = delete;
    // Move disabled
    InsertAction(InsertAction &&) = delete;
    InsertAction & operator=(InsertAction &&) = delete;

    /*! \brief Get insert action
     */
    QAction *insertAction() const
    {
      return mAction;
    }

   signals:

    /*! \brief Emitted when insert action was triggered
     */
    void insertTriggered();

   private:

    void updateEnableState() override;
    void disableAllActions() override;

    QAction *mAction;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef
