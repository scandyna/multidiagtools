/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_DOUBLE_EDIT_DIALOG_H
#define MDT_DOUBLE_EDIT_DIALOG_H

#include "ui_mdtDoubleEditDialog.h"
#include <QDialog>
#include <QString>

/*! \brief Dialog for double value edition
 */
class mdtDoubleEditDialog : public QDialog, Ui::mdtDoubleEditDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructs a new double edit dialog with the given parent
   */
  mdtDoubleEditDialog(QWidget *parent = 0);

  /*! \brief Set label text
   */
  void setLabelText(const QString & text);

  /*! \brief Get label text
   */
  QString labelText() const;

  /*! \brief Set minimum
   */
  void setMinimum(double min);

  /*! \brief Set minimum to -infinity
   */
  void setMinimumToMinusInfinity();

  /*! \brief Get minimum
   */
  double minimum() const;

  /*! \brief Set maximum
   */
  void setMaximum(double max);

  /*! \brief Set maximum to infinity
   */
  void setMaximumToInfinity();

  /*! \brief Get maximum
   */
  double maximum() const;

  /*! \brief Set range
   */
  void setRange(double min, double max);

  /*! \brief Set value
   */
  void setValue(const QVariant & v);

  /*! \brief Set value
   */
  void setValueDouble(double v);

  /*! \brief Get value
   */
  double valueDouble() const;

  /*! \brief Get value
   */
  QVariant value() const;

 public slots:

  /*! \brief Hides the modal dialog and sets the result code to Accepted.
   */
  void accept();

 private:

  Q_DISABLE_COPY(mdtDoubleEditDialog);
};

#endif // #ifndef MDT_DOUBLE_EDIT_DIALOG_H
