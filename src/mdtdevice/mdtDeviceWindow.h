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
#ifndef MDT_DEVICE_WINDOW_H
#define MDT_DEVICE_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "mdtDevice.h"
#include "ui_mdtDeviceWindow.h"
#include "mdtPortStatusWidget.h"

class QActionGroup;

/*! \brief
 */
class mdtDeviceWindow : public QMainWindow, public Ui::mdtDeviceWindow
{
 Q_OBJECT

 public:

  /*! \brief Construct a device window
   *
   * Will create a new mdtPortStatusWidget and
   *  add it as status bar.
   */
  mdtDeviceWindow(QWidget *parent = 0);

  ~mdtDeviceWindow();

  /*! \brief Set the device
   *
   * If device contains a port manager,
   *  mdtPortManager::stateChanged() and mdtPortManager::statusMessageChanged()
   *  signals are connected to status widget.
   *
   * \pre device must be a valid pointer.
   */
  void setDevice(mdtDevice *device);

  /*! \brief Set the I/O's widget
   */
  void setIosWidget(QWidget *widget);

  /*! \brief Get status widget
   *
   * Usefull for setup
   */
  mdtPortStatusWidget *statusWidget();

  /*! \brief Enable translations
   *
   * Will use mdtApplication to get the list
   *  of available translations and connect
   *  to retranslate slot.
   *
   * \pre The loop must be started with a
   *       mdtApplication instance (mdtApp must be valid).
   */
  void enableTranslations();

  /*! \brief Build the translations menu
   */
  void setAvailableTranslations(QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey);

 protected slots:

  /*! \brief Retranslate
   *
   * This slot is called by mdtApplication
   *  when language has changed
   */
  void retranslate();

 private:

  Q_DISABLE_COPY(mdtDeviceWindow);

  mdtPortStatusWidget *pvStatusWidget;
  mdtDevice *pvDevice;
  // Translations menu
  QActionGroup *pvLanguageActionGroup;
};

#endif  // #ifndef MDT_DEVICE_WINDOW_H
