#ifndef _MDTUSBDEVICEDESCRIPTOR_H
#define _MDTUSBDEVICEDESCRIPTOR_H


class mdtUsbDeviceDescriptor {
  private:
    <mdtUsbConfigDescriptor> pvConfigs;

    uint8 pvbDescriptorType;

    uint16 pvbcdUSB;

    uint8 pvbDeviceClass;

    uint8 pvbDeviceSubClass;

    uint8 pvbDeviceProtocol;

    uint8 pvbMaxPacketSize;

    uint16 pvidVendor;

    uint16 pvidProduct;

    uint16 pvbcdDevice;

    uint8 pviManufactuer;

    uint8 pviProduct;

    uint8 pviSerialNumber;


  public:
    bool fetchAttributes(const libusb_device & device);

    uint8 bDescriptorType();

    uint16 bcdUSB();

    uint8 bDeviceClass();

    uint8 bDeviceSubClass();

    uint8 bDeviceProtocol();

    uint8 bMaxPacketSize();

    uint16 idVendor();

    uint16 idProduct();

    uint16 bcdDevice();

    QList<mdtUsbConfigDescriptor*> configurations();

};
#endif
