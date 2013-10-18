
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlDataValidator.h"
#include "mdtSqlRelation.h"

mdtAbstractSqlWidget::mdtAbstractSqlWidget(const QWidget & parent) {
}

mdtAbstractSqlWidget::~mdtAbstractSqlWidget() {
}

void mdtAbstractSqlWidget::addDataValidator(const mdtSqlDataValidator & validator, bool putAsTopPriority) {
}

void mdtAbstractSqlWidget::addChildWidget(const mdtAbstractSqlWidget & widget, const mdtSqlRelation & relation) {
}

QList<mdtAbstractSqlWidget> mdtAbstractSqlWidget::sqlWidgets() {
}

void mdtAbstractSqlWidget::submitTriggered() {
}

void mdtAbstractSqlWidget::revertTriggered() {
}

void mdtAbstractSqlWidget::insertTriggered() {
}

void mdtAbstractSqlWidget::removeTriggered() {
}

void mdtAbstractSqlWidget::errorOccured() {
}

void mdtAbstractSqlWidget::operationSucceed() {
}

void mdtAbstractSqlWidget::dataEdited() {
}

void mdtAbstractSqlWidget::currentIndexChanged(int index) {
}

void mdtAbstractSqlWidget::onStateVisualizingEntered() {
}

void mdtAbstractSqlWidget::onStateEditingEntered() {
}

void mdtAbstractSqlWidget::onStateSubmittingEntered() {
}

void mdtAbstractSqlWidget::onStateRevertingEntered() {
}

void mdtAbstractSqlWidget::onStateInsertingEntered() {
}

void mdtAbstractSqlWidget::onStateSubmittingNewRowEntered() {
}

void mdtAbstractSqlWidget::onStateRevertingNewRowEntered() {
}

void mdtAbstractSqlWidget::onStateRemovingEntered() {
}

