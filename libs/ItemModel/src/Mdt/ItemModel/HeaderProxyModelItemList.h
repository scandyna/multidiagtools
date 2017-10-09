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
 ** along with Mdt. If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_LIST_H
#define MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_LIST_H

#include "HeaderProxyModelItem.h"
#include "MdtItemModelExport.h"
#include <QString>
#include <QStringList>
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Container for HeaderProxyModel
   */
  class MDT_ITEMMODEL_EXPORT HeaderProxyModelItemList
  {
   public:

    /*! \brief Get count of items in this list
     */
    int count() const
    {
      return mList.size();
    }

    /*! \brief Set labels
     *
     * If count of labels in \a labels is > count(),
     *  default constructed items are added to this list,
     *  and label will also be set for these new items.
     *  If count of labels in \a labels is < count(),
     *  existing labels at end will be removed.
     *
     * \note Any existing attributes other than labels are keeped unchanged.
     */
    void setLabels(const QStringList & labels);

    /*! \brief Set label for given index
     *
     * If \a index is >= count(),
     *  default constructed items are added to this list,
     *  and label will be set for item at \a index
     *
     * \pre \a index must be >= 0
     */
    void setLabelAt(int index, const QString & label);

    /*! \brief Get label at given index
     *
     * \pre \a index must be in valid range ( 0 <= index < count() )
     */
    QString labelAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());
      return mList[index].label();
    }

    /*! \brief Clear this list
     */
    void clear();

   private:

    std::vector<HeaderProxyModelItem> mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_LIST_H
