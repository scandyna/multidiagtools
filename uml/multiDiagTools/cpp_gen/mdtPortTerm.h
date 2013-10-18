#ifndef _MDTPORTTERM_H
#define _MDTPORTTERM_H


class mdtPortTerm : public QWidget, public ui_mdtPortTerm {
  public:
    mdtPortTerm(const QWidget & parent);

    ~mdtPortTerm();


  private:
    <mdtSerialPortManager> pvSerialPortManager;

    <mdtSerialPortCtlWidget> pvSerialPortCtlWidget;


  public:
    void appendReadenData();

    void sendCmd();

    void on_pbSendCmdAbort_clicked();

    void on_pbClearTerm_clicked();


  private:
    void attachToSerialPort();

    void detachFromSerialPort();

    mdtPortTerm(const mdtPortTerm & other);

    mdtPortManager * pvCurrentPortManager;

};
#endif
