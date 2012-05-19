/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_UIC_NUMBER_WIDGET_H
#define MDT_UIC_NUMBER_WIDGET_H

#include <QWidget>
#include <QString>
#include "mdtUicNumber.h"
#include "ui_mdtUicNumberWidget.h"

/*! \brief UIC Number Widget
 */
class mdtUicNumberWidget : public QWidget, Ui::mdtUicNumberWidget
{
 Q_OBJECT

 public:

  mdtUicNumberWidget(QWidget *parent = 0);
  ~mdtUicNumberWidget();

  /*! \brief Set the uicNumber object
   * 
   * \pre uicNumber must be a valid pointer
   */
  void setUicNumber(mdtUicNumber *uicNumber);

  /*! \brief Get uicNumber object
   * 
   * \pre Valid object muts be set before with setUicNumber()
   */
  mdtUicNumber *uicNumber();

  /*! \brief Set the directory containing UIC databases CSV files
   */
  void setUicDbsDir(const QString &dir);
  
 private slots:

  void checkUicNumber();
  void diseableDetailsButton();
  void showDetails(bool show);
  void updateDetails(bool valid);

 private:

  mdtUicNumber *pvUicNumber;
  QString pvUicDbsDir;        // Directory containig UIC CSV databases files

};

#endif  // #ifndef MDT_UIC_NUMBER_WIDGET_H
