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
#ifndef MDT_CL_ARTICLE_CONNECTION_H
#define MDT_CL_ARTICLE_CONNECTION_H

#include "mdtClConnector.h"

/*! \brief Helper class for article connection and article connector manipulations
 *
 * Acts mainly on ArticleConnection_tbl and ArticleConnector_tbl
 */
class mdtClArticleConnection : public mdtClConnector
{
 public:

  /*! \brief Constructor
   */
  mdtClArticleConnection(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClArticleConnection(QSqlDatabase db);

 private:

  Q_DISABLE_COPY(mdtClArticleConnection);
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_H
