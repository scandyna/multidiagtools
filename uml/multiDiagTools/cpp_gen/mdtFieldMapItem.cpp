
#include "mdtFieldMapItem.h"

mdtFieldMapItem::mdtFieldMapItem() {
}

mdtFieldMapItem::~mdtFieldMapItem() {
}

void mdtFieldMapItem::set_pvFieldIndex(int value) {
  pvFieldIndex = value;
}

void mdtFieldMapItem::set_pvFieldName(QString value) {
  pvFieldName = value;
}

void mdtFieldMapItem::set_pvFieldDisplayText(QString value) {
  pvFieldDisplayText = value;
}

void mdtFieldMapItem::set_pvSourceFieldIndex(int value) {
  pvSourceFieldIndex = value;
}

void mdtFieldMapItem::set_pvSourceFieldName(QString value) {
  pvSourceFieldName = value;
}

void mdtFieldMapItem::set_pvSourceFieldDisplayText(QString value) {
  pvSourceFieldDisplayText = value;
}

void mdtFieldMapItem::set_pvSourceFieldDataStartOffset(int value) {
  pvSourceFieldDataStartOffset = value;
}

void mdtFieldMapItem::set_pvSourceFieldDataEndOffset(int value) {
  pvSourceFieldDataEndOffset = value;
}

void mdtFieldMapItem::set_pvDataType(QVariant::Type value) {
  pvDataType = value;
}

