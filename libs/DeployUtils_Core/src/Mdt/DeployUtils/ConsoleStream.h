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
#ifndef MDT_DEPLOY_UTILS_CONSOLE_STREAM_H
#define MDT_DEPLOY_UTILS_CONSOLE_STREAM_H

#include <QtGlobal>
#include <QString>
#include <memory>

class QDebug;

namespace Mdt{ namespace DeployUtils{

  /*! \brief
   */
  class ConsoleStream
  {
   public:

    /*! \brief Construct a stream
     */
    ConsoleStream(QtMsgType msgType, int minLevel, int level);

    ConsoleStream & operator<<(const QString & str);
//     ConsoleStream & operator<<(const QDebug & dbg, const QString & str);

   private:

//     int mMinLevel;
//     int mLevel;
    std::shared_ptr<QDebug> mDbg;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_CONSOLE_STREAM_H
