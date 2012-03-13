#ifndef MDT_DEVICE_H
#define MDT_DEVICE_H

#include <QObject>
#include <QString>
#include <QStringList>

class mdtDevice : public QObject
{
 Q_OBJECT

 public:

  mdtDevice(QObject *parent = 0);
  ~mdtDevice();

};

#endif  // #ifndef MDT_DEVICE_H
