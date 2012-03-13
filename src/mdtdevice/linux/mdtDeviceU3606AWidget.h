#ifndef MDT_DEVICE_U3606A_WIDGET_H
#define MDT_DEVICE_U3606A_WIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include "ui_mdtDeviceU3606AWidget.h"
#include "mdtDeviceU3606A.h"

class mdtDeviceU3606AWidget : public QWidget , private Ui::mdtDeviceU3606AWidget
{
 Q_OBJECT

 public:

  mdtDeviceU3606AWidget(QWidget *parent = 0);

 public slots:

  void essais();
  
  void scan();
  
  void selectPort(const QString &port);

  void displayIdn(const QStringList &data);
  
 private:

  // Set IDN to unknow
  void setDefaulIdn();
  
  mdtDeviceU3606A pvDevice;
};

#endif  // #ifndef MDT_DEVICE_U3606A_WIDGET_H
