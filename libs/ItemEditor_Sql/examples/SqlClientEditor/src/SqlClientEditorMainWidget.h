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
#ifndef SQL_CLIENT_EDITOR_MAIN_WIDGET_H
#define SQL_CLIENT_EDITOR_MAIN_WIDGET_H

#include "Mdt/ItemEditor/StandardEditorLayoutWidget.h"
#include "Mdt/ItemEditor/AbstractController.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QPointer>

class SqlClientWidget;
class SqlAddressWidget;

/*! \brief Main widget of SQL Client editor
 */
class SqlClientEditorMainWidget : public Mdt::ItemEditor::StandardEditorLayoutWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  explicit SqlClientEditorMainWidget(QWidget* parent = nullptr);

  /*! \brief Setup
   */
  bool setup(const QSqlDatabase & db);

  /*! \brief Get main controller
   */
  Mdt::ItemEditor::AbstractController *mainController() const
  {
    return mMainController;
  }

 private:

  SqlClientWidget *mClientWidget;
  SqlAddressWidget *mAddressWidget;
  QPointer<Mdt::ItemEditor::AbstractController> mMainController;
};

#endif // #ifndef SQL_CLIENT_EDITOR_MAIN_WIDGET_H
