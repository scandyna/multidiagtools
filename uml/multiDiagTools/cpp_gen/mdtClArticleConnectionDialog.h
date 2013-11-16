#ifndef _MDTCLARTICLECONNECTIONDIALOG_H
#define _MDTCLARTICLECONNECTIONDIALOG_H


#include "mdtClArticleConnectionData.h"

class mdtClArticleConnectionDialog : public QDialog, public Ui::mdtClArticleConnectionDialog
{
  private:
    mdtClArticleConnectionData * pvData;


  public:
  mdtClArticleConnectionDialog(const QDialog & parent);

  ~mdtClArticleConnectionDialog();

  void setData(const mdtClArticleConnectionData & data);

  mdtClArticleConnectionData data();


  private:
    virtual void accept();

};

#endif // #ifndef MDTCLARTICLECONNECTIONDIALOG_H
