#ifndef _MDTSQLDATAVALIDATOR_H
#define _MDTSQLDATAVALIDATOR_H


class mdtSqlDataValidator : public QObject {
  private:
    QSqlTableModel * pvModel;


  public:
    mdtSqlDataValidator(const QSqlTableModel & model, const QObject & parent);

    virtual ~mdtSqlDataValidator();

    QSqlTableModel model();

    virtual bool checkBeforeSubmit() = 0;

    void displayWarning(const QString & text, const QString & informativeText, const QSqlError & sqlError);

    void displayError(const QString & text, const QString & informativeText, const QSqlError & sqlError);

};
#endif
