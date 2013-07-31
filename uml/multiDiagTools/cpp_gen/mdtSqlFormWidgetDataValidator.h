#ifndef _MDTSQLFORMWIDGETDATAVALIDATOR_H
#define _MDTSQLFORMWIDGETDATAVALIDATOR_H


#include "mdtSqlDataValidator.h"

class mdtSqlFieldHandler;

class mdtSqlFormWidgetDataValidator : public mdtSqlDataValidator {
  public:
    mdtSqlFormWidgetDataValidator(const QObject & parent, const QSqlTableModel & model, const mdtSqlFieldHandler & fieldHandler);

    ~mdtSqlFormWidgetDataValidator();

    virtual bool checkBeforeSubmit();


  private:
    mdtSqlFieldHandler * pvFieldHandlers;

};
#endif
