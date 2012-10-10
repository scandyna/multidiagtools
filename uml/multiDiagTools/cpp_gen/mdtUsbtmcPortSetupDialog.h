#ifndef _MDTUSBTMCPORTSETUPDIALOG_H
#define _MDTUSBTMCPORTSETUPDIALOG_H


class mdtUsbtmcPortSetupDialog : public QDialog, public Ui_mdtUsbtmcPortSetupDialog {
  public:
    mdtUsbtmcPortSetupDialog(const QWidget & parent);

    ~mdtUsbtmcPortSetupDialog();

    void setPortManager(const mdtUsbtmcPortManager & manager);

    void setStateRunning();

    void setStateError();

    void diseableApplyButtons();

    void enableApplyButtons();

    void displayConfig();

    void updateConfig();

    void applySetup();


  private:
    mdtUsbtmcPortManager * pvPortManager;

    mdtPortConfigWidget pvPortConfigWidget;

};
#endif
