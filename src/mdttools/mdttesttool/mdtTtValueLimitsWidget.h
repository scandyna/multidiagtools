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
#ifndef MDT_TT_VALUE_LIMITS_WIDGET_H
#define MDT_TT_VALUE_LIMITS_WIDGET_H

#include "ui_mdtTtValueLimitsWidget.h"
#include "mdtTtValueLimits.h"
#include <QWidget>

/*! \brief Widget to display and edit mdtTtValueLimits
 */
class mdtTtValueLimitsWidget : public QWidget, Ui::mdtTtValueLimitsWidget
{
 Q_OBJECT

 // For unit tests
 friend class mdtTtValueLimitsTest;

 public:

  /*! \brief Constructor
   */
  mdtTtValueLimitsWidget(QWidget *parent = 0);

 private slots:

  /*! \brief Let user edit left bottom limit
   */
  void editLeftBottomLimit();

  /*! \brief Let user edit left top limit
   */
  void editLeftTopLimit();

  /*! \brief Let user edit right bottom limit
   */
  void editRightBottomLimit();

  /*! \brief Let user edit right top limit
   */
  void editRightTopLimit();

 private:

  /*! \brief Update left limits widgets
   */
  void displayLeftLimits();

  /*! \brief Update right limits widgets
   */
  void displayRightLimits();

  Q_DISABLE_COPY(mdtTtValueLimitsWidget);

  mdtTtValueLimits pvLimits;
};

#endif // #ifndef MDT_TT_VALUE_LIMITS_WIDGET_H
