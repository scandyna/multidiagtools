
#include "mdtFieldMap.h"

mdtFieldMap::mdtFieldMap() {
}

 mdtFieldMap::~mdtFieldMap() {
}

void mdtFieldMap::addItem(const mdtFieldMapItem & item) {
}

void mdtFieldMap::clear() {
}

mdtFieldMapItem mdtFieldMap::itemAtFieldIndex(int index) {
}

mdtFieldMapItem mdtFieldMap::itemAtFieldName(const QString & name) {
}

mdtFieldMapItem mdtFieldMap::itemAtDisplayText(const QString & text) {
}

mdtFieldMapItem mdtFieldMap::itemAtSourceFieldIndex(int index) {
}

mdtFieldMapItem mdtFieldMap::itemAtSourceFieldName(const QString & name) {
}

mdtFieldMapItem mdtFieldMap::itemAtSourceFieldDisplayText(const QString & text) {
}

QVariant mdtFieldMap::dataForFieldIndex(const QStringList & sourceData, const  & ) {
}

QVariant mdtFieldMap::dataForFieldName(const QStringList & sourceData, const  & ) {
}

QVariant mdtFieldMap::dataForDisplayText(const QStringList & sourceData, const  & ) {
}

QString mdtFieldMap::dataForSourceFieldIndex(const QList<QVariant> & data) {
}

QString mdtFieldMap::dataForSourceFieldName(const QList<QVariant> & data) {
}

QString mdtFieldMap::dataForSourceFieldDisplayText(const QList<QVariant> & data) {
}

QList<mdtFieldMapItem*> mdtFieldMap::mappingItems() 
{
}

 mdtFieldMap::setSourceFields(const QList<mdtFieldMapField> & fields) 
{
}

void mdtFieldMap::setSourceFields(const QStringList & fields) 
{
}

mdtFieldMapField mdtFieldMap::sourceFields() 
{
}

 mdtFieldMap::setDestinationFields(const QList<mdtFieldMapField> & fields) 
{
}

void mdtFieldMap::setDestinationFields(const QStringList & fields) 
{
}

mdtFieldMapField mdtFieldMap::destinationFields() 
{
}

bool mdtFieldMap::generateMappingByIndexes() 
{
}

bool mdtFieldMap::generateMappingByNames() 
{
}

mdtError mdtFieldMap::lastError() 
{
}

