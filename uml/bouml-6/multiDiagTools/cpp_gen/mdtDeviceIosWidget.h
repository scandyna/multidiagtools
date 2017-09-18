#ifndef _MDTDEVICEIOSWIDGET_H
#define _MDTDEVICEIOSWIDGET_H


class mdtDeviceIos;
class mdtAnalogInWidget;
class mdtDigitalInWidget;
class mdtAnalogOutWidget;
class mdtDigitalOutWidget;

class mdtDeviceIosWidget : public QWidget {
  public:
    mdtDeviceIosWidget(const QWidget & parent);

    ~mdtDeviceIosWidget();

    void setMaxColumns(int n);

    void setDeviceIos(const mdtDeviceIos & ios);


  private:
    <mdtAnalogInWidget> pvAnalogInputs;

    <mdtDigitalInWidget> pvDigitalInputs;

    int pvMaxColumns;

    QGridLayout pvLayout;

    <mdtAnalogOutWidget> pvAnalogOutputs;

    <mdtDigitalOutWidget> pvDigitalOutputs;


  protected:
    mdtDeviceIos * pvIos;

};
#endif
