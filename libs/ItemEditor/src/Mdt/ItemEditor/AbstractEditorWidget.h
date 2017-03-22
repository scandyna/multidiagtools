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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H
#define MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H

#include "AbstractController.h"
#include <QWidget>
#include <QPointer>

class QToolBar;
// class QVBoxLayout;
// class QHBoxLayout;
class QAbstractProxyModel;

namespace Mdt{ namespace ItemEditor{

  class AbstractEditorLayout;
//   class AbstractController;
  class AbstractActionContainer;
  class InsertAction;
  class RemoveAction;
  class EditionActions;

  /*! \brief Base class to create a editor widget
   *
   * AbstractEditorWidget provides 3 areas:
   *  - Top area that can hold tool bars
   *  - Central widget
   *  - Bottom area can hold tool bars
   *
   * Set the central widget with setCentralWidget().
   *
   * By default, no tool bar is instanciated.
   *  Use method like addInsertActionToTopArea() or addInsertActionToBottomArea()
   *  which will create a tool bar in top area or bottom area respectively.
   *  To add a custom widget, use addWidgetToTopArea() or addWidgetToBottomArea().
   *  It is also possible to add custom tool bar using addToolBarToTopArea() or addToolBarToBottomArea().
   *
   * AbstractEditorWidget does not own any controller,
   *  but references the one a subclass handles.
   */
  class AbstractEditorWidget : public QWidget
  {
   Q_OBJECT

   public:

//     /*! \brief Used ba addActionToTopBar() and addActionToBottomBar()
//      */
//     enum AddSeparatorInToolBar
//     {
//       AddSeparator,
//       NoSeparator
//     };

    /*! \brief Constructor
     */
    explicit AbstractEditorWidget(QWidget* parent = nullptr);

//     /*! \brief Get controller
//      */
//     AbstractController *controller() const
//     {
//       Q_ASSERT(pvController != nullptr);
//       return pvController;
//     }

    /*! \brief Set central widget
     *
     * \pre \a widget must be a valid pointer
     * \note This AbstractEditorWidget takes ownership of \a widget
     *        (it will delete it at the appropriate time)
     */
    void setCentralWidget(QWidget *widget);

    /*! \brief Add a tool bar to the top area
     */
    QToolBar *addToolBarToTopArea();

    /*! \brief Add a tool bar to the bottom area
     */
    QToolBar *addToolBarToBottomArea();

//     /*! \brief Add a action to the top bar
//      *
//      * The top tool bar will be created if not allready,
//      *  then \a action will be added to it.
//      *
//      * If \a addSeparator is AddSeparator, a separator is added if a action allready exists before \a action .
//      *
//      * \pre \a action must be a valid pointer
//      * \note The enable state of \a action is not handled automatically.
//      *        The recommanded way is to create a subclass of AbstractActionContainer,
//      *        add each of its actions, then register the container using registerActions().
//      * \note This AbstractEditorWidget will not take ownership of \a action
//      *        (The caller is responsible to delete it)
//      */
//     void addActionToTopBar(QAction *action, AddSeparatorInToolBar addSeparator = NoSeparator);

    /*! \brief Add a widget to the top area
     *
     * This AbstractEditorWidget takes the ownership of \a widget
     *  (it will delete it when this is deleted)
     *
     * \pre \a widget must be a valid pointer
     */
    void addWidgetToTopArea(QWidget *widget);

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

    /*! \brief Add edition actions to bottom area
     */
    void addEditionActionsToBottomArea();

    /*! \brief Register actions
     *
     * Once registered, \a actions will be informed
     *  each time row state or controller state changes.
     *
     * \pre \a actions must be a valid pointer
     */
    void registerActions(AbstractActionContainer *actions);

    /*! \brief Prepend a proxy model
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void prependProxyModel(QAbstractProxyModel *proxyModel);

    /*! \brief Append a proxy model
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void appendProxyModel(QAbstractProxyModel *proxyModel);

   protected:

    /*! \brief Get controller
     *
     * Subclass using a concrete controller must implement this method.
     */
    virtual AbstractController *abstractController() const = 0;

//     /*! \brief Set controller
//      *
//      * Subclass that owns its controller
//      *  must tell it by calling this method.
//      */
//     virtual void setController(AbstractController *controller);

   private:

    void createTopEditBarIfNot();
    void createBottomEditBarIfNot();
    void createTopCustomWidgetsBarIfNot();
    void createInsertActionIfNot();
    void createRemoveActionIfNot();
    void createEditionActionsIfNot();

    AbstractEditorLayout *mMainLayout;
    QToolBar *mTopEditBar = nullptr;
    QToolBar *mBottomEditBar = nullptr;
    QToolBar *mTopCustomWidgetsBar = nullptr;
    InsertAction *mInsertAction = nullptr;
    RemoveAction *mRemoveAction = nullptr;
    EditionActions *mEditionActions = nullptr;

//     AbstractController *pvController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H
