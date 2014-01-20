#ifndef _MDTUICNUMBERWIDGET_H
#define _MDTUICNUMBERWIDGET_H


class mdtUicNumberWidget : public QWidget, public Ui::mdtUicNumberWidget {
  public:
    mdtUicNumberWidget(const QWidget & parent);

    ~mdtUicNumberWidget();


  private:
    mdtUicNumber * pvUicNumber;


  public:
    void setUicNumber(const mdtUicNumber & uicNumber);

    mdtUicNumber uicNumber();

};
#endif
