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
#ifndef MDT_ITEM_EDITOR_STANDARD_WINDOW_H
#define MDT_ITEM_EDITOR_STANDARD_WINDOW_H

#include "ui_StandardWindow.h"
#include <QMainWindow>

namespace Mdt{ namespace ItemEditor{

  /*! \brief StandardWindow cam be used as editor for basic cases
   *
   * Based on QMainWindow, it displays a menu, toolbar and a status bar.
   */
  class StandardWindow : public QMainWindow, Ui::StandardWindow
  {
   Q_OBJECT

   public:

    /*! \brief Construct a empty window
     */
    StandardWindow(QWidget *parent = nullptr);

   private:

    
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_STANDARD_WINDOW_H
