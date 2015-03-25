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
#ifndef MDT_TT_TEST_STEP_CONTAINER_WIDGET_H
#define MDT_TT_TEST_STEP_CONTAINER_WIDGET_H

#include "mdtTtTestStepWidget.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QList>

/*! \brief Widget that contains mdtTtTestStepWidget items
 *
 * Note: if this widget is used in a QScrollArea,
 *       set the QScrollArea::widgetResizable property to true,
 *       else some problems will occur when internal widgets
 *       are updated (with new text, style, ...).
 */
class mdtTtTestStepContainerWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestStepContainerWidget(QWidget *parent = 0);

  /*! \brief Add a test item widget
   */
  void addItem(mdtTtTestStepWidget *w);

  /*! \brief Add a stretch to internal layout
   */
  void addStretch();

  /*! \brief Enable or disable run/abort function of all widgets
   */
  void setRunAbortEnabled(bool enable);

 private slots:

  /*! \brief Disable run/abort function of all item widgets, expect the one that called this slot
   */
  void disableRunAbortOfOtherWidgets(mdtTtTestStepWidget *callerWidget);

  /*! \brief Enable run/abort function of all item widgets
   */
  void enableRunAbortOfAllWidgets();

 private:

  Q_DISABLE_COPY(mdtTtTestStepContainerWidget);

  QVBoxLayout *pvLayout;
  QList<mdtTtTestStepWidget*> pvStepWidgets;
};

#endif // #ifndef MDT_TT_TEST_STEP_CONTAINER_WIDGET_H
