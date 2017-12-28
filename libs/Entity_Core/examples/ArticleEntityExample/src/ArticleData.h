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
#ifndef ARTICLE_DATA_H
#define ARTICLE_DATA_H

#include "ArticleDef.h"
#include "ArticleEntityExampleExport.h"
#include <Mdt/Entity/DataTemplate.h>

/*! \brief Data class of the Article entity
 */
class ARTICLEENTITYEXAMPLELIB_EXPORT ArticleData : public Mdt::Entity::DataTemplate<ArticleDataStruct, ArticleDef>
{
 public:

  /*! \brief Set id
   *
   * \pre \a id must be >= 0
   */
  void setId(qlonglong id);

  /*! \brief Get id
   */
  qlonglong id() const
  {
    return constData().id;
  }

  /*! \brief Set description
   */
  void setDescription(const QString & description);

  /*! \brief Get description
   */
  QString description() const
  {
    return constData().description;
  }
};

#endif // #ifndef ARTICLE_DATA_H
