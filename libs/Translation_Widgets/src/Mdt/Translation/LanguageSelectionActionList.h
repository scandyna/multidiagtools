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
#ifndef MDT_TRANSLATION_LANGUAGE_SELECTION_ACTION_LIST_H
#define MDT_TRANSLATION_LANGUAGE_SELECTION_ACTION_LIST_H

#include "Mdt/Translation/LanguageCodeList.h"
#include "MdtTranslation_WidgetsExport.h"
#include <QList>

class QAction;
class QObject;

namespace Mdt{ namespace Translation{

  /*! \brief Helper class to create actions for language selection
   */
  class MDT_TRANSLATION_WIDGETS_EXPORT LanguageSelectionActionList
  {
   public:

    /*! \brief Create actions for \a languages
     */
    void createActions(const LanguageCodeList & languages, QObject *parent);

    /*! \brief Get actions
     */
    QList<QAction*> actions() const
    {
      return mActions;
    }

   private:

    QList<QAction*> mActions;
  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_LANGUAGE_SELECTION_ACTION_LIST_H
