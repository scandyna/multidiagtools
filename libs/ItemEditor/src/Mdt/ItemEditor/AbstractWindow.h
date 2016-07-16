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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H
#define MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H

#include "NavigationActions.h"
#include "InsertAction.h"
#include <QMainWindow>

namespace Mdt{ namespace ItemEditor{

  class AbstractEditorWidget;

  /*! \brief AbstractWindow is the base class recommanded to create Window based editors
   *
   * Based on QMainWindow, it displays a menu, toolbar and a status bar.
   *
   * A example how to subclass AbstractWindow can be found in StandardWindow.h and StandardWindow.cpp .
   */
  class AbstractWindow : public QMainWindow
  {
   Q_OBJECT

   public:

    /*! \brief Construct a empty window
     */
    AbstractWindow(QWidget *parent = nullptr);

    /*! \brief Set main editor widget
     *
     * This helper method will do needed signals/slots connections
     *  between the editor widget and actions.
     *
     * \note widget will not be placed anywhere, use setCentralWidget() fir this.
     */
    void setMainEditorWidget(AbstractEditorWidget *widget);

   protected:

    /*! \brief Get NavigationActions
     *
     * The first time this method is called,
     *  the NavigationActions is created.
     */
    NavigationActions *navigationActions();

    /*! \brief Get InsertAction
     *
     * The first time this method is called,
     *  the InsertAction is created.
     */
    InsertAction *insertAction();

   private:

    NavigationActions *pvNavigationActions;
    InsertAction *pvInsertAction;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H
