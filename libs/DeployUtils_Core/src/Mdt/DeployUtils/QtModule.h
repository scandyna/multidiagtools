/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_H
#define MDT_DEPLOY_UTILS_QT_MODULE_H

#include <QMetaType>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Enumeration of known Qt modules
   */
  enum class QtModule
  {
    Unknown,            /*!< Not a Qt module */
    Core,               /*!< Qt Core */
    Gui,                /*!< Qt GUI */
    Multimedia,         /*!< Qt Multimedia */
    MultimediaWidgets,  /*!< Qt Multimedia Widgets */
    Network,            /*!< Qt Network */
    Positioning,        /*!< Qt Positioning */
    PrintSupport,       /*!< Qt Print Support */
    Qml,                /*!< Qt QML */
    Quick,              /*!< Qt Quick */
    Sensors,            /*!< Qt Sensors */
    Sql,                /*!< Qt SQL */
    Svg,                /*!< Qt SVG */
    Test,               /*!< Qt Test */
    Widgets,            /*!< Qt Widgets */
    DBus                /*!< Qt D-BUS */
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::QtModule)

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_H
