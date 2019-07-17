#include "..\Features.h"

void Legitbot::OnCreateMove() // best legit backtrack ever xd
{
	if ( !g_pEngine->IsInGame())
		return;

	if (!g_Menu.Config.LegitBacktrack)
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g::pLocalEntity->GetActiveWeapon())
		return;

	int ClosestDistance = 9999;
	float BestSimTime = 0.f;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam())
			continue;

		int Height, Width;
		g_pEngine->GetScreenSize(Width, Height);

		Vector punchAngle = g::pLocalEntity->GetAimPunchAngle();

		float x = Width / 2;
		float y = Height / 2;

		int dy = Height / 90;
		int dx = Width / 90;
		x -= (dx*(punchAngle.y));
		y += (dy*(punchAngle.x));

		Vector2D screenPunch = Vector2D(x, y);

		if (LagComp::get().PlayerRecord[i].size() > 0)
		{
			for (int tick = 0; tick < LagComp::get().PlayerRecord[i].size(); tick++)
			{
				if (!Autowall::get().VectortoVectorVisible(g::pLocalEntity->GetEyePosition(), LagComp::get().PlayerRecord[i].at(tick).Head, pPlayerEntity))
					continue;

				Vector2D HeadW2S;
				if (!Utils::WorldToScreen(LagComp::get().PlayerRecord[i].at(tick).Head, HeadW2S))
					HeadW2S = Vector2D(99999.f, 99999.f);

				if (abs(g_Math.Distance(HeadW2S, screenPunch)) < ClosestDistance)
				{
					BestSimTime = LagComp::get().PlayerRecord[i].at(tick).SimTime;
					ClosestDistance = abs(g_Math.Distance(HeadW2S, screenPunch));
				}
			}
		}	
	}

	if (g::pLocalEntity->IsAlive() && (g::pCmd->buttons & IN_ATTACK) && BestSimTime != 0.f)
	{
		float flServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;
		bool canShoot = (g::pLocalEntity->GetActiveWeapon()->GetNextPrimaryAttack() <= flServerTime);

		if (canShoot)
			g::pCmd->tick_count = TIME_TO_TICKS(BestSimTime + LagComp::get().LerpTime());
	}
}
