#ifndef _MDTSQLDIALOG_H
#define _MDTSQLDIALOG_H


class mdtAbstractSqlWidget;

class mdtSqlDialog : public QDialog {
  private:
    mdtAbstractSqlWidget * pvSqlWidget;


  public:
    mdtSqlDialog(const QWidget & parent);

    ~mdtSqlDialog();

    void setSqlWidget(const mdtAbstractSqlWidget & sqlWidget);

    void addChildWidget(const QWidget & widget, const QString & label);

    bool setCurrentRow(int row);

    bool setCurrentRow(const QString & fieldName, const QVariant & matchData);

    bool setCurrentRow(const QList<QPair<QString, AVariant> > & matchList);

    virtual void accept();

};
#endif
