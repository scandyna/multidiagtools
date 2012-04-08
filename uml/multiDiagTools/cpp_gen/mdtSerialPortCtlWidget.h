#ifndef _MDTSERIALPORTCTLWIDGET_H
#define _MDTSERIALPORTCTLWIDGET_H


class mdtSerialPortCtlWidget : public QWidget, public Ui_mdtSerialPortCtlWidget {
  public:
    mdtSerialPortCtlWidget(const QWidget & parent);

    mdtLed ;


  private:
    mdtLed ldCar;

    mdtLed ldRng;

    mdtLed ldDsr;

    mdtLed ldCts;

    mdtLed ldTx;

    mdtLed ldRx;

};
#endif
