/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtUsbtmcFrame.h"
#include <QtGlobal>
#include <cstring>

#include <QDebug>

mdtUsbtmcFrame::mdtUsbtmcFrame ( int bufferSize )
{
  Q_ASSERT(bufferSize >= 512);
  Q_ASSERT((bufferSize % 4) == 0);

  pvBuffer.assign(bufferSize, 0);
  pvBufferLength = 12;
}

void mdtUsbtmcFrame::clear()
{
  pvBuffer.assign(12, 0);
  pvBufferLength = 12;
}

void mdtUsbtmcFrame::setTransferSize ( uint32_t size )
{
  Q_ASSERT(size > 0);
  Q_ASSERT(size <= (pvBuffer.capacity()-12));

  pvBuffer[4] = size & 0xFF;
  pvBuffer[5] = (size >> 8) & 0xFF;
  pvBuffer[6] = (size >> 16) & 0xFF;
  pvBuffer[7] = (size >> 24) & 0xFF;
}

void mdtUsbtmcFrame::setData ( const QByteArray & ba )
{
  Q_ASSERT(ba.size() > 0);

  // Ajust buffer length
  pvBufferLength = 12 + ba.size();
  auto abc = pvBufferLength % 4;   // Alignment bytes count
  if(abc != 0){
    abc = 4 - (pvBufferLength % 4);
    pvBufferLength += abc;
  }
  Q_ASSERT((size_t)pvBufferLength <= pvBuffer.capacity());
  // Copy data
  auto *dest = pvBuffer.data() + 12;
  ::memcpy(dest, ba.constData(), ba.size());
  setTransferSize(ba.size());
  // Fill with aligment bytes
  dest += ba.size();
  for(auto i = 0; i < abc; ++i){
    *dest = 0;
    ++dest;
  }
}

QByteArray mdtUsbtmcFrame::data() const
{
  int size = transferSize();
  Q_ASSERT(size >= 0);
  Q_ASSERT(size < (int)(pvBuffer.capacity()-12));
  // Make some ugly casts to tell the compiler that we want simply copy bytes into ba
  auto *p = static_cast<const void*>(pvBuffer.data());
  auto *src = static_cast<const char*>(static_cast<const char*>(p) + 12);
  QByteArray ba(src, size);   // QByteArray makes a copy of passed const char* array
  Q_ASSERT(ba.data() != src);

  return ba;
}
