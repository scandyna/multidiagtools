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
#ifndef _MDTDEVICESTATUSWIDGET_H
#define _MDTDEVICESTATUSWIDGET_H

#include <QWidget>
#include <QString>
#include "mdtDevice.h"
#include "mdtBlinkLed.h"

class QLabel;
class QPushButton;

/// \todo Add color per state (same as texts)

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

 public slots:

  /*! \brief
   */
  void setState(int state);

  /*! \brief
   */
  void setState(int state, const QString &message, const QString &details);

 private:

  Q_DISABLE_COPY(mdtDeviceStatusWidget);

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
  // Status colors
  mdtLed::color_t pvReadyColor;
  mdtLed::color_t pvConnectingColor;
  mdtLed::color_t pvBusyColor;
};
#endif
