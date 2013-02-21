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
#ifndef MDT_DEVICE_STATUS_WIDGET_H
#define MDT_DEVICE_STATUS_WIDGET_H

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include "mdtDevice.h"
#include "mdtBlinkLed.h"
#include "mdtPortThread.h"

class QLabel;
class QPushButton;
class QTimer;

/*! \brief
 */
class mdtDeviceStatusWidget : public QWidget
{
  Q_OBJECT

 public:

  /*! \brief Build a status widget with default texts
   */
  mdtDeviceStatusWidget(QWidget *parent = 0);

  ~mdtDeviceStatusWidget();

  /*! \brief Set device
   *
   * Will make some connections
   *
   * \pre device must be valid.
   */
  void setDevice(mdtDevice *device);

  /*! \brief Enable the TX/RX LEDs
   *
   * \pre txThread and rxThread must be valid.
   */
  void enableTxRxLeds(mdtPortThread *txThread, mdtPortThread *rxThread);

  /*! \brief Disable the TX/RX LED's
   */
  void disableTxRxLeds();

  /*! \brief Add a custom widget
   *
   * One custom widget can be added.
   */
  void addCustomWidget(QWidget *widget);

  /*! \brief Remove the custom widget
   *
   * \see addCustomWidget().
   */
  void removeCustomWidget();

  /*! \brief Set default state texts
   */
  void setDefaultStateTexts();

  /*! \brief Set text for ready state
   */
  void setStateReadyText(const QString &text);

  /*! \brief Set text for disconnected state
   */
  void setStateDisconnectedText(const QString &text);

  /*! \brief Set text for connecting state
   */
  void setStateConnectingText(const QString &text);

  /*! \brief Set text for busy state
   */
  void setStateBusyText(const QString &text);

  /*! \brief Set default state colors
   */
  void setDefaultStateColors();

  /*! \brief Set color for ready state
   */
  void setStateReadyColor(mdtLed::color_t color);

  /*! \brief Set color for connecting state
   */
  void setStateConnectingColor(mdtLed::color_t color);

  /*! \brief Set color for busy state
   */
  void setStateBusyColor(mdtLed::color_t color);

 public slots:

  /*! \brief Set state
   *
   * \param state A state as define in mdeDevice
   */
  void setState(int state);

  /*! \brief
   *
   * \param state A state as define in mdeDevice
   */
  void setState(int state, const QString &message, const QString &details);

  /*! \brief Used to show a message
   *
   * \param message Message to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showMessage(const QString &message, int timeout = 0);

 private slots:

  /*! \brief Trig the TX led for 100 ms
   *
   * \pre TX led must be set (see enableTxRxLeds)
   */
  void trigTxLed();

  /*! \brief Trig the RX led for 100 ms
   *
   * \pre RX led must be set (see enableTxRxLeds)
   */
  void trigRxLed();

  /*! \brief Re-display state text after timeout
   *
   * Used by showMessage()
   */
  void backToStateText();

 private:

  Q_DISABLE_COPY(mdtDeviceStatusWidget);

  QGridLayout *pvLayout;
  QWidget *pvCustomWidget;
  mdtBlinkLed *ldState;
  QPushButton *pbDetails;
  QLabel *lbMessage;
  QLabel *lbTx;
  QLabel *lbRx;
  mdtBlinkLed *ldTx;
  mdtBlinkLed *ldRx;
  // Status texts
  QString pvReadyText;
  QString pvDisconnectedText;
  QString pvConnectingText;
  QString pvBusyText;
  QString pvCurrentStateText;
  QTimer *pvBackToStateTextTimer;
  bool pvShowingMessage;  // Set by showMessage()
  // Status colors
  mdtLed::color_t pvReadyColor;
  mdtLed::color_t pvConnectingColor;
  mdtLed::color_t pvBusyColor;
  // Used by disableTxRxLeds()
  mdtPortThread *pvTxThread;
  mdtPortThread *pvRxThread;
};

#endif  // #ifndef MDT_DEVICE_STATUS_WIDGET_H
