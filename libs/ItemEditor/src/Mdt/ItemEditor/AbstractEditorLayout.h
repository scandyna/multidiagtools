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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_LAYOUT_H
#define MDT_ITEM_EDITOR_ABSTRACT_EDITOR_LAYOUT_H

#include <QVBoxLayout>
#include <QPointer>

class QHBoxLayout;
class QWidget;
class QToolBar;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Helper class to layout a top bar area, a central widget and a bottom bar area
   */
  class AbstractEditorLayout : public QVBoxLayout
  {
   Q_OBJECT

   public:

    /*! \brief Set central widget
     *
     * \pre \a widget must be a valid pointer
     * \note The widget, on which this layout is set,
     *        will take ownership of \a widget (it will delete it when it is destroyed).
     *        If a widget was previously set, it will first be removed and deleted.
     */
    void setCentralWidget(QWidget *widget);

    /*! \brief Get central widget
     *
     * Returns the widget set by setCentralWidget()
     *  or a nullptr if no widget was set.
     */
    QWidget *centralWidget() const;

    /*! \brief Add a tool bar to the top area
     */
    QToolBar *addToolBarToTopArea();

    /*! \brief Add a tool bar to the bottom area
     */
    QToolBar *addToolBarToBottomArea();

   private:

    void createTopBarLayoutIfNot();
    void createBottomBarLayoutIfNot();

    QPointer<QWidget> mCentralWidget;
    QHBoxLayout *mTopBarLayout = nullptr;
    QHBoxLayout *mBottomBarLayout = nullptr;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_LAYOUT_H
