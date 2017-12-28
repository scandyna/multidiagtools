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
#ifndef ARTICLE_DEF_H
#define ARTICLE_DEF_H

// #include "ArticleEntityExampleExport.h"
#include <Mdt/Entity/Def.h>
#include <QtGlobal>
#include <QString>

/*! \brief Data struct of the Article entity
 */
struct ArticleDataStruct
{
  qlonglong id = 0;
  QString description;
};

/*! \brief Definition struct of the Article entity
 */
MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
  (description, FieldMaxLength(250))
)

#endif // #ifndef ARTICLE_DEF_H
