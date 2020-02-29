/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_RAILWAY_TASK_LIST_TEST_LIB_RESPONSE_HANDLER_H
#define MDT_RAILWAY_TASK_LIST_TEST_LIB_RESPONSE_HANDLER_H

#include <QObject>
#include <vector>

namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{

  /*! \internal
   */
  template<typename Emitter, typename Record>
  class ResponseHandler : public QObject
  {
   public:

    template<typename Signal>
    void connectEmitter(Emitter & emitter, Signal sig)
    {
      connect(&emitter, sig, this, &ResponseHandler::addRecord);
    }

    int recordCount() const
    {
      return mList.size();
    }

    const Record & recordAt(int i) const
    {
      Q_ASSERT(i >= 0);
      Q_ASSERT(i < recordCount());

      return mList[i];
    }

    void addRecord(const Record & record)
    {
      mList.push_back(record);
    }

   private:

    std::vector<Record> mList;
  };

}}}} // namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{

#endif // #ifndef MDT_RAILWAY_TASK_LIST_TEST_LIB_RESPONSE_HANDLER_H
