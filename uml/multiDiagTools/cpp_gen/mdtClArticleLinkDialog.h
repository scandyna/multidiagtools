#ifndef _MDTCLARTICLELINKDIALOG_H
#define _MDTCLARTICLELINKDIALOG_H


class mdtSqlSelectionDialog;

class mdtClArticleLinkDialog : public QDialog, public Ui::mdtClArticleLinkDialog {
  private:
    QVariant pvLinkTypeCode;

    QVariant pvLinkDirectionCode;

    double pvValue;

    QVariant pvStartConnectionId;

    QVariant pvEndConnectionId;

    QComboBox [2];


  public:
    void onCbLinkTypeCurrentIndexChanged(int index);

    void onCbLinkDirectionCurrentIndexChanged(int index);

    void selectStartConnection();

    void selectEndConnection();


  private:
    QSqlQueryModel [3];


  public:
    void setLinkTypeCode(const QVariant & code);

    void setLinkDirectionCode(const QVariant & code);

    void setValue(const QVariant & value);

    void setStartConnectionId(const QVariant & id);

    void setEndConnectionId(const QVariant & id);

    QVariant linkTypeCode();

    QVariant linkDirectionCode();

    QVariant value();

    QVariant startConnectionId();

    QVariant endConnectionId();

};
#endif
