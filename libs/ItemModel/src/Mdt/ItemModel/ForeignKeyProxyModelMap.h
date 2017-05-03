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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_H

#include "ForeignKeyProxyModelMapItem.h"
#include "ForeignKey.h"
#include <QString>
#include <QHash>

namespace Mdt{ namespace ItemModel{

  /*! \brief Used for implementation of ForeignKeyProxyModel
   */
  class ForeignKeyProxyModelMap
  {
   public:

    /*! \brief Add a foreign key
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must not allready exist
     * \pre \a fk must not be null
     */
    void addForeignKey(const QString & foreignEntityName, const Mdt::ItemModel::ForeignKey & fk);

    /*! \brief Remove a foreign key
     *
     * \pre \a foreignEntityName must not be empty
     */
    void removeForeignKey(const QString & foreignEntityName);

    /*! \brief Check if a foreign key referencing a entity exists
     *
     * \pre \a entityName must not be empty
     */
    bool hasForeignKeyReferencing(const QString & entityName) const;

    /*! \brief Get foreign key referencing a entity
     *
     * Returns the foreign key referencing the entity named \a entityName if it exists,
     *  otherwise a null foreign key.
     *
     * \pre \a entityName must not be empty
     */
    ForeignKey getForeignKeyReferencing(const QString & entityName) const;

    /*! \brief Set foreign key editable
     *
     * By default, all foreign keys are editable
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    void setForeignKeyEditable(const QString & foreignEntityName, bool editable);

    /*! \brief Set all foreign key editable
     *
     * By default, all foreign keys are editable
     */
    void setAllForeignKeysEditable(bool editable);

    /*! \brief Check if foreign key is editable
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    bool isForeignKeyEditable(const QString & foreignEntityName) const;

    /*! \brief Set foreign key items enabled
     *
     * By default, all foreign keys items are enabled.
     *
     * \pre \a foreignEntityName must not be empty
     */
    void setForeignKeyItemsEnabled(const QString & foreignEntityName, bool enable);

    /*! \brief Set foreign key items enabled for all foreign keys
     *
     * By default, all foreign keys items are enabled.
     */
    void setAllForeignKeysItemsEnabled(bool enable);

    /*! \brief Check if foreign key items are enabled
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    bool isForeignKeyItemsEnabled(const QString & foreignEntityName) const;

    /*! \brief Get most restricitive flags for a column
     *
     * \pre \a column must be >= 0
     */
    ForeignKeyProxyModelMapItemFlags getMostRestrictiveFlagsForColumn(int column) const;

   private:

    QHash<QString, ForeignKeyProxyModelMapItem> mMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_MAP_H
