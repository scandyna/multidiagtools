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
#ifndef MDT_CL_ARTICLE_LINK_H
#define MDT_CL_ARTICLE_LINK_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include "mdtError.h"
#include "mdtClArticleLinkData.h"
#include "mdtClArticleLinkKeyData.h"
#include "mdtSqlRecord.h"
#include <QSqlRecord>
#include <QList>

/*! \brief Helper class for article link manipulations
 *
 * Acts on ArticleLink_tbl
 */
class mdtClArticleLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClArticleLink(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClArticleLink(QSqlDatabase db);

  /*! \brief Add article link to database
   *
   * \param data Article link data to store
   */
  bool addLink(const mdtClArticleLinkData & data);

  /*! \brief Get article link data from database
   *
   * \param key Data that contains start and end article connection IDs (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClArticleLinkData getLinkData(const mdtClArticleLinkPkData & key, bool & ok);

  /*! \brief Get number of links in Link_tbl that are based on given article link
   *
   * \return Number of related links in Link_tbl or value < 0 on error
   */
  int relatedLinksCount(const mdtClArticleLinkPkData & pk);

  /*! \brief Check if given article link has related links in Link_tbl
   */
  bool hasRelatedLinks(const mdtClArticleLinkPkData & pk, bool & ok)
  {
    int n = relatedLinksCount(pk);
    if(n < 0){
      ok = false;
      return false;
    }
    ok = true;
    return (n > 0);
  }

 private:

  /*! \brief Fill given record with given article link data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClArticleLinkData & data);

  /*! \brief Fill given article link data with given record
   *
   * \pre record must contain all fields from ArticleLink_tbl
   */
  void fillData(mdtClArticleLinkData & data, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClArticleLink);
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_H
