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
#ifndef MDT_ITEM_EDITOR_NAVIGATION_ACTIONS_H
#define MDT_ITEM_EDITOR_NAVIGATION_ACTIONS_H

#include "RowState.h"
#include <QAction>

namespace Mdt{ namespace ItemEditor{

  /*! \brief NavigationActions is a collection of actions to navigate trough data in a item editor
   *
   * Like recommended in Qt documentation (see QObject),
   *  NavigationActions objects should be created on the heap,
   *  and a parent passed in its constructor.
   *
   * For a example on how NavigationActions could be used,
   *  see StandardWindow.h and StandardWindow.cpp .
   */
  class NavigationActions : public QObject
  {
   Q_OBJECT

   public:
 
    /*! \brief Constructor
     */
    explicit NavigationActions(QObject* parent);

    // Copy disabled
    NavigationActions(const NavigationActions &) = delete;
    NavigationActions & operator=(const NavigationActions &) = delete;
    // Move disabled
    NavigationActions(NavigationActions &&) = delete;
    NavigationActions & operator=(NavigationActions &&) = delete;

    /*! \brief Get toFirst action
     */
    QAction *toFirst()
    {
      return pvToFirst;
    }

    /*! \brief Get toPrevious action
     */
    QAction *toPrevious()
    {
      return pvToPrevious;
    }

    /*! \brief Get toNext action
     */
    QAction *toNext()
    {
      return pvToNext;
    }

    /*! \brief Get toLast action
     */
    QAction *toLast()
    {
      return pvToLast;
    }

   public slots:

    /*! \brief Set row state
     */
    void setRowState(RowState rs);

   signals:

    /*! \brief Emitted when ToFirst action was triggered
     */
    void toFirstTriggered();

    /*! \brief Emitted when ToPrevious action was triggered
     */
    void toPreviousTriggered();

    /*! \brief Emitted when ToNext action was triggered
     */
    void toNextTriggered();

    /*! \brief Emitted when ToLast action was triggered
     */
    void toLastTriggered();

   private:

    /*! \brief Update actions enable states
     */
    void updateEnableStates();

    RowState pvRowState;
    QAction *pvToFirst;
    QAction *pvToPrevious;
    QAction *pvToNext;
    QAction *pvToLast;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_NAVIGATION_ACTIONS_H
