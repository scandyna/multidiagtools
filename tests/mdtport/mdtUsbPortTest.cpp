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
#include "mdtUsbPortTest.h"
#include "mdtApplication.h"
#include "mdtUsbPort.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbPortThread.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameCodecK8055.h"
#include "mdtFrameUsbTmc.h"

#include <QDebug>

void mdtUsbPortTest::transfertCallback(libusb_transfer *transfer)
{
  qDebug() << "Tranfert callback ...";
  int *completed = (int*)transfer->user_data;
  *completed = 1;
}

void mdtUsbPortTest::essais()
{
  mdtUsbPort port;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbConfigDescriptor *configDescriptor;
  mdtUsbInterfaceDescriptor *ifaceDescriptor;
  mdtUsbEndpointDescriptor *endpointDescriptor;
  mdtUsbPortThread thd;
  mdtPortConfig cfg;
  mdtFrame *f;
  mdtFrameUsbTmc *uf;
  mdtFrameCodecK8055 codec;

  int retVal;
  uint16_t searchedVid = 0x0957;
  ///uint16_t searchedVid = 0x10cf;
  unsigned char data[5000];
  int written = 0;
  int len = 0;

  libusb_context *ctx;

  retVal = libusb_init(&ctx);
  if(retVal != 0){
    qDebug() << "Init failed, code: " << retVal;
    return;
  }

  // Scan ...
  libusb_device **list;
  libusb_device_handle *handle;
  ssize_t i = 0;
  int n, o, p;
  ssize_t cnt = libusb_get_device_list(0, &list);
  if(cnt < 0){
    qDebug() << "libusb_get_device_list() failed";
    libusb_exit(ctx);
    return;
  }
  for(i=0; i<cnt; i++){
    // Device
    libusb_device *dev = list[i];
    qDebug() << "Found a device:";
    qDebug() << "-> Bus number: " << libusb_get_bus_number(dev);
    qDebug() << "-> Device address: " << libusb_get_device_address(dev);
    //qDebug() << "-> Device speed: " <<  libusb_get_device_speed(dev);
    // Device descriptor
    struct libusb_device_descriptor desc;
    retVal = libusb_get_device_descriptor(dev, &desc);
    if(retVal < 0){
      qDebug() << "->  Failed to get descriptor";
      continue;
    }
    // Get attributes
    QVERIFY(deviceDescriptor.fetchAttributes(dev, true) == 0);
    qDebug() << "->  IDV: " << hex << deviceDescriptor.idVendor() << " , IDP: " << deviceDescriptor.idProduct() << dec;
    if(deviceDescriptor.idVendor() == searchedVid){
      qDebug() << "->  Found IDV !";
    }
    qDebug() << "->  Class: " << deviceDescriptor.bDeviceClass() << " , Subclass: " << deviceDescriptor.bDeviceSubClass();
    // Get configurations
    for(n=0; n<deviceDescriptor.configurations().size(); n++){
      configDescriptor = deviceDescriptor.configurations().at(n);
      qDebug() << "->   Configuration type: " << configDescriptor->bDescriptorType();
      qDebug() << "->   Self powered      : " << configDescriptor->isSelfPowered();
      qDebug() << "->   Imax [mA]         : " << configDescriptor->maxPower();
      // Get interfaces
      for(o=0; o<configDescriptor->interfaces().size(); o++){
        ifaceDescriptor = configDescriptor->interfaces().at(o);
        qDebug() << "->   Interface " << o;
        qDebug() << "->    Type    : " << ifaceDescriptor->bDescriptorType();
        qDebug() << "->    Class   : " << ifaceDescriptor->bInterfaceClass();
        qDebug() << "->    Subclass: " << ifaceDescriptor->bInterfaceSubClass();
        qDebug() << "->    Protocol: " << ifaceDescriptor->bInterfaceProtocol();
        // Get endpoints
        for(p=0; p<ifaceDescriptor->endpoints().size(); p++){
          endpointDescriptor = ifaceDescriptor->endpoints().at(p);
          qDebug() << "->    Endpoint " << p;
          qDebug() << "->     Adrress        : " << endpointDescriptor->address();
          if(endpointDescriptor->isDirectionIN()){
            qDebug() << "->     Direction      : IN";
          }
          if(endpointDescriptor->isDirectionOUT()){
            qDebug() << "->     Direction      : OUT";
          }
          if(endpointDescriptor->isTransfertTypeControl()){
            qDebug() << "->     Transfert type : Control";
          }
          if(endpointDescriptor->isTransfertTypeIsochronus()){
            qDebug() << "->     Transfert type : Isochronous";
          }
          if(endpointDescriptor->isTransfertTypeBulk()){
            qDebug() << "->     Transfert type : Bulk";
          }
          if(endpointDescriptor->isTransfertTypeInterrupt()){
            qDebug() << "->     Transfert type : Interrupt";
          }
          if(endpointDescriptor->isAsynchronous()){
            qDebug() << "->     Sync           : Asynch";
          }
          if(endpointDescriptor->isAdaptative()){
            qDebug() << "->     Sync           : Adaptative";
          }
          if(endpointDescriptor->isSynchronous()){
            qDebug() << "->     Sync           : Synch";
          }
          if(endpointDescriptor->isDataEndpoint()){
            qDebug() << "->     Usage type     : Data";
          }
          if(endpointDescriptor->isFeedbackEndpoint()){
            qDebug() << "->     Usage type     : Feedback";
          }
          if(endpointDescriptor->isImplicitFeedbackEndpoint()){
            qDebug() << "->     Usage type     : Implicit feedback";
          }
          qDebug() << "->     Max packet size: " << endpointDescriptor->maxPacketSize();
          qDebug() << "->     Transact cnt/fr: " << endpointDescriptor->transactionsCountPerMicroFrame();
          qDebug() << "->     bInterval      : " << endpointDescriptor->bInterval();
        }
      }
    }
  }
  libusb_free_device_list(list, 1);

  // Setup
  cfg.setReadQueueSize(500);
  cfg.setFrameType(mdtFrame::FT_USBTMC);
  port.setConfig(&cfg);
  ///port.setPortName("0x0957:0x4d18");
  port.setPortName("0x10cf:0x5500");
  thd.setPort(&port);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Start thread
  QVERIFY(thd.start());

  // USBTMC --- Send a message
  /**
  QByteArray msg;
  
  msg = "*IDN?";
  msg.append(0x0D);
  
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(1);
  uf->setMessageData("*IDN?");
  uf->encode();
  for(int i=0; i<uf->size(); i++){
    qDebug() << "uf[" << i << "]: " << hex << (quint8)uf->at(i) << " , char: " << dec << (char)uf->at(i);
  }
  port.writeFrames().enqueue(uf);
  port.unlockMutex();
  QTest::qWait(1000);
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  uf->setbTag(2);
  uf->setMessageData("");
  uf->encode();
  for(int i=0; i<uf->size(); i++){
    qDebug() << "uf[" << i << "]: " << hex << (quint8)uf->at(i) << " , char: " << dec << (char)uf->at(i);
  }
  port.writeFrames().enqueue(uf);
  port.unlockMutex();
  QVERIFY(rThd.start());
  QTest::qWait(1000);
  // Read ...
  port.lockMutex();
  f = port.readenFrames().dequeue();
  QVERIFY(f != 0);
  for(int i=0; i<f->size(); i++){
    qDebug() << "f[" << i << "]: " << f->at(i);
  }
  port.unlockMutex();
  QTest::qWait(3000);
  */

  // k8055 tests
  for(int q=0; q<10; q++){
    // Send some data
    codec.setDigitalOut((q%7)+1, true);
    codec.setDigitalOut((q+2)%7+1, true);
    port.lockMutex();
    f = port.writeFramesPool().dequeue();
    QVERIFY(f != 0);
    f->clear();
    f->append(codec.encodeSetOutputs());
    ///port.writeFrames().enqueue(f);
    port.unlockMutex();
    port.addFrameToWrite(f);
    QTest::qWait(100);
    
    // See if something was readen
    QTest::qWait(100);
    port.lockMutex();
    len = port.readenFrames().size();
    for(int i=0; i<len; i++){
      f = port.readenFrames().dequeue();
      QVERIFY(f != 0);
      ///qDebug() << "Frame[0]: " << (int)f->at(0);
      QVERIFY(codec.decode(*f));
      ///qDebug() << "Frame[0]: " << (int)f->at(0);
      for(int i=0; i<codec.values().size(); i++){
        qDebug() << "Value[" << i << "]: " << codec.values().at(i);
      }
      port.readFramesPool().enqueue(f);
    }
    port.unlockMutex();
    QTest::qWait(100);
  }

  qDebug() << "TEST , about to quit";
  thd.stop();
  return;
  
  
  
  
  // Open specific device
  handle = libusb_open_device_with_vid_pid(ctx, 0x0957, 0x4d18);
  if(handle == 0){
    qDebug() << "Open failed";
    libusb_exit(ctx);
    return;
  }
#ifdef Q_OS_UNIX
  if(libusb_kernel_driver_active(handle, 0) == 1){
    retVal = libusb_detach_kernel_driver(handle, 0);
    if(retVal != 0){
      qDebug() << "Cannot detach kernel driver";
      libusb_close(handle);
      libusb_exit(ctx);
      return;
    }
  }
#endif
  retVal = libusb_claim_interface(handle, 0);
  if(retVal != 0){
    qDebug() << "Clam failed, code: " << retVal;
    libusb_close(handle);
    libusb_exit(ctx);
    return;
  }
  
  
  /*
   * Asynch I/O
   */
  struct libusb_transfer *transfer;
  int completed = 0;
  
  // Write ...
  data[0] = 1;
  data[1] = 1;
  data[2] = 0xFE;
  data[3] = 0;
  data[4] = 6;
  data[5] = 0;
  data[6] = 0;
  data[7] = 0;
  data[8] = 1;
  data[9] = 0;
  data[10] = 0;
  data[11] = 0;
  data[12] = '*';
  data[13] = 'I';
  data[14] = 'D';
  data[15] = 'N';
  data[16] = '?';
  data[17] = '\n';
  data[18] = 0;
  data[19] = 0;
  len = 20;
      
  /*
  data[0] = 5;  // CMD
  data[1] = 7;  // Image dig out
  len = 2;
  */
  /*
  data[2] = 'D';
  data[3] = 'N';
  data[4] = '?';
  data[5] = '\n';
  len = 6;
  */

  completed = 0;
  transfer = libusb_alloc_transfer(0);
  if(transfer == 0){
    qDebug() << "Transfert alloc failed";
    libusb_close(handle);
    libusb_exit(ctx);
    return;
  }
  libusb_fill_bulk_transfer(transfer, handle, 0x01, data, len, transfertCallback, &completed, 0);
  /// NOTE: transfer->type ??
  retVal = libusb_submit_transfer(transfer);
  if(retVal != 0){
    qDebug() << "Transfer submit failed";
    libusb_free_transfer(transfer);
    libusb_close(handle);
    libusb_exit(ctx);
    return;
  }
  while(!completed){
    ///retVal = libusb_handle_events_completed(ctx, &completed); NOTE: dès v1.0.9
    retVal = libusb_handle_events(ctx);
    if(retVal != 0){
      qDebug() << "Handle events failed";
      libusb_free_transfer(transfer);
      libusb_close(handle);
      libusb_exit(ctx);
      return;
    }
  }
  libusb_free_transfer(transfer);

  /*
  retVal = libusb_bulk_transfer(handle, 1, data, len, &written, 1000);
  if(retVal != 0){
    qDebug() << "Write failed, code: " << retVal;
  }
  */
  ///usleep(100000);
  // Read ...
  data[0] = 2;
  data[1] = 2;
  data[2] = 0xFD;
  data[3] = 0;
  data[4] = 0x64;
  data[5] = 0;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 0;
  data[10] = 0;
  data[11] = 0;
  len = 12;
  retVal = libusb_bulk_transfer(handle, 1, data, len, &written, 1000);
  if(retVal != 0){
    qDebug() << "Write failed, code: " << retVal;
  }
  ///usleep(100000);
  // Read
  len = 1024;
  /*
  retVal = libusb_bulk_transfer(handle, 0x82, data, len, &written, 0);
  if(retVal != 0){
    qDebug() << "Read failed, code: " << retVal;
  }else{
    qDebug() << "Readen " << written << " bytes: ";
    for(int i=0; i<written; i++){
      qDebug() << "data[" << i << "]: " << data[i] << " char: " << (char)data[i];
    }
  }
  */
  // Read
  completed = 0;
  transfer = libusb_alloc_transfer(0);
  if(transfer == 0){
    qDebug() << "Transfert alloc failed";
    libusb_close(handle);
    libusb_exit(ctx);
    return;
  }
  libusb_fill_bulk_transfer(transfer, handle, 0x82, data, len, transfertCallback, &completed, 0);
  /// NOTE: transfer->type ??
  retVal = libusb_submit_transfer(transfer);
  if(retVal != 0){
    qDebug() << "Transfer submit failed";
    libusb_free_transfer(transfer);
    libusb_close(handle);
    libusb_exit(ctx);
    return;
  }
  while(!completed){
    qDebug() << "Handle events ...";
    ///retVal = libusb_handle_events_completed(ctx, &completed); NOTE: dès v1.0.9
    retVal = libusb_handle_events(ctx);
    if(retVal != 0){
      qDebug() << "Handle events failed";
      libusb_free_transfer(transfer);
      libusb_close(handle);
      libusb_exit(ctx);
      return;
    }
  }
  written = transfer->actual_length;
  for(int i=0; i<written; i++){
    qDebug() << "data[" << i << "]: " << data[i] << " char: " << (char)data[i];
  }

  libusb_free_transfer(transfer);

  /*
  len = 4096;
  retVal = libusb_bulk_transfer(handle, 0x82, data, len, &written, 0);
  if(retVal != 0){
    qDebug() << "Read failed, code: " << retVal;
  }else{
    qDebug() << "Readen " << written << " bytes: ";
    for(int i=0; i<written; i++){
      qDebug() << "data[" << i << "]: " << data[i] << " char: " << (char)data[i];
    }
  }
  */
  // Release, close, ...
  retVal = libusb_release_interface(handle, 0);
  if(retVal != 0){
    qDebug() << "Release failed, code: " << retVal;
  }
  libusb_close(handle);

  
  libusb_exit(ctx);
}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtUsbPortTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
