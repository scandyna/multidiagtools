/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_LIST_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_LIST_H

#include "ForeignKey.h"
#include <QHash>
#include <QString>

namespace Mdt{ namespace ItemModel{

  /*! \brief Container for ForeignKey
   */
  class ForeignKeyList
  {
  public:

    /*! \brief Add a foreign key
     *
     * If foreign key support was not enabled,
     *  it will be enabled before adding \a fk.
     *
     * \pre \a foreignEntityName must not be empty
     * \pre \a fk must not be null
     */
    void addForeignKey(const QString & foreignEntityName, const Mdt::ItemModel::ForeignKey & fk);

    /*! \brief Get foreign key referencing a entity
     *
     * Returns the foreign key referencing entity named \a entityName if exists,
     *  otherwise a null foreign key.
     *
     * \pre \a entityName must not be empty
     */
    ForeignKey getForeignKeyReferencing(const QString & entityName) const;

  private:

    QHash<QString, ForeignKey> mMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_LIST_H
