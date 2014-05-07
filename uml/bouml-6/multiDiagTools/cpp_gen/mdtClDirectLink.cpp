
#include "mdtClDirectLink.h"
#include "mdtSqlDatabaseManager.h"

mdtClDirectLink::mdtClDirectLink(const QObject & parent, const QSqlDatabase & db) 
{
}

mdtClDirectLink::~mdtClDirectLink() 
{
}

QVariant mdtClDirectLink::getUnitConnectionIdListPartOfUnit(const QVariant & unitConnectionIdList, const QVariant & unitId, bool & ok) 
{
}

QVariant mdtClDirectLink::getUnitConnectionIdListPartOfUnitConnector(const QVariant & unitConnectionIdList, const QVariant & unitConnectorId, bool & ok) 
{
}

bool mdtClDirectLink::addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId) 
{
}

bool mdtClDirectLink::createDirectLinkTable() 
{
}

bool mdtClDirectLink::removeDirectLinkTable() 
{
}

