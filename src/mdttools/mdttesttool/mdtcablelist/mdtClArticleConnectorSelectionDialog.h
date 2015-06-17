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
#ifndef MDT_CL_ARTICLE_CONNECTOR_SELECTION_DIALOG_H
#define MDT_CL_ARTICLE_CONNECTOR_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include "mdtClArticleConnectorKeyData.h"
#include <QSqlDatabase>
#include <QVariant>

/*! \brief Dialog for article connector selection (in ArticleConnector_tbl)
 */
class mdtClArticleConnectorSelectionDialog : public mdtSqlSelectionDialog
{
 Q_OBJECT

 public:

   /*! \brief Constructor
    */
  mdtClArticleConnectorSelectionDialog(QWidget *parent);

  /*! \brief Set SQL query and select data
   *
   * Will select article connectors that belongs to given article
   */
  bool select(QSqlDatabase db, const QVariant & articleId);

  /*! \brief Set SQL query and select data
   *
   * Will select article connectors that belongs to given article and that is not used in given unit
   */
  bool select(QSqlDatabase db, const QVariant & articleId, const QVariant & unitId);

  /*! \brief Get selected article connector key
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClArticleConnectorKeyData selectedArticleConnectorKey() const;

  /*! \brief Get selected article connector name
   *
   * If user rejected the dialog, a null value is returned.
   */
  QVariant selectedArticleConnectorName() const;

 private:

  Q_DISABLE_COPY(mdtClArticleConnectorSelectionDialog);
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTOR_SELECTION_DIALOG_H
