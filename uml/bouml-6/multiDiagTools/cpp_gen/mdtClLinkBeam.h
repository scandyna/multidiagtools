#ifndef _MDTCLLINKBEAM_H
#define _MDTCLLINKBEAM_H


class mdtClLinkBeam : public mdtTtBase
{
  public:
  mdtClLinkBeam(const QObject & parent, const QSqlDatabase & db);

  QString sqlForStartUnitSelection(const QVariant & linkBeamId);

  QString sqlForEndUnitSelection(const QVariant & linkBeamId);

  bool addStartUnit(const QVariant & unitId, const QVariant & linkBeamId);

  bool removeStartUnit(const QVariant & unitId, const QVariant & linkBeamId);

  bool addEndUnit(const QVariant & unitId, const QVariant & linkBeamId);

  bool removeEndUnit(const QVariant & unitId, const QVariant & linkBeamId);

};

#endif // #ifndef MDTCLLINKBEAM_H
