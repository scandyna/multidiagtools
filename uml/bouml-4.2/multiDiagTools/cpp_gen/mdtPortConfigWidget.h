#ifndef _MDTPORTCONFIGWIDGET_H
#define _MDTPORTCONFIGWIDGET_H


class mdtPortConfigWidget : public QWidget, public ui_mdtPortConfigWidget {
  public:
    mdtPortConfigWidget(const QWidget & parent);

    void displayConfig(const mdtPortConfig & config);

    void updateConfig(mdtPortConfig & config);

};
#endif
