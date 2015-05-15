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
#include <vector>

class mdtTtTestStepContainer;
class QFrame;

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

  /*! \brief Set test step container
   *
   * Given container will signal when a widget is added,
   *  and this widget will also be added to widgets layout.
   */
  void setContainer(mdtTtTestStepContainer *c);

  /*! \brief Get test step widgets count
   */
  int stepWidgetsCount() const
  {
    return pvStepWidgetsCount;
  }

  /*! \brief Show separation lines
   */
  void showSeparationLines();

  /*! \brief Hide separation lines
   */
  void hideSeparationLines();

 public slots:

  /*! \brief Add a test step widget
   */
  void addStepWidget(mdtTtTestStepWidget *tsw);

  /*! \brief Remove a test step widget
   *
   * Given widget will be removed from layout.
   * Note that it will also be destroyed.
   */
  void removeStepWidget(mdtTtTestStepWidget *tsw);

 private:

  /*! \brief Add a separation line
   */
  void addSeparationLine();

  /*! \brief remove last added separation line
   */
  void removeSeparationLine();

  /*! \brief Update separation lines visiblity
   */
  void updateSeparationLinesVisibility();

  Q_DISABLE_COPY(mdtTtTestStepContainerWidget);

  QVBoxLayout *pvLayout;
  int pvStepWidgetsCount;
  bool pvSeparationLinesVisible;
  std::vector<QFrame*> pvSeparationLines; // We hold pointers to be able to set lines visible or not visible
};

#endif // #ifndef MDT_TT_TEST_STEP_CONTAINER_WIDGET_H
