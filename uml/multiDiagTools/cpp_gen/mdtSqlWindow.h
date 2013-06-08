#ifndef _MDTSQLWINDOW_H
#define _MDTSQLWINDOW_H


class mdtAbstractSqlWidget;

class mdtSqlWindow : public QMainWindow, public Ui::mdtSqlWindow {
  public:
    mdtSqlWindow(const QWidget & parent, const Qt::WindowFlags & flags);

    ~mdtSqlWindow();


  private:
    mdtAbstractSqlWidget * pvSqlWidget;


  public:
    void setSqlWidget(const mdtAbstractSqlWidget & sqlWidget);


  private:
    QAction actSubmit;

    QAction actRevert;

    QAction actInsert;

    QAction actRemove;

    QAction actNavToFirst;

    QAction actNavToLast;

    QAction actNavToPrevious;

    QAction actNavToNext;


  public:
    void enableNavigation();

    void disableNavigation();

    void enableEdition();

    void disableEdition();

};
#endif
