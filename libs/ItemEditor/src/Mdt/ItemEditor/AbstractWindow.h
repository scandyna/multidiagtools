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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H
#define MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H

#include "NavigationActions.h"
#include "EditionActions.h"
#include "InsertAction.h"
#include "RemoveAction.h"
#include <QMainWindow>
#include <QPointer>

namespace Mdt{ namespace ItemEditor{

  class AbstractEditorWidget;
  class AbstractController;

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
//     void setMainEditorWidget(AbstractEditorWidget *widget);

    /*! \brief Set main controller
     *
     * This helper method will do needed signals/slots connections
     *  between \a controller and actions.
     *
     * \pre controller must be a valid pointer
     */
    void setMainController(AbstractController *controller);

   protected:

    /*! \brief Get NavigationActions
     *
     * The first time this method is called,
     *  the NavigationActions is created.
     */
    NavigationActions *navigationActions();

    /*! \brief Get EditionActions
     *
     * The first time this method is called,
     *  the EditionActions is created.
     */
    EditionActions *editionActions();

    /*! \brief Get InsertAction
     *
     * The first time this method is called,
     *  the InsertAction is created.
     */
    InsertAction *insertAction();

    /*! \brief Get RemoveAction
     *
     * The first time this method is called,
     *  the RemoveAction is created.
     */
    RemoveAction *removeAction();

   private:

    NavigationActions *mNavigationActions;
    InsertAction *mInsertAction;
    EditionActions *mEditionActions;
    RemoveAction *mRemoveAction;
    QPointer<AbstractController> mController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_WINDOW_H
