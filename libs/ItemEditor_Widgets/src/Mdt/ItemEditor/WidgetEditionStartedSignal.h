/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_WIDGET_EDITION_STARTED_SIGNAL_H
#define MDT_ITEM_EDITOR_WIDGET_EDITION_STARTED_SIGNAL_H

#include "MdtItemEditor_WidgetsExport.h"
#include <QObject>
#include <QWidget>
#include <QMetaMethod>

class QComboBox;
class QLineEdit;

namespace Mdt{ namespace ItemEditor{

//   Helper class to get the most appropriate signal to notifiy that the user began to edit

  /*! \brief Helper class that emits a signal when edition started for a widget
   *
   * \code
   * WidgetEditionStartedSignal widgetEditionStartedSignal;
   * connect(&widgetEditionStartedSignal, &WidgetEditionStartedSignal::editionStarted, this, &MyClass::mySlot);
   *
   * QLineEdit lineEdit;
   * widgetEditionStartedSignal.addWidget(&lineEdit);
   *
   * QComboBox comboBox;
   * widgetEditionStartedSignal.addWidget(&comboBox);
   * \endcode
   *
   * \sa DataWidgetMapper
   */
  class MDT_ITEMEDITOR_WIDGETS_EXPORT WidgetEditionStartedSignal : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit WidgetEditionStartedSignal(QObject *parent = nullptr);

    /*! \brief Add \a widget
     */
    void addWidget(const QWidget * const widget);

    /*! \brief Remove \a widget
     */
    void removeWidget(const QWidget * const widget);

    /*! \brief Returns the meta method that correspond the signal that notifies a edition began
     *
     * This implementation handles most of the Qt edition widgets like
     *  QLineEdit, QComboBox, QSpinBox, ...
     *
     * To support other widgets, this method can be re-implemented in a subclass.
     *
     * If \a widget has no known signal to notify a edition began,
     *  a invalid meta method is returned
     *
     * \pre \a widget must be a valid pointer
     */
    virtual QMetaMethod getEditionStartedSignal(const QWidget * const widget) const;

   signals:

    /*! \brief Emitted when one of the widget notified edition started
     */
    void editionStarted();

   protected:

    /*! \brief Connect the signal that notifies edition began to onWidgetEditionStarted() slot
     *
     * This implementation handles most of the Qt edition widgets like
     *  QLineEdit, QComboBox, QSpinBox, ...
     *
     * If \a widget is not directly handled by this method, it will use the user property of the widget.
     *  If no user property is defined, or it has no notify signal,
     *  a warning is logged and no connection is made at all.
     *
     * To support other widgets, this method can be re-implemented in a subclass.
     *  In this case, disconnectWidgetEditionStartedSignal() should also be re-implemented.
     */
    virtual void connectWidgetEditionStartedSignal(const QWidget * const widget);

    /*! \brief Disconnect the signal that notifies edition began to onWidgetEditionStarted() slot
     *
     * \sa connectWidgetEditionStartedSignal()
     */
    virtual void disconnectWidgetEditionStartedSignal(const QWidget * const widget);

   private slots:

    void onWidgetEditionStarted();

   private:

    void connectQLineEditEditionStartedSignal(const QLineEdit * const lineEdit);
    void disconnectQLineEditEditionStartedSignal(const QLineEdit * const lineEdit);

    void connectQComboboxEditionStartedSignal(const QComboBox * const comboBox);
    void disconnectQComboboxEditionStartedSignal(const QComboBox * const comboBox);

    QMetaMethod getEditionStartedSignalFromUserProperty(const QWidget * const widget) const;
    QMetaMethod getOnDataEditionStartedMethod() const;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_WIDGET_EDITION_STARTED_SIGNAL_H
