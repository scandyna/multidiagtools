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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_H

#include "ForeignKey.h"
#include "ForeignKeyProxyModelMap.h"
#include "ForeignKeyRecord.h"
#include "PkFkProxyModelBase.h"
#include <QString>
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief Hold informations about a foreign key of a item model
   *
   * Example of usage:
   * \code
   * QTableView view;
   * auto *model = new MyModel(&view);
   * auto *proxyModel = new ForeignKeyProxyModel(&view);
   *
   * proxyModel->setSourceModel(model);
   * proxyModel->addForeignKey("Client", {1,2});
   * proxyModel->setForeignKeyEditable("Client", false);
   *
   * view.setModel(proxyModel);
   * \endcode
   */
  class ForeignKeyProxyModel : public PkFkProxyModelBase
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit ForeignKeyProxyModel(QObject* parent = nullptr);

    /*! \brief Add a foreign key
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must not allready exist
     * \pre \a fk must not be null
     */
    void addForeignKey(const QString & foreignEntityName, const Mdt::ItemModel::ForeignKey & fk);

    /*! \brief Add a foreign key
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must not allready exist
     * \pre Each column in \a fk must be >= 0
     * \pre Each column in \a fk must be unique
     */
    void addForeignKey(const QString & foreignEntityName, std::initializer_list<int> fk);

    /*! \brief Remove all foreign keys
     */
    void removeAllForeignKeys();

    /*! \brief Get foreign key referencing a entity
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

    /*! \brief Get item flags for given index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Get foreign key record for row referencing foreign entity
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     * \pre \a row must be in correct range ( 0 <= row < rowCount() )
     */
    ForeignKeyRecord getForeignKeyRecord(const QString & foreignEntityName, int row) const;

    /*! \brief Get a list of all columns that are part of a foreign key
     */
    ColumnList getColumnsPartOfForeignKey() const;

    /*! \brief Get a list of foreign keys
     */
    ForeignKeyList getForeignKeyList() const;


    /*! \brief Set foreign key
     *
     * \pre \a fk must not be null
     */
    void setForeignKey(const ForeignKey & fk);

    /*! \brief Set foreign key
     *
     * \pre Each column in \a fk must be >= 0
     * \pre Each column in \a fk must be unique
     */
    void setForeignKey(std::initializer_list<int> fk);

    /*! \brief Get foreign key
     */
    ForeignKey foreignKey() const;

    /*! \brief Set foreign key editable
     *
     * By default, foreign key is editable
     */
    void setForeignKeyEditable(bool editable);

    /*! \brief Check if foreign is editable
     */
    bool isForeignKeyEditable() const
    {
      return isKeyEditable();
    }

    /*! \brief Set foreign key items enabled
     *
     * By default, foreign key items are enabled.
     */
    void setForeignKeyItemsEnabled(bool enable);

    /*! \brief Check if foreign key items are enabled
     */
    bool isForeignKeyItemsEnabled() const
    {
      return isKeyItemsEnabled();
    }

    /*! \brief Get foreign key record for row
     *
     * \pre \a row must be in correct range ( 0 <= row < rowCount() )
     */
    ForeignKeyRecord foreignKeyRecord(int row) const;

   private:

    ForeignKeyProxyModelMap mMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_H
