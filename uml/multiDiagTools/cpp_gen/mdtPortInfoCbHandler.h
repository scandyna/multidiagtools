#ifndef _MDTPORTINFOCBHANDLER_H
#define _MDTPORTINFOCBHANDLER_H


#include "mdtPortInfo.h"
#include "mdtDeviceInfo.h"

class mdtPortInfoCbHandler : public QObject {
  private:
    QComboBox * cbPorts;

    QComboBox * cbDevices;


  public:
    mdtPortInfoCbHandler(const QObject & parent);

    ~mdtPortInfoCbHandler();

    void setPortsComboBox(const QComboBox & comboBox);

    void setDevicesComboBox(const QComboBox & comboBox);

    void fillComboBoxes(const mdtPortInfo & portInfoList);

    mdtPortInfo currentPortInfo();

    mdtDeviceInfo currentDeviceInfo();


  private:
    void updateDevicesCb(int portsCbIndex);

};
#endif
