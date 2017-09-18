#ifndef _MDTMVSORTINGSETUPDIALOG_H
#define _MDTMVSORTINGSETUPDIALOG_H


class mdtSortFilterProxyModel;

class mdtMvSortingSetupDialog : public mdtItemsSelectorDialog
{
  private:
    QAbstractTableModel * pvModel;

    mdtSortFilterProxyModel * pvProxyModel;


  public:
  mdtMvSortingSetupDialog(const QWidget & parent, const QAbstractTableModel & model, const mdtSortFilterProxyModel & proxyModel);

    virtual void accept();


  private:
  void displayAvailableColumns();

  void displaySortedColumns();

  void applySelectedColumns();

  void applyNotSelectedColumns();

};

#endif // #ifndef MDTMVSORTINGSETUPDIALOG_H
