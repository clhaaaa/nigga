#include "GlobalVars.h"

namespace g
{
    CUserCmd*      pCmd         = nullptr;
    C_BaseEntity*  pLocalEntity = nullptr;
    std::uintptr_t uRandomSeed  = NULL;
	Vector         OriginalView;
	bool           bSendPacket  = true;
	bool		   LagPeek      = false;
	int            TargetIndex  = -1;
	int			   tickCount	= -1;
	Vector         RealAngle;
	Vector         FakeAngle;
	bool           Shot[65];
	bool           Hit[65];
	int            MissedShots[65];
	int			   CheatType = -1;

	DWORD CourierNew;
	DWORD Tahoma;
}
