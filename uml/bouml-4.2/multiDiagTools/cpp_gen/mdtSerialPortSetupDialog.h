#ifndef _MDTSERIALPORTSETUPDIALOG_H
#define _MDTSERIALPORTSETUPDIALOG_H


class mdtSerialPortSetupDialog : public ui_mdtSerialPortSetupDialog, public QDialog {
  public:
    mdtSerialPortSetupDialog(const QWidget & parent);


  private:
    mdtSerialPortManager * pvPortManager;


  public:
    void setPortManager(const mdtSerialPortManager & manager);


  private:
    ui_mdtPortConfigWidget * ;

    ui_mdtSerialPortConfigWidget * ;

    void displayConfig();

    void updateConfig();

    void applySetup();

};
#endif
