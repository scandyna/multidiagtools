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
#include "mdtPortWriteThread.h"
#include <QApplication>

#include <QDebug>

mdtPortWriteThread::mdtPortWriteThread(QObject *parent)
 : mdtPortThread(parent)
{
}

mdtFrame *mdtPortWriteThread::getNewFrame()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Check if there is a frame to transmit
  if(pvPort->writeFrames().size() < 1){
    return 0;
  }
  frame = pvPort->writeFrames().dequeue();
  Q_ASSERT(frame != 0);

  return frame;
}

void mdtPortWriteThread::run()
{
  Q_ASSERT(pvPort != 0);

  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  qint64 toWrite = 0;
  qint64 written = 0;
 
  // Set the running flag
  pvPort->lockMutex();
  pvRunning = true;

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }

    // Wait the minimal time if requierd - Used for timeout protocol and byte per byte write
    if(pvWriteMinWaitTime > 0){
      pvPort->unlockMutex();
      msleep(pvWriteMinWaitTime);
      pvPort->lockMutex();
    }
    // Wait on write ready event
    if(!pvPort->waitEventWriteReady()){
      emit(errorOccured(MDT_PORT_IO_ERROR));
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvPort->writeTimeoutOccured()){
      // Check if we have something to transmit
      if(frame == 0){
        frame = getNewFrame();
        if(frame != 0){
          // New frame to transmit
          bufferCursor = frame->data();
          toWrite = frame->size();
        }
      }
      if(frame != 0){
        // Write data to port
        if((toWrite > 0)&&(pvBytePerByteWrite)){
          written = pvPort->write(bufferCursor, 1);
        }else{
          written = pvPort->write(bufferCursor, toWrite);
        }
        //qDebug() << "WTHD: written: " << written;
        if(written < 0){
          written = 0;
          pvPort->unlockMutex();
          msleep(100);
          pvPort->lockMutex();
          emit(errorOccured(MDT_PORT_IO_ERROR));
        }
        frame->take(written);
        // Check if current frame was completly sent
        if(frame->isEmpty()){
          pvPort->writeFramesPool().enqueue(frame);
          emit frameWritten();
          frame = 0;
        }else{
          bufferCursor = frame->data();
          Q_ASSERT(bufferCursor < (frame->data() + frame->size()));
          toWrite -= written;
          Q_ASSERT(toWrite >= 0);
        }
      }else{
        // Sleep a while
        pvPort->unlockMutex();
        msleep(100);
        pvPort->lockMutex();
      }
    }
  }

  pvPort->unlockMutex();
}
