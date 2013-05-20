/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtSerialPortConfigWidget.h"
#include <QString>

//#include <QDebug>

mdtSerialPortConfigWidget::mdtSerialPortConfigWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
}

void mdtSerialPortConfigWidget::setAvailableBaudRates(QList<int> &baudRates)
{
  cbBaudRate->clear();
  for(int i=0; i<baudRates.size(); i++){
    cbBaudRate->addItem(QString::number(baudRates.at(i)), baudRates.at(i));
  }
}

void mdtSerialPortConfigWidget::displayConfig(mdtSerialPortConfig &config)
{
  int index;

  // Baud rate
  index = cbBaudRate->findData(config.baudRate());
  if(index >= 0){
    cbBaudRate->setCurrentIndex(index);
  }
  // Parity
  switch((int)config.parity()){
    case mdtSerialPortConfig::NoParity:
      rbParityNone->setChecked(true);
      break;
    case mdtSerialPortConfig::ParityEven:
      rbParityEven->setChecked(true);
      break;
    case mdtSerialPortConfig::ParityOdd:
      rbParityOdd->setChecked(true);
      break;
  }
  // Data bits count
  switch(config.dataBitsCount()){
    case 5:
      rbDataBit5->setChecked(true);
      break;
    case 6:
      rbDataBit6->setChecked(true);
      break;
    case 7:
      rbDataBit7->setChecked(true);
      break;
    case 8:
      rbDataBit8->setChecked(true);
      break;
  }
  // Stop bits count
  switch(config.stopBitsCount()){
    case 1:
      rbStopBit1->setChecked(true);
      break;
    case 2:
      rbStopBit2->setChecked(true);
      break;
  }
  // Flow control
  if(config.flowCtlRtsCtsEnabled()){
    rbFlowCtlRtsCts->setChecked(true);
  }else if(config.flowCtlXonXoffEnabled()){
    rbFlowCtlXonXoff->setChecked(true);
  }else{
    rbFlowCtlNone->setChecked(true);
  }
  // Xon char
  if(cbXonChar->count() < 1){
    cbXonChar->addItem("DC1 (0x11)", (char)0x11);
  }
  index = cbXonChar->findData(config.xonChar());
  if(index >= 0){
    cbXonChar->setCurrentIndex(index);
  }
  // Xoff char
  if(cbXoffChar->count() < 1){
    cbXoffChar->addItem("DC3 (0x13)", (char)0x13);
  }
  index = cbXoffChar->findData(config.xoffChar());
  if(index >= 0){
    cbXoffChar->setCurrentIndex(index);
  }
  on_rbFlowCtlXonXoff_toggled(rbFlowCtlXonXoff->isChecked());
}

void mdtSerialPortConfigWidget::updateConfig(mdtSerialPortConfig &config)
{
  int index;
  char xon = 0;
  char xoff = 0;

  // Baud rate
  index = cbBaudRate->currentIndex();
  if(index >= 0){
    config.setBaudRate(cbBaudRate->itemData(index).toInt());
  }
  // Parity
  if(rbParityNone->isChecked()){
    config.setParity(mdtSerialPortConfig::NoParity);
  }else if(rbParityEven->isChecked()){
    config.setParity(mdtSerialPortConfig::ParityEven);
  }else{
    config.setParity(mdtSerialPortConfig::ParityOdd);
  }
  // Data bits count
  if(rbDataBit5->isChecked()){
    config.setDataBitsCount(5);
  }else if(rbDataBit6->isChecked()){
    config.setDataBitsCount(6);
  }else if(rbDataBit7->isChecked()){
    config.setDataBitsCount(7);
  }else{
    config.setDataBitsCount(8);
  }
  // Stop bits count
  if(rbStopBit1->isChecked()){
    config.setStopBitsCount(1);
  }else{
    config.setStopBitsCount(2);
  }
  // Flow control
  if(rbFlowCtlRtsCts->isChecked()){
    config.enableFlowCtlRtsCts();
  }else{
    config.diseableFlowCtlRtsCts();
  }
  if(rbFlowCtlXonXoff->isChecked()){
    // Get Xon char
    index = cbXonChar->currentIndex();
    if(index >= 0){
      xon = cbXonChar->itemData(index).toChar().toAscii();
    }
    // Get Xoff char
    index = cbXoffChar->currentIndex();
    if(index >= 0){
      xoff = cbXoffChar->itemData(index).toChar().toAscii();
    }
    config.enableFlowCtlXonXoff(xon, xoff);
  }else{
    config.diseableFlowCtlXonXoff();
  }
  if(rbFlowCtlNone->isChecked()){
    config.diseableFlowCtlRtsCts();
    config.diseableFlowCtlXonXoff();
  }
  // Redisplay the config, so the user can see what was really setup
  displayConfig(config);
}

void mdtSerialPortConfigWidget::on_rbFlowCtlXonXoff_toggled(bool checked)
{
  if(checked){
    cbXonChar->setEnabled(true);
    cbXoffChar->setEnabled(true);
  }else{
    cbXonChar->setEnabled(false);
    cbXoffChar->setEnabled(false);
  }
}
