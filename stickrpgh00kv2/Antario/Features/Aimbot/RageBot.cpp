#include "..\Features.h"

void RageBot::Autostop()
{
	if (!g_Menu.Config.Autostop)
		return;

	Vector Velocity = g::pLocalEntity->GetVelocity();
	static float Speed = 450.f;

	Vector Direction;
	Vector RealView;
	g_Math.VectorAngles(Velocity, Direction);
	g_pEngine->GetViewAngles(RealView);
	Direction.y = RealView.y - Direction.y;

	Vector Forward;
	g_Math.AngleVectors(Direction, &Forward);
	Vector NegativeDirection = Forward * -Speed;

	g::pCmd->forwardmove = NegativeDirection.x;
	g::pCmd->sidemove = NegativeDirection.y;
}

bool RageBot::HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, Vector Point, int chance)
{
	if (chance == 0)
		return true;

	if (Backtrack[pEnt->EntIndex()] || ShotBacktrack[pEnt->EntIndex()]) // doing this bec im lazy
	{
		float Velocity = g::pLocalEntity->GetVelocity().Length();

		if (Velocity <= (g::pLocalEntity->GetActiveWeapon()->GetCSWpnData()->max_speed_alt * .34f))
			Velocity = 0.0f;

		float SpreadCone = pWeapon->GetAccuracyPenalty() * 256.0f / M_PI + pWeapon->GetCSWpnData()->max_speed * Velocity / 3000.0f; // kmeth https://github.com/DankPaster/kmethdude
		float a = (Point - g::pLocalEntity->GetEyePosition()).Length();
		float b = sqrt(tan(SpreadCone * M_PI / 180.0f) * a);
		if (2.2f > b) return true;
		return (chance <= ((2.2f / fmax(b, 2.2f)) * 100.0f));
	}

	static float Seeds = 256.f;

	Vector forward, right, up;

	g_Math.AngleVectors(Angle, &forward, &right, &up);

	int Hits = 0, neededHits = (Seeds * (chance / 100.f));

	float weapSpread = pWeapon->GetSpread(), weapInaccuracy = pWeapon->GetInaccuracy();

	bool Return = false;

	for (int i = 0; i < Seeds; i++)
	{
		float Inaccuracy = g_Math.RandomFloat(0.f, 1.f) * weapInaccuracy;
		float Spread = g_Math.RandomFloat(0.f, 1.f) * weapSpread;

		Vector spreadView((cos(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (cos(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Spread), (sin(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (sin(g_Math.RandomFloat(0.f, 2.f * M_PI)) * Spread), 0), direction;
		direction = Vector(forward.x + (spreadView.x * right.x) + (spreadView.y * up.x), forward.y + (spreadView.x * right.y) + (spreadView.y * up.y), forward.z + (spreadView.x * right.z) + (spreadView.y * up.z)).Normalize(); // guess i cant put vector in a cast *nvm im retarded

		Vector viewanglesSpread, viewForward;

		g_Math.VectorAngles(direction, up, viewanglesSpread);
		g_Math.NormalizeAngles(viewanglesSpread);

		g_Math.AngleVectors(viewanglesSpread, &viewForward);
		viewForward.NormalizeInPlace();

		viewForward = g::pLocalEntity->GetEyePosition() + (viewForward * pWeapon->GetCSWpnData()->range);

		C_Trace Trace;

		g_pTrace->ClipRayToEntity(C_Ray(g::pLocalEntity->GetEyePosition(), viewForward), mask_shot | contents_grate, pEnt, &Trace);

		if (Trace.m_pEnt == pEnt)
			Hits++;

		if (((Hits / Seeds) * 100.f) >= chance)
		{
			Return = true;
			break;
		}

		if ((Seeds - i + Hits) < neededHits)
			break;
	}

	return Return;
}

bool ShouldBaim(C_BaseEntity* pEnt) // probably dosnt make sense
{
	static float oldSimtime[65];
	static float storedSimtime[65];

	static float ShotTime[65];
	static float NextShotTime[65];
	static bool BaimShot[65];

	if (storedSimtime[pEnt->EntIndex()] != pEnt->GetSimulationTime())
	{
		oldSimtime[pEnt->EntIndex()] = storedSimtime[pEnt->EntIndex()];
		storedSimtime[pEnt->EntIndex()] = pEnt->GetSimulationTime();
	}

	float simDelta = storedSimtime[pEnt->EntIndex()] - oldSimtime[pEnt->EntIndex()];

	bool Shot = false;

	if (pEnt->GetActiveWeapon() && !pEnt->IsKnifeorNade())
	{
		if (ShotTime[pEnt->EntIndex()] != pEnt->GetActiveWeapon()->GetLastShotTime())
		{
			Shot = true;
			BaimShot[pEnt->EntIndex()] = false;
			ShotTime[pEnt->EntIndex()] = pEnt->GetActiveWeapon()->GetLastShotTime();
		}
		else
			Shot = false;
	}
	else
	{
		Shot = false;
		ShotTime[pEnt->EntIndex()] = 0.f;
	}

	if (Shot)
	{
		NextShotTime[pEnt->EntIndex()] = pEnt->GetSimulationTime() + pEnt->FireRate();

		if (simDelta >= pEnt->FireRate())
			BaimShot[pEnt->EntIndex()] = true;
	}

	if (BaimShot[pEnt->EntIndex()])
	{
		if (pEnt->GetSimulationTime() >= NextShotTime[pEnt->EntIndex()])
			BaimShot[pEnt->EntIndex()] = false;
	}

	if (g_Menu.Config.BaimPitch && BaimShot[pEnt->EntIndex()] && !(pEnt->GetFlags() & FL_ONGROUND))
		return true;

	if (g_Menu.Config.BaimInAir && !(pEnt->GetFlags() & FL_ONGROUND))
		return true;

	return false;
}
//"head"0, "chest"1, "pelvis"2, "upper arm"3, "toes"4, "thighs"5
Vector RageBot::Hitscan(C_BaseEntity* pEnt)
{
	float tempDmg = -1.f;
	float angToLocal = g_Math.CalcAngle(g::pLocalEntity->GetOrigin(), pEnt->GetOrigin()).y;

	Vector Forward, Forward1;

	g_Math.AngleVectors(Vector(0.f, g_Math.NormalizeYaw(angToLocal + 90.f), 0.f), &Forward);
	g_Math.NormalizeAngles(Forward);
	g_Math.AngleVectors(Vector(0.f, g_Math.NormalizeYaw(angToLocal - 90.f), 0.f), &Forward1);
	g_Math.NormalizeAngles(Forward1);

	Vector Mutipoint[] = { Forward, Forward1 };

	float Velocity = abs(pEnt->GetVelocity().Length2D());

	if (!g_Menu.Config.DelayShot && Velocity > 29.f)
		Velocity = 30.f;

	std::vector<HitboxInfo> Scan;

	int HeadHeight = 0;

	bool Baim = ShouldBaim(pEnt);

	matrix3x4_t* Matrix = LagComp::get().pMatrix[pEnt->EntIndex()];

	if (!Baim && g_Menu.Config.Hitscan[0])
		Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_HEAD, Matrix), false });

	if (Velocity <= 215.f || Baim)
	{
		if (g_Menu.Config.Hitscan[2])
		{
			Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_PELVIS, Matrix), true });
			Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_THORAX, Matrix), true });
		}

		if (g_Menu.Config.Hitscan[1])
		{
			Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_LOWER_CHEST, Matrix), true });
			Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_UPPER_CHEST, Matrix), false });
		}

		if (g_Menu.Config.MultiPoint)
		{
			float Radius = 0.f;
			Vector Hitbox;

			if (g_Menu.Config.Hitscan[2])
			{
				if (!Autowall::get().VectortoVectorVisible(g::pLocalEntity->GetEyePosition(), pEnt->GetHitboxPosition(HITBOX_PELVIS, Matrix), pEnt))
				{
					Hitbox = pEnt->GetHitboxPosition(HITBOX_PELVIS, Matrix, &Radius);
					Radius *= (g_Menu.Config.BodyScale / 100.f);

					Scan.push_back({ Vector(Hitbox + (Mutipoint[0] * Radius)), true });//pelvis
					Scan.push_back({ Vector(Hitbox + (Mutipoint[1] * Radius)), true });
				}

				if (!Autowall::get().VectortoVectorVisible(g::pLocalEntity->GetEyePosition(), pEnt->GetHitboxPosition(HITBOX_THORAX, Matrix), pEnt))
				{
					Hitbox = pEnt->GetHitboxPosition(HITBOX_THORAX, Matrix, &Radius);
					Radius *= (g_Menu.Config.BodyScale / 100.f);

					Scan.push_back({ Vector(Hitbox + (Mutipoint[0] * Radius)), true });//thorax
					Scan.push_back({ Vector(Hitbox + (Mutipoint[1] * Radius)), true });
				}
			}

			if (g_Menu.Config.Hitscan[1])
			{
				if (!Autowall::get().VectortoVectorVisible(g::pLocalEntity->GetEyePosition(), pEnt->GetHitboxPosition(HITBOX_UPPER_CHEST, Matrix), pEnt))
				{
					Hitbox = pEnt->GetHitboxPosition(HITBOX_UPPER_CHEST, Matrix, &Radius);
					Radius *= (g_Menu.Config.BodyScale / 100.f);

					Scan.push_back({ Vector(Hitbox + (Mutipoint[0] * Radius)), false });//upperchest
					Scan.push_back({ Vector(Hitbox + (Mutipoint[1] * Radius)), false });
				}
			}

			HeadHeight = g_Menu.Config.HeadScale;

			if (!Baim && g_Menu.Config.Hitscan[0])
			{
				Hitbox = pEnt->GetHitboxPosition(0, LagComp::get().pMatrix[pEnt->EntIndex()], &Radius);
				Radius *= (HeadHeight / 100.f);

				if (!Autowall::get().VectortoVectorVisible(g::pLocalEntity->GetEyePosition(), pEnt->GetHitboxPosition(HITBOX_HEAD, Matrix), pEnt))
				{
					Scan.push_back({ Vector(Hitbox + (Mutipoint[0] * Radius)), false });//head
					Scan.push_back({ Vector(Hitbox + (Mutipoint[1] * Radius)), false });
				}
				Scan.push_back({ Vector(Hitbox += Vector(0, 0, Radius)), false });
			}
		}

		if (!g_Menu.Config.IgnoreLimbs)
			Velocity = 0.f;

		if (Velocity <= 29.f)
		{
			if (g_Menu.Config.Hitscan[4])
			{
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_LEFT_FOOT, Matrix), false });
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_RIGHT_FOOT, Matrix), false });
			}

			if (g_Menu.Config.Hitscan[3])
			{
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_LEFT_UPPER_ARM, Matrix), false });
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_RIGHT_UPPER_ARM, Matrix), false });
			}

			if (g_Menu.Config.Hitscan[5])
			{
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_LEFT_THIGH, Matrix), false });
				Scan.push_back({ pEnt->GetHitboxPosition(HITBOX_RIGHT_THIGH, Matrix), false });
			}
		}
	}

	int BestHitbox = -1;

	if (Scan.size() != 0)
	{
		Hitboxes[pEnt->EntIndex()] = Scan;

		for (int i = 0; i < Scan.size(); i++)
		{
			float Damage = Autowall::get().Damage(Scan.at(i).Pos);		

			if (g_Menu.Config.BaimLethal && Scan.at(i).Body && Damage >= (pEnt->GetHealth() + 10))
			{
				Damage = 400.f;
				Baim = true;
			}

			if (Damage > tempDmg)
			{
				BestHitbox = i;
				tempDmg = Damage;
			}
		}
	}

	TickInfo pPlayerEntityRecord = LagComp::get().PlayerRecord[pEnt->EntIndex()].at(0);

	Backtrack[pEnt->EntIndex()] = false;
	ShotBacktrack[pEnt->EntIndex()] = false;

	if (g_Menu.Config.ShotBacktrack && LagComp::get().ShotTick[pEnt->EntIndex()] != -1 && Autowall::get().CanHitFloatingPoint(pEnt->GetHitboxPosition(HITBOX_HEAD, LagComp::get().PlayerRecord[pEnt->EntIndex()].at(LagComp::get().ShotTick[pEnt->EntIndex()]).Matrix), g::pLocalEntity->GetEyePosition(), pEnt) && !Baim)
	{
		bestEntDmg = (1000000.f - fabs(g_Math.Distance(Vector2D(g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y), Vector2D(pEnt->GetOrigin().x, pEnt->GetOrigin().y)))); // just doing this to get the closest player im backtracking
		ShotBacktrack[pEnt->EntIndex()] = true;
		return pEnt->GetHitboxPosition(HITBOX_HEAD, LagComp::get().PlayerRecord[pEnt->EntIndex()].at(LagComp::get().ShotTick[pEnt->EntIndex()]).Matrix);
	}
	else if (tempDmg >= g_Menu.Config.Mindmg && BestHitbox != -1)
	{
		bestEntDmg = tempDmg;
		return Scan.at(BestHitbox).Pos;
	}
	else if (g_Menu.Config.PosBacktrack && pPlayerEntityRecord.Velocity >= 29.f && BestHitbox != -1)
	{
		if (g_Menu.Config.BacktrackHitbox[0] && Autowall::get().CanHitFloatingPoint(pEnt->GetHitboxPosition(HITBOX_HEAD, pPlayerEntityRecord.Matrix), g::pLocalEntity->GetEyePosition(), pEnt))
		{
			bestEntDmg = (100000.f - fabs(g_Math.Distance(Vector2D(g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y), Vector2D(pEnt->GetOrigin().x, pEnt->GetOrigin().y))));
			Backtrack[pEnt->EntIndex()] = true;
			return pEnt->GetHitboxPosition(HITBOX_HEAD, pPlayerEntityRecord.Matrix);
		}
		else if (g_Menu.Config.BacktrackHitbox[1] && Autowall::get().CanHitFloatingPoint(pEnt->GetHitboxPosition(HITBOX_PELVIS, pPlayerEntityRecord.Matrix), g::pLocalEntity->GetEyePosition(), pEnt))
		{
			bestEntDmg = (100000.f - fabs(g_Math.Distance(Vector2D(g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y), Vector2D(pEnt->GetOrigin().x, pEnt->GetOrigin().y))));
			Backtrack[pEnt->EntIndex()] = true;
			return pEnt->GetHitboxPosition(HITBOX_PELVIS, pPlayerEntityRecord.Matrix);
		}
		else
			return Vector(0, 0, 0);
	}

	return Vector(0, 0, 0);
}

void RageBot::OnCreateMove()
{
	if (!g_Menu.Config.Aimbot || g_Menu.Config.LegitBacktrack || !g_pEngine->IsInGame())
		return;

	if (g::CheatType != 1)
		return;

	static bool shot = false;

	if (!g::pLocalEntity->IsAlive())// k  lol
	{
		shot = false;
		return;
	}

	if (!g::pLocalEntity->GetActiveWeapon())// k  lol
	{
		shot = false;
		return;
	}

	if (g::pLocalEntity->IsKnifeorNade())// k  lol
	{
		shot = false;
		return;
	}

	Vector Aimpoint = { 0,0,0 };
	C_BaseEntity* Target = nullptr;

	int targetID = 0;
	int tempDmg = 0;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam()
			|| LagComp::get().PlayerRecord[i].size() == 0)
			continue;

		bestEntDmg = 0;

		Vector Hitbox = Hitscan(pPlayerEntity);

		if (Hitbox != Vector(0, 0, 0) && tempDmg <= bestEntDmg)
		{
			Aimpoint = Hitbox;
			Target = pPlayerEntity;
			targetID = Target->EntIndex();
			tempDmg = bestEntDmg;
		}
	}

	if (shot)
	{
		if (g_Menu.Config.ShotLag == 1) // ik it dosnt realy fix much just makes ur pitch go down faster
		{
			g::bSendPacket = true;
			AntiAim::get().OnCreateMove();
		}
		shot = false;
	}

	g::pLocalEntity->GetActiveWeapon()->AccuracyPenalty();
	float flServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;
	bool canShoot = (g::pLocalEntity->GetActiveWeapon()->GetNextPrimaryAttack() <= flServerTime);

	if (Target)
	{
		g::TargetIndex = targetID;

		float SimulationTime = 0.f; 

		if (Backtrack[targetID])
			SimulationTime = LagComp::get().PlayerRecord[targetID].at(0).SimTime;
		else 
			SimulationTime = LagComp::get().PlayerRecord[targetID].at(LagComp::get().PlayerRecord[targetID].size() - 1).SimTime;

		if (ShotBacktrack[targetID])
			SimulationTime = LagComp::get().PlayerRecord[targetID].at(LagComp::get().ShotTick[targetID]).SimTime;

		Vector Angle = g_Math.CalcAngle(g::pLocalEntity->GetEyePosition(), Aimpoint);

		if (g::pLocalEntity->GetVelocity().Length() >= (g::pLocalEntity->GetActiveWeapon()->GetCSWpnData()->max_speed_alt * .34f) - 5 /*;)*/ && !GetAsyncKeyState(VK_SPACE))
			Autostop();

		if (g_Menu.Config.AutoScope && g::pLocalEntity->IsSniper() && !g::pLocalEntity->IsScoped())
			g::pCmd->buttons |= IN_ATTACK2;

		if (!(g::pCmd->buttons & IN_ATTACK) && canShoot && HitChance(Target, g::pLocalEntity->GetActiveWeapon(), Angle, Aimpoint, g_Menu.Config.HitchanceValue))
		{
			if (!Backtrack[targetID] && !ShotBacktrack[targetID])
				g::Shot[targetID] = true;

			if (g_Menu.Config.ShotLag != 2 && !GetAsyncKeyState(g_Menu.Config.FakeDuckKey))
				g::bSendPacket = true;

			shot = true;

			if (g_Menu.Config.SilentAim == 0) // so dumb
				g_pEngine->SetViewAngles(Angle);

			if (g_Menu.Config.SilentAim == 2)
				Angle.x += 1080.f; // hmhmmMJMHMHM

			g::pCmd->viewangles = Angle - (g::pLocalEntity->GetAimPunchAngle() * g_pCvar->FindVar("weapon_recoil_scale")->GetFloat());
			g::pCmd->buttons |= IN_ATTACK;
			g::pCmd->tick_count = TIME_TO_TICKS(SimulationTime + LagComp::get().LerpTime());
		}
	}
}