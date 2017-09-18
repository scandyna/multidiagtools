#ifndef _MDTTTTESTLINKDIALOG_H
#define _MDTTTTESTLINKDIALOG_H


#include "mdtTtTestLinkData.h"

class mdtTtTestLinkDialog : public QDialog, public Ui::mdtTtTestLinkDialog
{
  public:
  mdtTtTestLinkDialog(const QWidget & parent, const QSqlDatabase & db);

  ~mdtTtTestLinkDialog();

  void setTestUnit(const QVariant & unitId);

  void setDutUnit(const QVariant & unitId);

  void setTestConnection(const QVariant & unitConnectionId);

  void setDutConnection(const QVariant & unitConnectionId);

  mdtTtTestLinkData linkData();


  private:
  void selectTestConnection();

  void selectDutConnection();

  void accept();

  void reject();

  void displayTestConnection();

  void displayDutConnection();

};

#endif // #ifndef MDTTTTESTLINKDIALOG_H
