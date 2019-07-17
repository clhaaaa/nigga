#include "..\Features.h"

/*
this shit is mega hella retarded pls dont use or judge i have no idea how to fix animations
*/

void Resolver::AnimationFix(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt) 
{
	if (g::CheatType != 1)
		return;

	// i rly am just to lazy pls dont judge
	static float ShotTime[65];

	static float oldSimtime[65];
	static float storedSimtime[65];

	static Vector oldEyeAngles[65];
	static float oldGoalfeetYaw[65];
	static Vector oldOrigin[65];

	float* PosParams = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(pEnt) + 0x2774);
	bool shot = false;

	auto* AnimState = pEnt->AnimState();
	if (!AnimState || !pEnt->AnimOverlays() || !PosParams)
		return;

	if (*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(AnimState) + 0x164) < 0) // sorry had to anti pasta this basically the key to nospread
		*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(AnimState) + 0x110) = 0.f;

	bool update = false;

	if (storedSimtime[pEnt->EntIndex()] != pEnt->GetSimulationTime())
	{
		pEnt->ClientAnimations() = true;
		pEnt->UpdateClientAnimation();
		pEnt->ClientAnimations() = false;

		update = true;
	}

	oldGoalfeetYaw[pEnt->EntIndex()] = AnimState->m_flGoalFeetYaw;

	if (pEnt->GetActiveWeapon() && !pEnt->IsKnifeorNade())
	{
		if (ShotTime[pEnt->EntIndex()] != pEnt->GetActiveWeapon()->GetLastShotTime())
		{
			shot = true;
			ShotTime[pEnt->EntIndex()] = pEnt->GetActiveWeapon()->GetLastShotTime();
		}
		else
			shot = false;
	}
	else
	{
		shot = false;
		ShotTime[pEnt->EntIndex()] = 0.f;
	}

	if (pLocalEnt && pLocalEnt->IsAlive())
	{
		float angToLocal = g_Math.NormalizeYaw(g_Math.CalcAngle(pLocalEnt->GetOrigin(), pEnt->GetOrigin()).y);

		float Back = g_Math.NormalizeYaw(angToLocal);
		float DesyncFix = 0;

		float Resim = g_Math.NormalizeYaw((TICKS_TO_TIME(16) / (pEnt->GetSimulationTime() - oldSimtime[pEnt->EntIndex()]))*g_Math.NormalizeYaw(oldEyeAngles[pEnt->EntIndex()].y - pEnt->GetEyeAngles().y));

		if (Resim > 60.f)
			Resim = 60.f;
		if (Resim < -60.f)
			Resim = -60.f;

		if (g_Menu.Config.Resolver[0] && !shot && !isnan(angToLocal) && !isinf(angToLocal) && pEnt != pLocalEnt && pEnt->GetTeam() != pLocalEnt->GetTeam())
		{
			float AntiSide = 0.f;

			if (g::MissedShots[pEnt->EntIndex()] % 2)
			{
				if (g_Math.NormalizeYaw(pEnt->GetEyeAngles().y - Back) > 0.f)
					AntiSide = -90.f;
				else if (g_Math.NormalizeYaw(pEnt->GetEyeAngles().y - Back) < 0.f)
					AntiSide = 90.f;
			}
			else
			{
				if (g_Math.NormalizeYaw(pEnt->GetEyeAngles().y - g_Math.NormalizeYaw(Back + 90)) > 0.f)
					AntiSide = 180.f;
				else if (g_Math.NormalizeYaw(pEnt->GetEyeAngles().y - g_Math.NormalizeYaw(Back + 90)) < 0.f)
					AntiSide = 0.f;
			}

			float Brute = g_Math.NormalizeYaw(Back + AntiSide);
			float Delta = g_Math.NormalizeYaw(g_Math.NormalizeYaw(Brute - pEnt->GetEyeAngles().y) + Resim);

			if (Delta > 60.f) // who needs clamps
				Delta = 60.f;
			if (Delta < -60.f)
				Delta = -60.f;

			Resim += Delta;
			DesyncFix += Delta;

			if (Resim > 60.f)
				Resim = 60.f;
			if (Resim < -60.f)
				Resim = -60.f;
		}

		float Equalized;

	//	if (fabs(g_Math.NormalizeYaw(oldEyeAngles[pEnt->EntIndex()].y - pEnt->GetEyeAngles().y)) < 60.f) // yea basically im retarded
			Equalized = g_Math.NormalizeYaw(pEnt->GetEyeAngles().y + Resim);
	//	else
	//		Equalized = g_Math.NormalizeYaw(pEnt->GetEyeAngles().y - Resim);

		if (g_Menu.Config.Resolver[0] && !shot && pEnt != pLocalEnt && pEnt->GetTeam() != pLocalEnt->GetTeam() && (pEnt->GetFlags() & FL_ONGROUND))
			AnimState->m_flGoalFeetYaw = Equalized;
	}

	if (g_Menu.Config.Resolver[1] && !shot && pEnt != pLocalEnt && pEnt->GetTeam() != pLocalEnt->GetTeam()) // 1337 pitch resolver
	{
		switch (g::MissedShots[pEnt->EntIndex()] % 3) // i skip case 0 on purpose noob
		{
		case 1:PosParams[12] = 89.f; break;
		case 2:PosParams[12] = -89.f; break;
		}
	} 

	if (shot && pEnt != pLocalEnt && pEnt->GetTeam() != pLocalEnt->GetTeam() && PosParams[12] <= -80) // ez shot pitch fix LMAO
		PosParams[12] = 89.f;

	if (update)
	{
		oldEyeAngles[pEnt->EntIndex()] = pEnt->GetEyeAngles();
		oldSimtime[pEnt->EntIndex()] = storedSimtime[pEnt->EntIndex()];
		storedSimtime[pEnt->EntIndex()] = pEnt->GetSimulationTime();
		oldOrigin[pEnt->EntIndex()] = pEnt->GetOrigin();
	}

	pEnt->SetAbsAngles(Vector(0, oldGoalfeetYaw[pEnt->EntIndex()], 0));
}

void Resolver::HandleHits(C_BaseEntity* pEnt, C_BaseEntity* pLocalEnt)
{
	if (g::CheatType != 1)
		return;

	auto NetChannel = g_pEngine->GetNetChannelInfo();

	if (!NetChannel)
		return;

	static float predTime[65];
	static bool init[65];

	static float StoredSimtime[65];
	static float SimDelta[65];

	if (StoredSimtime[pEnt->EntIndex()] != pEnt->GetSimulationTime())
	{
		SimDelta[pEnt->EntIndex()] = fabs(pEnt->GetSimulationTime() - StoredSimtime[pEnt->EntIndex()]);
		StoredSimtime[pEnt->EntIndex()] = pEnt->GetSimulationTime();
	}

	if (g::Shot[pEnt->EntIndex()])
	{
		if (init[pEnt->EntIndex()])
		{
			Resolver::get().pitchHit[pEnt->EntIndex()] = pEnt->GetEyeAngles().x;
			predTime[pEnt->EntIndex()] = g_pGlobalVars->curtime + NetChannel->GetAvgLatency(FLOW_INCOMING) + pLocalEnt->FireRate();  // maybe????
			init[pEnt->EntIndex()] = false;
		}

		if (g_pGlobalVars->curtime > predTime[pEnt->EntIndex()] && !g::Hit[pEnt->EntIndex()])
		{
			g::MissedShots[pEnt->EntIndex()] += 1;
			g::Shot[pEnt->EntIndex()] = false;
		}
		else if (g_pGlobalVars->curtime <= predTime[pEnt->EntIndex()] && g::Hit[pEnt->EntIndex()])
			g::Shot[pEnt->EntIndex()] = false;

	}
	else
		init[pEnt->EntIndex()] = true;

	g::Hit[pEnt->EntIndex()] = false;
}

void Resolver::OnCreateMove() // kinda dont need this anymore
{
	if (g::CheatType != 1)
		return;

	if (!g_Menu.Config.Resolver[0])
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g::pLocalEntity->GetActiveWeapon() || g::pLocalEntity->IsKnifeorNade())
		return;


	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam())
		{
			UseFreestandAngle[i] = false;
			continue;
		}

		if (abs(pPlayerEntity->GetVelocity().Length2D()) > 29.f)
			UseFreestandAngle[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->GetVelocity().Length2D()) <= 29.f && !UseFreestandAngle[pPlayerEntity->EntIndex()])
		{
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

			float angToLocal = g_Math.CalcAngle(g::pLocalEntity->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector ViewPoint = g::pLocalEntity->GetOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector Origin = pPlayerEntity->GetOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (Autowall::get().CanHitFloatingPoint(OriginAutowall, ViewPoint, pPlayerEntity))
				{
					if (side == 0)
					{
						HitSide1 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = 90;
					}
					else if (side == 1)
					{
						HitSide2 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = -90;
					}

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (Autowall::get().CanHitFloatingPoint(OriginAutowall222, OriginAutowall2, pPlayerEntity))
						{
							if (side222 == 0)
							{
								HitSide1 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = 90;
							}
							else if (side222 == 1)
							{
								HitSide2 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = -90;
							}

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)	
					UseFreestandAngle[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle[pPlayerEntity->EntIndex()] = true;
			}
		}
	}
}