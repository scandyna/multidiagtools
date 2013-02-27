#ifndef _MDTABSTRACTPORTSETUPDIALOG_H
#define _MDTABSTRACTPORTSETUPDIALOG_H


class mdtAbstractPortSetupDialog : public QDialog, public Ui::mdtAbstractPortSetupDialog {
  public:
    mdtAbstractPortSetupDialog(const QWidget & parent);

    ~mdtAbstractPortSetupDialog();

    void setPortManager(const mdtPortManager & portManager);

    mdtPortManager portManager();

    void setStateFromPortManager(int state);


  protected:
    virtual void setStateDisconnected() = 0;

    virtual void setStateConnecting() = 0;

    virtual void setStateReady() = 0;

    virtual void setStateBusy() = 0;

    virtual void setStateWarning() = 0;

    virtual void setStateError() = 0;

    virtual bool applySetup() = 0;

    void showStatusMessage(const QString & message, int timeout,  );

    void showStatusMessage(const QString & message, const QString & details, int timeout);


  private:
    mdtPortManager * pvPortManager;

    void on_buttonBox_clicked(const QAbstractButton & button);

};
#endif
