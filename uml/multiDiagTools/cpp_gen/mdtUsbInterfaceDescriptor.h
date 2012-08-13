#ifndef _MDTUSBINTERFACEDESCRIPTOR_H
#define _MDTUSBINTERFACEDESCRIPTOR_H


class mdtUsbInterfaceDescriptor {
  private:
    <mdtUsbEndointDescriptor> pvEndpoints;

    uint8 pvbDescriptorType;

    uint8 pvbInterfaceClass;

    uint8 pvbInterfaceSubClass;

    uint8 pvbInterfaceProtocol;

    uint8 pviInterface;


  public:
    mdtUsbInterfaceDescriptor();

    ~mdtUsbInterfaceDescriptor();

    void fetchAttributes(const libusb_interface_descriptor & descriptor);

    uint8 bDescriptorType();

    uint8 bInterfaceClass();

    uint8 bInterfaceSubClass();

    uint8 bInterfaceProtocol();

    uint8 iInterface();

    QList<mdtUsbEndPoint*> endpoints();

};
#endif
