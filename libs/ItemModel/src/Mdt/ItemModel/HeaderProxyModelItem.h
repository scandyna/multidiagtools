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
#ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_H
#define MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_H

#include <QString>

namespace Mdt{ namespace ItemModel{

  /*! \brief HeaderProxyModelItem is a item in a HeaderProxyModel header
   */
  class HeaderProxyModelItem
  {
   public:

    /*! \brief Set label
     */
    void setLabel(const QString & label)
    {
      mLabel = label;
    }

    /*! \brief Get label
     */
    QString label() const
    {
      return mLabel;
    }

   private:

    QString mLabel;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_HEADER_PROXY_MODEL_ITEM_H
