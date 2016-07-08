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
#ifndef MDT_ITEM_EDITOR_STANDARD_EDITOR_LAYOUT_WIDGET_H
#define MDT_ITEM_EDITOR_STANDARD_EDITOR_LAYOUT_WIDGET_H

#include <QWidget>
#include <QString>
#include <QIcon>

class QTabWidget;
class QVBoxLayout;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Provides a standard layout of for a editor
   */
  class StandardEditorLayoutWidget : public QWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit StandardEditorLayoutWidget(QWidget* parent = nullptr);

    /*! \brief Set main widget
     *
     * This StandardEditorLayoutWidget will take ownership of widget.
     *  If a widget was allready set, it will be deleted first.
     *
     * \pre widget must be a valid pointer
     */
    void setMainWidget(QWidget *widget);

    /*! \brief Add widget to childs tab
     *
     * All child widgets are added to a internal QTabWidget.
     *
     * This StandardEditorLayoutWidget will take ownership of widget.
     *
     * \pre widget must be a valid pointer
     */
    void addChildWidget(QWidget *widget, const QString & label, const QIcon & icon = QIcon());

   private:

    QVBoxLayout *pvMainLayout;
    QWidget *pvMainWidget;
    QTabWidget *pvChildsTabWidget;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_STANDARD_EDITOR_LAYOUT_WIDGET_H
