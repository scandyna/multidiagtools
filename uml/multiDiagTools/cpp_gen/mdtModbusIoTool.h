#ifndef _MDTMODBUSIOTOOL_H
#define _MDTMODBUSIOTOOL_H


#include "mdtDeviceModbusWago.h"
#include "mdtDeviceIosWidget.h"

class mdtModbusIoTool : public QMainWindow, public ui_mdtModbusIoTool {
  private:
    mdtDeviceModbusWago pvDeviceModbusWago;

    mdtDeviceIosWidget pvDeviceIosWidget;


  public:
    mdtModbusIoTool(const QWidget & parent, const QT::WindowFlags & flags);

    ~mdtModbusIoTool();

    void setAvailableTranslations(const QMap<QString,QString> & avaliableTranslations, const QString & currentTranslationKey);

    void retranslate();


  private:
    void setStateFromPortManager(int state);

    void setStateDisconnected();

    void setStateConnecting();

    void setStateReady();

    void setStateBusy();

    void setStateWarning();

    void setStateError();


  public:
    void showStatusMessage(const QString & message, int timeout);

    void showStatusMessage(const QString & message, const QString & details, int timeout);


  private:
    bool pvReady;

};
#endif
