#ifndef _MDTAPPLICATION_H
#define _MDTAPPLICATION_H


class mdtApplication : public QtSingleApplication {
  public:
    mdtApplication(int argc, char argv, bool GUIenabled);

    ~mdtApplication();

};
#endif
