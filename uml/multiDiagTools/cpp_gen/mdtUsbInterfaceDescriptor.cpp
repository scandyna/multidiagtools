
#include "mdtUsbInterfaceDescriptor.h"

mdtUsbInterfaceDescriptor::mdtUsbInterfaceDescriptor() {
}

mdtUsbInterfaceDescriptor::~mdtUsbInterfaceDescriptor() {
}

void mdtUsbInterfaceDescriptor::fetchAttributes(const libusb_interface_descriptor & descriptor) {
}

uint8 mdtUsbInterfaceDescriptor::bDescriptorType() {
}

uint8 mdtUsbInterfaceDescriptor::bInterfaceClass() {
}

uint8 mdtUsbInterfaceDescriptor::bInterfaceSubClass() {
}

uint8 mdtUsbInterfaceDescriptor::bInterfaceProtocol() {
}

uint8 mdtUsbInterfaceDescriptor::iInterface() {
}

QList<mdtUsbEndPoint*> mdtUsbInterfaceDescriptor::endpoints() {
}

