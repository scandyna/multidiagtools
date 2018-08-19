/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H
#define MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H

#include "VariantRecord.h"
#include "AbstractReadOnlyCache.h"
#include "MdtEntity_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Entity{

  /*! \brief
   */
  class MDT_ENTITY_CORE_EXPORT AbstractAsyncReadOnlyCache : public AbstractReadOnlyCache
  {
   Q_OBJECT

    using BaseClass = AbstractReadOnlyCache;

   public:

    /*! \brief Constructor
     */
    explicit AbstractAsyncReadOnlyCache(QObject *parent = nullptr);

   private:

    
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H
