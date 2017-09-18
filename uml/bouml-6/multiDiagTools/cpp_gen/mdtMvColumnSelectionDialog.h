#ifndef _MDTMVCOLUMNSELECTIONDIALOG_H
#define _MDTMVCOLUMNSELECTIONDIALOG_H


class mdtMvColumnSelectionDialog : public mdtItemsSelectorDialog
{
  private:
    QAbstractTableModel * pvModel;

    QTableView * pvView;


  public:
  mdtMvColumnSelectionDialog(const QWidget & parent, const QAbstractTableModel & model, const QTableView & view);

    virtual void accept();


  private:
  void displayAvailableColumns();

  void displaySelectedColumns();

};

#endif // #ifndef MDTMVCOLUMNSELECTIONDIALOG_H
