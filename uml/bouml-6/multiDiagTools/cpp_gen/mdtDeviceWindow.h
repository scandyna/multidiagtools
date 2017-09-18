#ifndef _MDTDEVICEWINDOW_H
#define _MDTDEVICEWINDOW_H


class mdtDeviceWindow : public Ui::mdtDeviceWindow, public QMainWindow {
  public:
    mdtDeviceWindow(const QWidget & parent);

    ~mdtDeviceWindow();


  private:
    mdtDevice * ;

};
#endif
