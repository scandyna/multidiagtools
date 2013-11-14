/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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

#include "mdtClConnectionData.h"

class mdtClArticleConnectionData : public mdtClConnectionData
{
 public:

  /*! \brief Set article ID
   */
  void setArticleId(const QVariant & id);

  /*! \brief Get article ID
   */
  QVariant articleId() const;

  /*! \brief Set article connector ID
   */
  void setArticleConnectorId(const QVariant & id);

  /*! \brief Get article connector ID
   */
  QVariant articleConnectorId() const;

  /*! \brief Set I/O type
   */
  void setIoType(const QVariant & type);

  /*! \brief Get I/O type
   */
  QVariant ioType() const;

 private:

  QVariant pvArticleId;
  QVariant pvArticleConnectorId;
  QVariant pvIoType;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_DATA_H
