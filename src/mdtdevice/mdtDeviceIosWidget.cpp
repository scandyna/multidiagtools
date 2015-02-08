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
#include "mdtDeviceIosWidget.h"
#include "mdtDeviceIos.h"
#include "mdtAnalogInWidget.h"
#include "mdtDigitalInWidget.h"
#include "mdtAnalogOutWidget.h"
#include "mdtDigitalOutWidget.h"
#include <QGridLayout>

mdtDeviceIosWidget::mdtDeviceIosWidget(QWidget *parent)
 : QWidget(parent)
{
  pvMaxColumns = 10;
  pvLayout = new QGridLayout;

  setLayout(pvLayout);
}

mdtDeviceIosWidget::~mdtDeviceIosWidget()
{
}

void mdtDeviceIosWidget::setMaxColumns(int n)
{
  Q_ASSERT(n > 0);

  pvMaxColumns = n;
}

void mdtDeviceIosWidget::setDeviceIos(std::shared_ptr<mdtDeviceIos> ios)
{
  Q_ASSERT(ios);

  int i;
  int row, col;
  mdtAnalogInWidget *aiw;
  mdtAnalogOutWidget *aow;
  mdtDigitalInWidget *diw;
  mdtDigitalOutWidget *dow;
  QList<mdtAnalogIo*> analogInputs;
  QList<mdtAnalogIo*> analogOutputs;
  QList<mdtDigitalIo*> digitalInputs;
  QList<mdtDigitalIo*> digitalOutputs;

  // Remove existing widgets
  clearIoWidgets();

  pvIos = ios;
  row = 0;
  col = 0;

  // Add analog inputs
  analogInputs = pvIos->analogInputs();
  for(i=0; i<analogInputs.size(); i++){
    if(col >= pvMaxColumns){
      row++;
      col = 0;
    }
    aiw = new mdtAnalogInWidget;
    aiw->setIo(analogInputs.at(i));
    pvAnalogInputs.append(aiw);
    pvLayout->addWidget(aiw, row, col, 2, 1);
    col++;
  }
  // Add analog outputs
  analogOutputs = pvIos->analogOutputs();
  for(i=0; i<analogOutputs.size(); i++){
    if(col >= pvMaxColumns){
      row++;
      col = 0;
    }
    aow = new mdtAnalogOutWidget;
    aow->setIo(analogOutputs.at(i));
    pvAnalogOutputs.append(aow);
    pvLayout->addWidget(aow, row, col, 2, 1);
    col++;
  }
  // Add digital inputs
  digitalInputs = pvIos->digitalInputs();
  for(i=0; i<digitalInputs.size(); i++){
    if(col >= pvMaxColumns){
      row++;
      col = 0;
    }
    diw = new mdtDigitalInWidget;
    diw->setIo(digitalInputs.at(i));
    pvDigitalInputs.append(diw);
    // If we have minimum a analog I/O, we place 2 digital I/O's verticaly
    if((pvAnalogInputs.size()>0)||(pvAnalogOutputs.size()>0)){
      if((pvLayout->itemAtPosition(row, col))==0){
        pvLayout->addWidget(diw, row, col);
      }else{
        pvLayout->addWidget(diw, row+1, col);
        col++;
      }
    }else{
      pvLayout->addWidget(diw, row, col);
      col++;
    }
  }
  // Add digital outputs
  digitalOutputs = pvIos->digitalOutputs();
  for(i=0; i<digitalOutputs.size(); i++){
    if(col >= pvMaxColumns){
      row++;
      col = 0;
    }
    dow = new mdtDigitalOutWidget;
    dow->setIo(digitalOutputs.at(i));
    pvDigitalOutputs.append(dow);
    // If we have minimum a analog I/O, we place 2 digital I/O's verticaly
    if((pvAnalogInputs.size()>0)||(pvAnalogOutputs.size()>0)){
      if((pvLayout->itemAtPosition(row, col))==0){
        pvLayout->addWidget(dow, row, col);
      }else{
        pvLayout->addWidget(dow, row+1, col);
        col++;
      }
    }else{
      pvLayout->addWidget(dow, row, col);
      col++;
    }
  }

}

void mdtDeviceIosWidget::clearIoWidgets()
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); i++){
    pvLayout->removeWidget(pvAnalogInputs.at(i));
    delete pvAnalogInputs.at(i);
  }
  pvAnalogInputs.clear();
  for(i=0; i<pvAnalogOutputs.size(); i++){
    pvLayout->removeWidget(pvAnalogOutputs.at(i));
    delete pvAnalogOutputs.at(i);
  }
  pvAnalogOutputs.clear();
  for(i=0; i<pvDigitalInputs.size(); i++){
    pvLayout->removeWidget(pvDigitalInputs.at(i));
    delete pvDigitalInputs.at(i);
  }
  pvDigitalInputs.clear();
  for(i=0; i<pvDigitalOutputs.size(); i++){
    pvLayout->removeWidget(pvDigitalOutputs.at(i));
    delete pvDigitalOutputs.at(i);
  }
  pvDigitalOutputs.clear();
}
