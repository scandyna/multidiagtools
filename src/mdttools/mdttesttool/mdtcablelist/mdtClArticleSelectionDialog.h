/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_ARTICLE_SELECTION_DIALOG_H
#define MDT_CL_ARTICLE_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include <QSqlDatabase>
#include <QVariant>

/*! \brief Dialog for article selection (in Article_tbl)
 */
class mdtClArticleSelectionDialog : public mdtSqlSelectionDialog
{
 Q_OBJECT

 public:

   /*! \brief Constructor
    */
  mdtClArticleSelectionDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Get selected article ID
   *
   * If user rejected the dialog, a null value is returned.
   */
  QVariant selectedArticleId() const;

 private:

  Q_DISABLE_COPY(mdtClArticleSelectionDialog);
};

#endif // #ifndef MDT_CL_ARTICLE_SELECTION_DIALOG_H
