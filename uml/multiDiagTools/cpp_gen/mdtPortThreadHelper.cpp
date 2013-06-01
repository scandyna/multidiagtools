
#include "mdtPortThreadHelper.h"

void mdtPortThreadHelper::setPort(const mdtAbstractPort & port) {
}

void mdtPortThreadHelper::setThread(const mdtPortThread & thread) {
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonReadErrors(const mdtAbstractPort::error_t & portError, const  & ) {
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonWriteErrors(const mdtAbstractPort::error_t & portError, const  & ) {
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonReadWriteErrors(const mdtAbstractPort::error_t & portErros, const  & , const  & ) {
}

mdtFrame mdtPortThreadHelper::getNewFrameRead() {
}

int mdtPortThreadHelper::submitReadenData(char data, int size, bool emitNewFrameReaden) {
}

mdtFrame mdtPortThreadHelper::getNewFrameWrite() {
}

mdtFrame mdtPortThreadHelper::currentReadFrame() {
}

mdtFrame mdtPortThreadHelper::currentWriteFrame() {
}

void mdtPortThreadHelper::notifyError(int error, bool renotifySameError) {
}

