#include <QObject>
#include <QVariant>
#include <QString>
#include <QDebug>

class MySpy : public QObject
{
  Q_OBJECT
public:
  MySpy(QObject *parent = nullptr)
    : QObject(parent) {}
public slots:
  void display(const QString & v);
  void display(const int & v);
};
