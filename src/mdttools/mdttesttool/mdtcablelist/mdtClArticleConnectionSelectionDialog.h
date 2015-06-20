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
#ifndef MDT_CL_ARTICLE_CONNECTION_SELECTION_DIALOG_H
#define MDT_CL_ARTICLE_CONNECTION_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include "mdtClArticleConnectionKeyData.h"
#include "mdtClArticleConnectionData.h"
#include "mdtClArticleConnectorKeyData.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

/*! \brief Dialog for article connection selection (in ArticleConnection_tbl)
 */
class mdtClArticleConnectionSelectionDialog : public mdtSqlSelectionDialog
{
 public:

  /*! \brief Article connection to article connector membership filter
   */
  enum class ArticleConnectorMembership_t
  {
    PartOfArticleConnector,     /*!< List only article connections that are part of a article connector */
    NotPartOfArticleConnector,  /*!< List only article connections that are not part of a article connector */
    All                         /*!< Don't care if article connection is part of article connector or not, list all */
  };

  /*! \brief Article connection usage in article link filter
   */
  enum class ArticleLinkUsage_t
  {
    NotUsedAsConnectionStart, /*!< List only article connections that are not used in article link table as article connection start */
    NotUsedAsConnectionEnd,   /*!< List only article connections that are not used in article link table as article connection end */
    All                       /*!< Don't care about article link, list all */
  };

   /*! \brief Constructor
    */
  mdtClArticleConnectionSelectionDialog(QWidget *parent);

  /*! \brief Select article connections that are part of given article
   */
  bool select(QSqlDatabase db, const QVariant & articleId, ArticleConnectorMembership_t acms, ArticleLinkUsage_t alu, bool allowMultiSelection);

  /*! \brief Select article connections that are part of given article and not used in given unit
   */
  bool select(QSqlDatabase db, const QVariant & articleId, const QVariant & unitId, ArticleConnectorMembership_t acms, bool allowMultiSelection);

  /*! \brief Select article connections that are part of given article connector and not used in given unit
   */
  bool select(QSqlDatabase db, const mdtClArticleConnectorKeyData & articleConnectorKey, const QVariant & unitId, bool allowMultiSelection);

  /*! \brief Get selected article connection key
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClArticleConnectionKeyData selectedArticleConnectionKey() const;

  /*! \brief Get a list of selected connection key
   *
   * If user rejected the dialog, a empty list is returned.
   */
  QList<mdtClArticleConnectionKeyData> selectedArticleConnectionKeyList() const;

  /*! \brief Get selected article connection data
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClArticleConnectionData selectedArticleConnectionData() const;

  /*! \brief Get a list of selected connection data
   *
   * If user rejected the dialog, a empty list is returned.
   */
  QList<mdtClArticleConnectionData> selectedArticleConnectionDataList() const;

 private:

  /*! \brief Set SQL query and other setup
   */
  bool setQuery(const QString & sql, QSqlDatabase & db, bool allowMultiSelection);

  /*! \brief Get clause regarding article connector membership
   */
  QString acmsClause(ArticleConnectorMembership_t acms) const;

  /*! \brief Get clause regarding article link usage
   */
  QString articleLinkUsageClause(ArticleLinkUsage_t alu, const QVariant & articleId) const;

  /*! \brief Build article connection key data
   */
  mdtClArticleConnectionKeyData buildKeyData(const mdtSqlTableSelection & s, int row) const;

  /*! \brief Get table name to use
   */
//   QString tableNameToUse(ArticleConnectorMembership_t acms) const;

  Q_DISABLE_COPY(mdtClArticleConnectionSelectionDialog);
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_SELECTION_DIALOG_H
