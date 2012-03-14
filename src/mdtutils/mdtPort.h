#ifndef MDT_PORT_H
#define MDT_PORT_H

#include <QObject>
#include <QString>

#ifdef Q_OS_UNIX
 #include "linux/mdtPortPosix.h"
 #define mdtPortSys mdtPortPosix
#endif

class mdtPort : public mdtPortSys
{
 Q_OBJECT

 public:

  mdtPort(QObject *parent = 0);
  ~mdtPort();
};

#endif  // #ifndef MDT_PORT_H
