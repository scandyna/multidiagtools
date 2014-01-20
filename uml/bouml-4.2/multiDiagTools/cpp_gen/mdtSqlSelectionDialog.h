#ifndef _MDTSQLSELECTIONDIALOG_H
#define _MDTSQLSELECTIONDIALOG_H


class mdtSqlSelectionDialog : public QDialog {
  public:
    mdtSqlSelectionDialog(const QObject & parent);

    ~mdtSqlSelectionDialog();


  private:
    QTableView pvTableView;

    QSqlQueryModel * pvModel;


  public:
    void setModel(const QSqlQueryModel & model);

    void setColumnHidden(int column, bool hide);

    void setColumnHidden(const QString & fieldName, bool hide);

    void setSelectionResultColumns(const QList<int> & columns);

    void setSelectionResultColumns(const QStringList & fields);

    QList<QVariant> selectionResult();

    void reject();

};
#endif
