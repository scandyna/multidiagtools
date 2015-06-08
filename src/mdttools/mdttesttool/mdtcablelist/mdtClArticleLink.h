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

 private:

  /*! \brief Fill given record with given article link data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClArticleLinkData & data);

  Q_DISABLE_COPY(mdtClArticleLink);
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_H
