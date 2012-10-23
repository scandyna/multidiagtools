#ifndef _MDTABSTRACTIOWIDGET_H
#define _MDTABSTRACTIOWIDGET_H


class mdtAbstractIoWidget : public QWidget {
  public:
     mdtAbstractIoWidget(const QWidget & parent = 0);

     ~mdtAbstractIoWidget();


  private:
    int pvAddress;

    QString pvLabelShort;

    QString pvLabel;

    QString pvDetails;


  public:
    void setLabelShort(const QString & text);

    QString labelShort();

    void setLabel(const QString & text);

    QString label();

    void setDetails(const QString & text);

    QString details();

};
#endif
