#ifndef _MDTABSTRACTIOWIDGET_H
#define _MDTABSTRACTIOWIDGET_H


class mdtAbstractIo;
class mdtAnalogIo;

class mdtAbstractIoWidget : public QWidget {
  public:
     mdtAbstractIoWidget(const QWidget & parent = 0);

     ~mdtAbstractIoWidget();


  protected:
    void setIo(const mdtAbstractIo & io);


  private:
    void setAddress(int adr);

    void setLabelShort(const QString & text);

    void setLabel(const QString & text);

    void setDetails(const QString & text);

};
#endif
