/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_STRING_RECORD_LIST_H
#define MDT_PLAIN_TEXT_STRING_RECORD_LIST_H

#include "RecordListTemplate.h"
#include "StringRecord.h"
#include <QString>
#include <QMetaType>

namespace Mdt{ namespace PlainText{

  /*! \brief StringRecordList is a list of StringRecord
   */
  using StringRecordList = RecordListTemplate<StringRecord, QString>;

}} // namespace Mdt{ namespace PlainText{
Q_DECLARE_METATYPE(Mdt::PlainText::StringRecordList)

#endif // #ifndef MDT_PLAIN_TEXT_STRING_RECORD_LIST_H
