#ifndef _MDTDEVICESTATUSWIDGET_H
#define _MDTDEVICESTATUSWIDGET_H


class mdtDeviceStatusWidget : public QWidget {
  public:
    mdtDeviceStatusWidget(const QWidget & parent);

    ~mdtDeviceStatusWidget();

    void setState(const state_t & state);

    void setState(const state_t & state, const QString & message, const QString & details);


  private:
    mdtBlinkLed ldState;

    QPushButton pbDetails;

    QLabel lbMessage;

    QLabel lbTx;

    QLabel lbRx;

    mdtBlinkLed ldTx;

    mdtBlinkLed ldRx;

};
#endif
