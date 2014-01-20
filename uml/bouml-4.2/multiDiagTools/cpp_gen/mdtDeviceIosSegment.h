#ifndef _MDTDEVICEIOSSEGMENT_H
#define _MDTDEVICEIOSSEGMENT_H


class mdtAbstractIo;

class mdtDeviceIosSegment {
  private:
    int pvStartAddress;

    int pvEndAddress;

    QList<mdtValue> pvValues;


  public:
    mdtDeviceIosSegment();

    ~mdtDeviceIosSegment();

    void setIos(const QList<mdtAbstractIo> & ios);

    int startAddress() const;

    int endAddress() const;

    int ioCount();

    QList<int> addresses() const;

    bool setValues(const QList<mdtValue> & values);

    bool setValues(const QVariant & values);

const    QList<mdtValue> values() const;

    QList<int> valuesInt() const;

    QList<double> valuesDouble() const;

    QList<bool> valuesBool() const;

};
#endif
