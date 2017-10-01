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
#ifndef MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H
#define MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H

#include "ColumnList.h"
#include "KeyRecord.h"
#include <QIdentityProxyModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Common base for PrimaryKeyProxyModel and ForeignKeyProxyModel
   */
  class PkFkProxyModelBase : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit PkFkProxyModelBase(QObject* parent = nullptr);

   protected:

    /*! \brief Get key record for row
     *
     * \pre \a row must be in correct range ( 0 <= row < rowCount() )
     */
    KeyRecord getKeyRecord(int row, const ColumnList & key) const;

    /*! \brief Find first row that matches key record
     *
     * Returns the first row that matches, otherwise -1
     */
    int findFirstRowForKeyRecord(const KeyRecord & record) const;

   private:

    bool rowMatchesKeyRecord(int row, const KeyRecord & record) const;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_PK_FK_PROXY_MODEL_BASE_H
