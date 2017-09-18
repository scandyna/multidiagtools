#ifndef _MDTDEVICESTATUSWIDGET_H
#define _MDTDEVICESTATUSWIDGET_H


class mdtPortStatusWidget : public QWidget {
  public:
    mdtPortStatusWidget(const QWidget & parent);

    ~mdtPortStatusWidget();

    void setState(const state_t & state);

    void setState(int state, int stateText, int ledColor);

    void showMessage(const QString & message, const QString & details, int timeout);


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
