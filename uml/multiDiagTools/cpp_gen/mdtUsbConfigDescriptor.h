#ifndef _MDTUSBCONFIGDESCRIPTOR_H
#define _MDTUSBCONFIGDESCRIPTOR_H


class mdtUsbConfigDescriptor {
  private:
    <mdtUsbInterfaceDescriptor> pvInterfaces;

    uint8 pvbDescriptorType;

    uint16 pvwTotalLength;

    uint8 pvbConfigurationValue;

    uint8 pviConfiguration;

    uint8 pvbmAttributes;

    uint8 pvbMaxPower;


  public:
    mdtUsbConfigDescriptor();

    ~mdtUsbConfigDescriptor();

    uint8 bDescriptorType();

    uint16 wTotalLength();

    uint8 bConfigurationValue();

    uint8 iConfiguration();

    uint8 bmAttributes();

    uint8 bMaxPower();

    QList<mdtUsbInterfaceDescriptor*> interfaces();

};
#endif
