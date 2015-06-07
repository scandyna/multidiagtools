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
#ifndef MDT_CL_ARTICLE_CONNECTION_DATA_H
#define MDT_CL_ARTICLE_CONNECTION_DATA_H

#include "mdtClArticleConnectionKeyData.h"
#include <QVariant>

#include "mdtSqlRecord.h"
#include <QList>
#include <QSqlDatabase>

/*! \brief Data container for article connection data
 *
 * Refers to ArticleConnection_tbl
 */
struct mdtClArticleConnectionData : public mdtSqlRecord  /// \todo When all is adapted, remove this inheritance
{
 private:

  /*! \brief Key data
   */
  mdtClArticleConnectionKeyData pvKeyData;

 public:

  /*! \brief Get article connection key data
   */
  mdtClArticleConnectionKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   *
   * \pre If key.articleConnectorFk is not null, key.articleConnectorFk.articleId must match key.articleId
   */
  void setKeyData(const mdtClArticleConnectionKeyData & key);

  /*! \brief Set article connector FK
   *
   * \pre key.articleId must match keyData().articleId
   */
  void setArticleConnectorFk(const mdtClArticleConnectorKeyData & key);

  /*! \brief Set connection type
   */
  void setConnectionType(const mdtClConnectionTypeKeyData & typeKey);

  /*! \brief Check if data is null
   *
   * Article connection data is null when its key is null
   */
  inline bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Check if article connection is part of a article connector
   */
  inline bool isPartOfArticleConnector() const
  {
    return pvKeyData.isPartOfArticleConnector();
  }

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Name (ArticleContactName)
   */
  QVariant name;

  /*! \brief Contact resistance (Resistance)
   */
  QVariant resistance;

  /*! \brief I/O type (IoType)
   */
  QVariant ioType;

  /*! \brief Function english (FunctionEN)
   */
  QVariant functionEN;

  /*! \brief Function frensh (FunctionFR)
   */
  QVariant functionFR;

  /*! \brief Function german (FunctionDE)
   */
  QVariant functionDE;

  /*! \brief Function italian (FunctionIT)
   */
  QVariant functionIT;

 public:

  /*! \brief Construct a empty mdtClArticleConnectionData
   * \deprecated
   */
  mdtClArticleConnectionData();

  /*! \brief Contruct a mdtClArticleConnectionData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from ArticleConnection_tbl must exist in record
   * \deprecated
   */
  mdtClArticleConnectionData(const QSqlRecord & record);

  /*! \brief Setup fields from ArticleConnection_tbl
   * \deprecated
   */
  bool setup(QSqlDatabase db);
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_DATA_H
