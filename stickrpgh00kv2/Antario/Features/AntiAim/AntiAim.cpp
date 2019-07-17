#include "..\Features.h"

bool Swtich = false;
static float SentYaw;
static bool FreestandingSide = false;

void FreeStanding() // should have rewritten
{
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;

	float tempdis = 999999999.f;
	Vector2D LocalOrg = Vector2D(g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y);

	float closeYaw = 999.f;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam())
			continue;

		Vector2D EnemyOrg = Vector2D(pPlayerEntity->GetOrigin().x, pPlayerEntity->GetOrigin().y);


		if (tempdis > fabs(g_Math.Distance(EnemyOrg, LocalOrg)))
		{
			closeYaw = g_Math.NormalizeYaw(g_Math.CalcAngle(g::pLocalEntity->GetOrigin(), pPlayerEntity->GetOrigin()).y);
			tempdis = fabs(g_Math.Distance(EnemyOrg, LocalOrg));
		}

		if (g_Menu.Config.Freestanding)
		{
			float angToLocal = g_Math.CalcAngle(g::pLocalEntity->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector ViewPoint = pPlayerEntity->GetOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector Origin = g::pLocalEntity->GetOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 80 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (Autowall::get().CanHitFloatingPoint(OriginAutowall, ViewPoint, pPlayerEntity))
				{
					if (side == 0)
					{
						FreestandingSide = true;
						bside1 = true;
						FinalAngle = angToLocal + 90;
					}
					else if (side == 1)
					{
						FreestandingSide = false;
						bside2 = true;
						FinalAngle = angToLocal - 90;
					}
					autowalld = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 80 };

						if (Autowall::get().CanHitFloatingPoint(OriginAutowall222, OriginAutowall2, pPlayerEntity))
						{
							if (side222 == 0)
							{
								FreestandingSide = true;
								bside1 = true;
								FinalAngle = angToLocal + 90;
							}
							else if (side222 == 1)
							{
								FreestandingSide = false;
								bside2 = true;
								FinalAngle = angToLocal - 90;
							}
							autowalld = true;
						}
					}
				}
			}
		}
	}

	if (!autowalld || (bside1 && bside2))
	{
		if (closeYaw != 999.f && g_Menu.Config.AtTarget)
			g::pCmd->viewangles.y = g_Math.NormalizeYaw(closeYaw + SentYaw);
		else
			g::pCmd->viewangles.y += SentYaw;
	}
	else
		g::pCmd->viewangles.y = FinalAngle;
}

bool LBYUpdate() 
{
	static float NextUpdate = 0;
	float Curtime = g_pGlobalVars->curtime;
	auto* AnimState = g::pLocalEntity->AnimState();

	if (!AnimState || !(g::pLocalEntity->GetFlags() & FL_ONGROUND))
		return false;

	if (AnimState->speed_2d > 0.1f) 
		NextUpdate = Curtime + 0.22f;

	if (NextUpdate < Curtime) 
	{
		NextUpdate = Curtime + 1.1f;
		return true;
	}

	return false;
}

void Real()
{
	float Yaw = 0.f;
	if (g::bSendPacket)
	{
		switch (g_Menu.Config.Yaw)
		{
		case 0:
			Yaw += 180.f;
			break;
		case 1:
			Yaw += 0.f;//lol
			break;
		case 2:
			Yaw += Swtich ? -90.f : 90.f;
			break;
		case 3:
			Yaw = g_Math.NormalizeYaw(g_pGlobalVars->curtime * 360.f);
			break;
		}

		SentYaw = Yaw;

		FreeStanding();

		if (g_Menu.Config.JitterRange != 0)
		{
			float Offset = g_Menu.Config.JitterRange / 2.f;

			if (!g_Menu.Config.RandJitterInRange)
			{
				Swtich ? g::pCmd->viewangles.y -= Offset : g::pCmd->viewangles.y += Offset;
			}
			else
			{
				static bool oldSwtich = Swtich;

				g::pCmd->viewangles.y -= Offset;

				static int Add = 0;

				if (oldSwtich != Swtich)
				{
					Add = rand() % g_Menu.Config.JitterRange;
					oldSwtich = Swtich;
				}

				g::pCmd->viewangles.y += Add;
			}
		}

		SentYaw = g::pCmd->viewangles.y;
	}

	if (!g::bSendPacket && g::pLocalEntity->AnimState() && g_pEngine->GetNetChannel() && g_Menu.Config.DesyncAngle) // yea i have absolutly no clue on how todo desync just pasted in someting stacker gave me
	{
		auto NetChannel = g_pEngine->GetNetChannel();
		float Delta = g::pLocalEntity->GetMaxDesyncDelta();
		bool LBYUpdateb = LBYUpdate();

		if (!g_Menu.Config.Freestanding)
			if (LBYUpdateb)
				FreestandingSide = !FreestandingSide;

		if (FreestandingSide) 
		{
			if (LBYUpdateb)
				g::pCmd->viewangles.y = SentYaw - 120.f;
			else
				g::pCmd->viewangles.y = SentYaw - g::pLocalEntity->AnimState()->speed_2d <= 0.1 ? (Delta + 30.f) : Delta;
		}
		else 
		{
			if (LBYUpdateb)
				g::pCmd->viewangles.y = SentYaw + 120.f;
			else
				g::pCmd->viewangles.y = SentYaw + g::pLocalEntity->AnimState()->speed_2d <= 0.1 ? (Delta + 30.f) : Delta;
		}
	}
	else
		g::pCmd->viewangles.y = SentYaw;
}

void Pitch()
{
	switch (g_Menu.Config.Pitch)
	{
	case 0: 
		g::pCmd->viewangles.x = 89.9f; 
		break;
	case 1: 
		g::pCmd->viewangles.x = Swtich ? 991.f : 1080.f;
		break;
	case 2: 
		g::pCmd->viewangles.x = 1080.f;
		break;
	case 3:
	{
		static bool chjang = false;
		static float spin = 0;
		if (chjang != Swtich)
		{
			spin += 10.f;
			chjang = Swtich;
		}

		if (spin > 80.f)
			spin = 0.f;

		g::pCmd->viewangles.x = 1080.f - spin;
	}
		
		break;

	case 4: g::pCmd->viewangles.x = -89.9f; break;
	case 5: g::pCmd->viewangles.x = 991.f; break;
	}
}

void AntiAim::OnCreateMove()
{
	if (!g_pEngine->IsInGame() || !g_Menu.Config.Antiaim || g_Menu.Config.LegitBacktrack)
		return;

	if (g::CheatType != 1)
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g::pLocalEntity->GetActiveWeapon() || g::pLocalEntity->IsKnifeorNade())
		return;

	float flServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;
	bool canShoot = (g::pLocalEntity->GetActiveWeapon()->GetNextPrimaryAttack() <= flServerTime);

	if (canShoot && (g::pCmd->buttons & IN_ATTACK))
		return;

	if (g::pCmd->buttons & IN_USE)
	{
		g::bSendPacket = true;
		return;
	}
	
	if (g::bSendPacket)
		Swtich = !Swtich;

	Pitch();
	Real();
}