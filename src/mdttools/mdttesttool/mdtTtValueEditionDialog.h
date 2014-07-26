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
#ifndef MDT_TT_VALUE_EDITION_DIALOG_H
#define MDT_TT_VALUE_EDITION_DIALOG_H

#include "ui_mdtTtValueEditionDialog.h"
#include <QDialog>
#include <QVariant>

class QWidget;

/*! \brief Dialog to edit a value, limit and fail
 */
class mdtTtValueEditionDialog : public QDialog, Ui::mdtTtValueEditionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtValueEditionDialog(QWidget *parent = 0);

  /*! \brief Get limit value min
   */
  QVariant limitValueMin() const;

  /*! \brief Get limit value max
   */
  QVariant limitValueMax() const;

  /*! \brief Get fail value min
   */
  QVariant failValueMin() const;

  /*! \brief Get fail value min
   */
  QVariant failValueMax() const;

 private slots:

  void onValueChanged(double x, bool isValid);
  void onLimitMinChanged(double x);
  void onLimitMaxChanged(double x);
  void onFailMinChanged(double x);
  void onFailMaxChanged(double x);

 private:

  Q_DISABLE_COPY(mdtTtValueEditionDialog);
};


#endif
