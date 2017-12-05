/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_MENU_H
#define MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_MENU_H

#include <QMenu>
#include <QString>
#include "MdtTranslation_WidgetsExport.h"

namespace Mdt{ namespace Translation{

  /*! \brief The LanguageSelectionMenu class provides a language selection menu
   *
   * \code
   * using namespace Mdt::Translation;
   *
   * QMainWindow window;
   * auto *languageSelectionMenu = new LanguageSelectionMenu(&window);
   * window.menuBar()->addMenu(languageSelectionMenu);
   * window.show();
   * \endcode
   */
  class MDT_TRANSLATION_WIDGETS_EXPORT LanguageSelectionMenu : public QMenu
  {
   Q_OBJECT

   public:

    /*! \brief Construct a language selection menu
     */
    explicit LanguageSelectionMenu(QWidget *parent = nullptr);

    /*! \brief Construct a language selection menu
     */
    explicit LanguageSelectionMenu(const QString & title, QWidget *parent = nullptr);

  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_WIDGETS_LANGUAGE_SELECTION_MENU_H
