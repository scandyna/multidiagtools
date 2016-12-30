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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H
#define MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H

///#include "AbstractEditorWidget.h"
#include <QWidget>
#include <QPointer>

class QTableView;
class QAbstractItemModel;
class QToolBar;
class QVBoxLayout;
class QHBoxLayout;

namespace Mdt{ namespace ItemEditor{

  class AbstractController;
  class AbstractActionContainer;
  class InsertAction;
  class RemoveAction;
  class ResizeToContentsAction;

  /*! \brief Common base for QTableView based editor
   */
  class AbstractTableViewWidget : public QWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractTableViewWidget(QWidget* parent = nullptr);

    /*! \brief Get view
     */
    QTableView *view() const
    {
      return mView;
    }

    /*! \brief Add a tool bar to the top area
     */
    QToolBar *addToolBarToTopArea();

    /*! \brief Add a tool bar to the bottom area
     */
    QToolBar *addToolBarToBottomArea();

    /*! \brief Add insert action to top area
     */
    void addInsertActionToTopArea();

    /*! \brief Add insert action to bottom area
     */
    void addInsertActionToBottomArea();

    /*! \brief Set insert action text
     */
    void setInsertActionText(const QString & text);

    /*! \brief Add remove action to bottom area
     */
    void addRemoveActionToBottomBar();

    /*! \brief Set remove action text
     */
    void setRemoveActionText(const QString & text);

    /*! \brief Add resize to contents action to top area
     */
    void addResizeToContentsActionToTopBar();

    /*! \brief Register actions
     *
     * Once registered, actions will be informed
     *  each time row state or controller state changes.
     *
     * \pre actions must be a valid pointer
     */
    void registerActions(AbstractActionContainer *actions);

   public slots:

    /*! \brief Resize the view to its contents
     */
    void resizeViewToContents();

   protected:

    /*! \brief Access controller
     */
    virtual AbstractController *refController() const = 0;

   private:

    void createTopBarLayoutIfNot();
    void createBottomBarLayoutIfNot();
    void createTopToolsBarIfNot();
    void createTopEditBarIfNot();
    void createBottomEditBarIfNot();
    void createInsertActionIfNot();
    void createRemoveActionIfNot();
    void createResizeToContentsActionIfNot();

    QTableView *mView;
    QVBoxLayout *mMainLayout;
    QHBoxLayout *mTopBarLayout = nullptr;
    QHBoxLayout *mBottomBarLayout = nullptr;
    QToolBar *mTopToolsBar = nullptr;
    QToolBar *mTopEditBar = nullptr;
    QToolBar *mBottomEditBar = nullptr;
    InsertAction *mInsertAction = nullptr;
    RemoveAction *mRemoveAction = nullptr;
    ResizeToContentsAction *mResizeToContentsAction = nullptr;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_TABLE_VIEW_WIDGET_H
