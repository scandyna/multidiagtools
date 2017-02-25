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
#include "ForeignKeyRecord.h"
#include "PkFkProxyModelBase.h"
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
   * proxyModel->setForeignKey({1,2});
   * proxyModel->setForeignKeyEditable(false);
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

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_H
