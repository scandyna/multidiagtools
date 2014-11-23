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
#ifndef MDT_SQL_FIELD_SELECTION_DIALOG_H
#define MDT_SQL_FIELD_SELECTION_DIALOG_H

#include "ui_mdtSqlFieldSelectionDialog.h"
#include <QSqlDatabase>
#include <QString>
#include <QMap>

class QCheckBox;

namespace mdtSqlFieldSelectionDialogPrivate
{
  /*
   * Item container struct
   */
  struct selectionItem
  {
    // Members
    QString fieldName;
    QCheckBox *cb;
    // Constructor
    selectionItem(QWidget *parent = 0);
  };
};

/*! \brief Dialog that let the user select fields from a database table
 */
class mdtSqlFieldSelectionDialog : public QDialog, Ui::mdtSqlFieldSelectionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFieldSelectionDialog(QWidget * parent, QSqlDatabase db);

  /*! \brief Set message
   */
  void setMessage(const QString & msg);

  /*! \brief Get message
   */
  QString message() const{
    return lbMessage->text();
  }

  /*! \brief Add a field
   */
  void addField(const QString & fieldName, const QString & labelName, bool setSelected);

 private:

  /*! \brief Display items
   */
  void displayItems();

  /*! \brief Remove items
   */
  void removeItems();

  Q_DISABLE_COPY(mdtSqlFieldSelectionDialog);

  QMap<QString, mdtSqlFieldSelectionDialogPrivate::selectionItem> pvItems;
  QSqlDatabase pvDatabase;  /// \todo Voir si utile..
};

#endif // #ifndef MDT_SQL_FIELD_SELECTION_DIALOG_H
