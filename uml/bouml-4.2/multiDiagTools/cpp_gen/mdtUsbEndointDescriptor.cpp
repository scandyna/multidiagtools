
#include "mdtUsbEndointDescriptor.h"

mdtUsbEndointDescriptor::mdtUsbEndointDescriptor() {
}

mdtUsbEndointDescriptor::~mdtUsbEndointDescriptor() {
}

void mdtUsbEndointDescriptor::fetchAttributes(const libusb_endpoint_descriptor & descriptor) {
}

uint8 mdtUsbEndointDescriptor::address() {
}

bool mdtUsbEndointDescriptor::isDirectionIN() {
}

bool mdtUsbEndointDescriptor::isDirectionOUT() {
}

bool mdtUsbEndointDescriptor::isTransfertTypeControl() {
}

bool mdtUsbEndointDescriptor::isTransfertTypeIsochronus() {
}

bool mdtUsbEndointDescriptor::isTransfertTypeBulk() {
}

bool mdtUsbEndointDescriptor::isTransfertTypeInterrupt() {
}

bool mdtUsbEndointDescriptor::isAsynchronous() {
}

bool mdtUsbEndointDescriptor::isAdaptative() {
}

bool mdtUsbEndointDescriptor::isSynchronous() {
}

bool mdtUsbEndointDescriptor::isDataEndpoint() {
}

bool mdtUsbEndointDescriptor::isFeedbackEndpoint() {
}

bool mdtUsbEndointDescriptor::isImplicitFeedbackEndpoint() {
}

int mdtUsbEndointDescriptor::maxPacketSize() {
}

int mdtUsbEndointDescriptor::transactionsCountPerMicroFrame() {
}

uint8 mdtUsbEndointDescriptor::bInterval() {
}

