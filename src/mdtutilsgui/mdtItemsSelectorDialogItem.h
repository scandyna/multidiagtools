/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_ITEMS_SELECTOR_DIALOG_ITEM_H
#define MDT_ITEMS_SELECTOR_DIALOG_ITEM_H

#include <QString>
#include <Qt>

/*! \brief Data container for mdtItemsSelectorDialog
 */
class mdtItemsSelectorDialogItem
{
 public:

  /*! \brief Constructor
   */
  mdtItemsSelectorDialogItem();

  /*! \biref Set text
   */
  void setText(const QString & text);

  /*! \brief Get text
   */
  QString text(bool includeSortOrderText = false) const;

  /*! \brief Set column index
   */
  void setColumnIndex(int index);

  /*! \brief Get column index
   */
  inline int columnIndex() const { return pvColumnIndex; }

  /*! \brief Set name
   *
   * Name is a technical name, for example a field name, table name or else.
   */
  void setName(const QString & name);

  /*! \brief Get name
   *
   * Name is a technical name, for example a field name, table name or else.
   */
  inline QString name() const { return pvName; }

  /*! \brief Set sort order
   */
  void setSortOrder(Qt::SortOrder order);

  /*! \brief Get sort order
   */
  inline Qt::SortOrder sortOrder() const { return pvSortOrder; }

  /*! \brief Get sort order text
   */
  QString sortOrderText() const;

  /*! \brief Used by mdtItemsSelectorDialog to sort available items
   */
  bool operator<(const mdtItemsSelectorDialogItem & other) const;

 private:

  QString pvText;
  int pvColumnIndex;
  QString pvName;
  Qt::SortOrder pvSortOrder;
};

#endif // #ifndef MDT_ITEMS_SELECTOR_DIALOG_ITEM_H
