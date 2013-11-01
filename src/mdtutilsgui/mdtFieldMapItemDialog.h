/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef _MDTFIELDMAPITEMDIALOG_H
#define _MDTFIELDMAPITEMDIALOG_H

#include "ui_mdtFieldMapItemDialog.h"
#include "mdtFieldMap.h"
#include <QDialog>
#include <QWidget>
#include <QList>
#include <QString>

class mdtFieldMapItem;

/*! \brief A dialog to setup a field map item
 */
class mdtFieldMapItemDialog : public QDialog, public Ui::mdtFieldMapItemDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtFieldMapItemDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtFieldMapItemDialog();

  /*! \brief Set source fields
   */
  void setSourceFields(const QList<mdtFieldMapField> & fields);

  /*! \brief Set destination fields
   */
  void setDestinationFields(const QList<mdtFieldMapField> & fields);

  /*! \brief Set source data (used for preview)
   */
  void setSourceData(const QString & data);

  /*! \brief Set map item
   */
  void setMapItem(const mdtFieldMapItem & item);

  /*! \brief Get map item
   */
  mdtFieldMapItem mapItem() const;

 public slots:

  /*! \brief
   */
  void accept();

 private slots:

  /*! \brief Set the source field
   */
  void setSourceField(int cbIndex);

  /*! \brief Edit destination fields list
   */
  void editDestinationFieldList();

  /*! \brief Set the destination field
   */
  void setDestinationField(int cbIndex);

  /*! \brief Update the destination text preview
   */
  void updateDestinationPreview();

  /*! \brief Set source data start offset
   */
  void setSourceDataStartOffset(int value);

  /*! \brief Set source data end offset
   */
  void setSourceDataEndOffset(int value);

  /*! \brief Enable/disable splitt option widgets
   */
  void setSrcSplitParameterWidgetsEnabled(bool enable);

 private:

  Q_DISABLE_COPY(mdtFieldMapItemDialog);

  ///mdtFieldMapItem pvEditMapItem;
  ///mdtFieldMapItem pvTargetMapItem;
  mdtFieldMapItem pvMapItem;
};

#endif // #ifndef MDTFIELDMAPITEMDIALOG_H
