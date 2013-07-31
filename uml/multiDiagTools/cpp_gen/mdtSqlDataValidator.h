#ifndef _MDTSQLDATAVALIDATOR_H
#define _MDTSQLDATAVALIDATOR_H


class mdtSqlDataValidator : public QObject {
  private:
    QSqlTableModel * pvModel;


  public:
    mdtSqlDataValidator(const QObject & parent, const QSqlTableModel & model);

    virtual ~mdtSqlDataValidator();

    virtual bool checkBeforeSubmit() = 0;

    void displayWarning(const QString & text, const QString & informativeText, const QSqlError & sqlError);

    void displayError(const QString & text, const QString & informativeText, const QSqlError & sqlError);

};
#endif
