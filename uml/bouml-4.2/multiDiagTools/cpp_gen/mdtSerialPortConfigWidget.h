#ifndef _MDTSERIALPORTCONFIGWIDGET_H
#define _MDTSERIALPORTCONFIGWIDGET_H


class mdtSerialPortConfigWidget : public QWidget, public ui_mdtSerialPortConfigWidget {
  public:
    mdtSerialPortConfigWidget(const QWidget & parent);

    void displayConfig(const mdtSerialPortConfig & config);

    void updateConfig(const mdtSerialPortConfig & config);

};
#endif
