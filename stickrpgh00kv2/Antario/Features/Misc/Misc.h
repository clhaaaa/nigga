#pragma once


// kinda just throw shit in here

#define _SOLVEY(a, b, c, d, e, f) ((c * b - d * a) / (c * f - d * e))
#define SOLVEY(...) _SOLVEY(?, ?, ?, ?, ?, ?)
#define SOLVEX(y, world, forward, right) ((world.x - right.x * y) / forward.x)

class Misc : public singleton<Misc> // singleton ftw
{
public:
    void OnCreateMove()
    {
        this->pCmd   = g::pCmd;
        this->pLocal = g::pLocalEntity;

		this->SpoofSvCheats();
		this->DisablePostPro();
		this->NightMode();
		this->CircleStrafe();
		this->DoAutostrafe();
		this->DoBhop();
		this->DoFakeLag();
		this->FakeDuck();
		this->Walkbot();
    };

	void SpoofSvCheats()
	{
		if (g::CheatType != 1)
			return;

		ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");

		if (sv_cheats->GetInt() != 1)
		{
			*reinterpret_cast<int*>((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0;
			sv_cheats->SetValue(1);
		}
	}

	void MovementFix(Vector& oldang) // i think osmium
	{
		if (g::CheatType != 1)
			return;

		Vector vMovements(g::pCmd->forwardmove, g::pCmd->sidemove, 0.f);

		if (vMovements.Length2D() == 0)
			return;

		Vector vRealF, vRealR;
		Vector aRealDir = g::pCmd->viewangles;
		aRealDir.Clamp();

		g_Math.AngleVectors(aRealDir, &vRealF, &vRealR, nullptr);
		vRealF[2] = 0;
		vRealR[2] = 0;

		VectorNormalize(vRealF);
		VectorNormalize(vRealR);

		Vector aWishDir = oldang;
		aWishDir.Clamp();

		Vector vWishF, vWishR;
		g_Math.AngleVectors(aWishDir, &vWishF, &vWishR, nullptr);

		vWishF[2] = 0;
		vWishR[2] = 0;

		VectorNormalize(vWishF);
		VectorNormalize(vWishR);

		Vector vWishVel;
		vWishVel[0] = vWishF[0] * g::pCmd->forwardmove + vWishR[0] * g::pCmd->sidemove;
		vWishVel[1] = vWishF[1] * g::pCmd->forwardmove + vWishR[1] * g::pCmd->sidemove;
		vWishVel[2] = 0;

		float a = vRealF[0], b = vRealR[0], c = vRealF[1], d = vRealR[1];
		float v = vWishVel[0], w = vWishVel[1];

		float flDivide = (a * d - b * c);
		float x = (d * v - b * w) / flDivide;
		float y = (a * w - c * v) / flDivide;

		g::pCmd->forwardmove = x;
		g::pCmd->sidemove = y;
	}

	void NightMode() //credit for mat_force_tonemap_scale - altugsara123 https://www.unknowncheats.me/forum/counterstrike-global-offensive/321261-friendly-easiest-nightmode-method.html
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
			return;

		if (g::CheatType != 1)
			return;

		ConVar* mat_force_tonemap_scale = g_pCvar->FindVar("mat_force_tonemap_scale");

		if (g_Menu.Config.NightMode)
		{
			if (mat_force_tonemap_scale->GetFloat() != 0.2f)
				g_pEngine->ExecuteClientCmd("mat_force_tonemap_scale 0.2");		

			static float storedCurtime = g_pGlobalVars->curtime;

			double timeSoFar = abs(g_pGlobalVars->curtime - storedCurtime);

			static int choose = 1;

			srand(time(NULL));

			if (timeSoFar > 11.0)
			{
				switch (choose)
				{
				case 1:g_pEngine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder4.wav"); break;
				case 2:g_pEngine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder5.wav"); break;
				case 3:g_pEngine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder6.wav"); break;
				case 4:g_pEngine->ExecuteClientCmd("play ambient\\playonce\weather\\thunder_distant_01.wav"); break;
				case 5:g_pEngine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_02.wav"); break;
				case 6:g_pEngine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_06.wav"); break;
				}
				storedCurtime = g_pGlobalVars->curtime;
			}
			else
			{
				int randomnum = 1 + (rand() % 6);

				if (randomnum == choose)
					choose = 1 + (rand() % 6); // could still be the same number but less likely
				else
					choose = randomnum;
			}
		}
		else if (!g_Menu.Config.NightMode && mat_force_tonemap_scale->GetFloat() != 1.f)
				g_pEngine->ExecuteClientCmd("mat_force_tonemap_scale 1");

	}

	void FakeDuck()
	{
		if (g::CheatType != 1)
			return;

		static bool SwitchCrouch = true;
		static bool init = true;
		if (!GetAsyncKeyState(g_Menu.Config.FakeDuckKey))
		{
			SwitchCrouch = true;
			init = true;
			return;
		}

		if (!g::pLocalEntity->IsAlive())
			return;

		auto Animstate = g::pLocalEntity->AnimState();
		if (Animstate->m_bInHitGroundAnimation && Animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation != 1.f)
			return;

		auto NetChannel = g_pEngine->GetNetChannel();
		if (!NetChannel)
			return;

		float DuckAmnt = g::pLocalEntity->GetDuckAmount();

		bool skip = false;

		if (init)
		{
			skip = true;
			g::bSendPacket = true;
			init = false;
		}

		if (SwitchCrouch && !skip)
		{
			g::pCmd->buttons |= IN_DUCK;
			if (DuckAmnt >= 0.799999f)
			{
				g::bSendPacket = true;
				SwitchCrouch = false;
			}
			else
				g::bSendPacket = false;
		}
		else if (!skip)
		{
			g::bSendPacket = false;
			g::pCmd->buttons &= ~IN_DUCK;

			if ((DuckAmnt + .05f) <= 0.29999999f)
				SwitchCrouch = true;
		}
	}

	void DisablePostPro()
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
			return;

		if (g::CheatType != 1)
			return;

		ConVar* mat_postprocess_enable = g_pCvar->FindVar("mat_postprocess_enable");

		if (g_Menu.Config.PostProcess && mat_postprocess_enable->GetInt() != 0)
			g_pEngine->ExecuteClientCmd("mat_postprocess_enable 0");
		else if (!g_Menu.Config.PostProcess && mat_postprocess_enable->GetInt() != 1)
			g_pEngine->ExecuteClientCmd("mat_postprocess_enable 1");
	}

	void ThirdPerson(ClientFrameStage_t curStage)
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
			return;
		
		if (g::CheatType != 1)
			return;

		C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		if (!pLocalEntity)
			return;

		static bool init = false;

		if (GetKeyState(g_Menu.Config.ThirdPersonKey) && g::pLocalEntity->IsAlive())
		{
			if (init)
				g_pEngine->ExecuteClientCmd("thirdperson");
			init = false;
		}
		else
		{
			if (!init)
				g_pEngine->ExecuteClientCmd("firstperson");
			init = true;
		}

		if (curStage == FRAME_RENDER_START && GetKeyState(g_Menu.Config.ThirdPersonKey) && g::pLocalEntity->IsAlive())
			g_pPrediction->SetLocalViewAngles(Vector(g::RealAngle.x, g::RealAngle.y, 0)); // lol
	}

	void Crosshair()
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
			return;

		if (g::CheatType != 1)
			return;

		C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		if (!pLocalEntity)
			return;

		if (!pLocalEntity->IsAlive())
			return;
		
		if (pLocalEntity->IsScoped() && g_Menu.Config.NoScope)
		{
			int Height, Width;
			g_pEngine->GetScreenSize(Width, Height);

			Vector punchAngle = pLocalEntity->GetAimPunchAngle();

			float x = Width / 2;
			float y = Height / 2;
			int dy = Height / 90;
			int dx = Width / 90;
			x -= (dx*(punchAngle.y));
			y += (dy*(punchAngle.x));

			Vector2D screenPunch = { x, y };

			g_pRender->Line(0, screenPunch.y, Width, screenPunch.y, Color(0, 0, 0, 255));
			g_pRender->Line(screenPunch.x, 0, screenPunch.x, Height, Color(0, 0, 0, 255));
		}

		static bool init = false;
		static bool init2 = false;

		if (g_Menu.Config.Crosshair)
		{
			if (pLocalEntity->IsScoped())
			{
				if (init2)
				{
					g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 0");
					g_pEngine->ExecuteClientCmd("crosshair 0");
				}
				init2 = false;
			}
			else
			{
				if (!init2)
				{
					g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 3");
					g_pEngine->ExecuteClientCmd("crosshair 1");
				}
				init2 = true;
			}

			init = false;
		}
		else
		{
			if (!init)
			{
				g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 0");
				g_pEngine->ExecuteClientCmd("crosshair 1");
			}
			init = true;
		}
	}


	void NormalWalk() // heh
	{
		if (g::CheatType != 1)
			return;

		g::pCmd->buttons &= ~IN_MOVERIGHT;
		g::pCmd->buttons &= ~IN_MOVELEFT;
		g::pCmd->buttons &= ~IN_FORWARD;
		g::pCmd->buttons &= ~IN_BACK;

		if (g::pCmd->forwardmove > 0.f)
			g::pCmd->buttons |= IN_FORWARD;
		else if (g::pCmd->forwardmove < 0.f)
			g::pCmd->buttons |= IN_BACK;
		if (g::pCmd->sidemove > 0.f)
		{
			g::pCmd->buttons |= IN_MOVERIGHT;
		}
		else if (g::pCmd->sidemove < 0.f)
		{
			g::pCmd->buttons |= IN_MOVELEFT;
		}
	}

	void Walkbot() // wip
	{
		if (!g_Menu.Config.WalkBot || !g::pLocalEntity->IsAlive())
			return;

		if (g::CheatType != 1)
			return;

		float BestDistance = 0.f;
		float BestAngle = -9999999.f;

		g::pCmd->forwardmove = 450.f;

		C_TraceFilter filter1(g::pLocalEntity);
		Vector Origin = g::pLocalEntity->GetOrigin() + Vector(0, 0, 10);
	
		Vector EyeAngle;
		g_pEngine->GetViewAngles(EyeAngle);


		C_Trace TraceFront;
		g_pTrace->TraceRay(C_Ray(Origin, g_Math.ProjectPoint(Origin, g_Math.NormalizeYaw((EyeAngle.y + 90)), 40)), mask_solid, &filter1, &TraceFront);
		 
		static bool jumpedb4 = false;

		if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND))
			pCmd->buttons |= IN_DUCK;
		else
			jumpedb4 = false;

		if (TraceFront.flFraction != 1.0f && (g::pLocalEntity->GetFlags() & FL_ONGROUND) && !jumpedb4)
		{
			pCmd->buttons |= IN_JUMP;
			jumpedb4 = true;
		}

		static bool RandomDirect = true;
		RandomDirect = false;
		srand(time(NULL));
		int RandomStart = rand() % 36;
		int i = RandomStart;
		bool DoubleStart = false;
		bool Loop = true;
		while (Loop)
		{
			i += RandomDirect ? 1 : -1;

			if (RandomDirect && i > 36)
			{
				i = 0;
				DoubleStart = true;
			}
			else if (!RandomDirect && i < 0)
			{
				i = 36;
				DoubleStart = true;
			}

			if (RandomDirect && i >= RandomStart && DoubleStart)
			{
				Loop = false;
				break;
			}
			else if (!RandomDirect && i <= RandomStart && DoubleStart)
			{
				Loop = false;
				break;
			}

			C_Trace TraceInit;
			g_pTrace->TraceRay(C_Ray(Origin, g_Math.ProjectPoint(Origin, g_Math.NormalizeYaw(i * 10), 300) + Vector(0, 0, 90)), mask_solid, &filter1, &TraceInit);

			if (TraceInit.flFraction > BestDistance)
			{
				BestAngle = g_Math.NormalizeYaw(i * 10);
				BestDistance = TraceInit.flFraction;
			}
		}

		BestAngle = g_Math.NormalizeYaw(BestAngle - 90);

		float Delta = g_Math.NormalizeYaw(BestAngle - EyeAngle.y);

		if (Delta >= 0)
		{
			EyeAngle.y = g_Math.NormalizeYaw(EyeAngle.y + ((abs(Delta) <= 5) ? abs(Delta) : 5));
		}
		else
		{
			EyeAngle.y = g_Math.NormalizeYaw(EyeAngle.y - ((abs(Delta) <= 5) ? abs(Delta) : 5));
		}

		C_Trace TraceLeft;
		g_pTrace->TraceRay(C_Ray(Origin, g_Math.ProjectPoint(Origin, g_Math.NormalizeYaw((EyeAngle.y - 90) - 45), 300)), mask_solid, &filter1, &TraceLeft);
		C_Trace TraceRight;
		g_pTrace->TraceRay(C_Ray(Origin, g_Math.ProjectPoint(Origin, g_Math.NormalizeYaw((EyeAngle.y - 90) + 45), 300)), mask_solid, &filter1, &TraceRight);

		if (TraceLeft.flFraction > TraceRight.flFraction)
		{
			g::pCmd->sidemove = -100.f;
		}
		else
		{
			g::pCmd->sidemove = 100.f;
		}

		EyeAngle.x = 0.f;

		g_pEngine->SetViewAngles(EyeAngle);
	}

	void CircleStrafe()
	{
		if (g::CheatType != 1)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;

		static float StAngle = 0;

		if (GetAsyncKeyState(g_Menu.Config.CircleStrafeKey))
		{
			StAngle += g_pGlobalVars->intervalPerTick * 360.f;
			g::OriginalView.y = g_Math.NormalizeYaw(StAngle);
			g::pCmd->buttons |= IN_JUMP;
		}
		else
		{
			StAngle = g::OriginalView.y;
		}

	}

private:
    CUserCmd*     pCmd;
    C_BaseEntity* pLocal;

    void DoBhop() const
    {
        if (!g_Menu.Config.Bhop)
            return;

		if (!g::pLocalEntity->IsAlive())
			return;

        static bool bLastJumped = false;
        static bool bShouldFake = false;

        if (!bLastJumped && bShouldFake)
        {
            bShouldFake = false;
            pCmd->buttons |= IN_JUMP;
        }
        else if (pCmd->buttons & IN_JUMP)
        {
            if (pLocal->GetFlags() & FL_ONGROUND)
                bShouldFake = bLastJumped = true;
            else 
            {
                pCmd->buttons &= ~IN_JUMP;
                bLastJumped = false;
            }
        }
        else
            bShouldFake = bLastJumped = false;
    }

	void DoAutostrafe() const
	{
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !g_Menu.Config.AutoStrafe)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;

		if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND) && (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(g_Menu.Config.CircleStrafeKey)))
		{
			pCmd->forwardmove = (10000.f / g::pLocalEntity->GetVelocity().Length2D() > 450.f) ? 450.f : 10000.f / g::pLocalEntity->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->mousedx != 0 && !GetAsyncKeyState(g_Menu.Config.CircleStrafeKey)) ? (pCmd->mousedx < 0.0f) ? -450.f : 450.f : (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
		}
	}

	void DoFakeLag() const
	{
		if (g_Menu.Config.LegitBacktrack || !g_pEngine->IsConnected() || !g_pEngine->IsInGame() || (g_Menu.Config.StandingLag == 0 && g_Menu.Config.MovingLag == 0 && g_Menu.Config.InAirLag == 0))
			return;

		if (g::CheatType != 1)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;

		if (g::pLocalEntity->IsKnifeorNade())
			return;

		auto NetChannel = g_pEngine->GetNetChannel();

		if (!NetChannel)
			return;

		static bool Hitinit = false;
		bool SkipTick = false;
		static bool canHit = false;
		bool HitCheck = false;

		if (g_Menu.Config.FakeLagOnPeek && LagComp::get().pMatrix[g::pLocalEntity->EntIndex()])
		{
			Vector Head = { g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y, (g::pLocalEntity->GetHitboxPosition(0, LagComp::get().pMatrix[g::pLocalEntity->EntIndex()]).z + 10.f) };
			Vector HeadExtr = (Head + (g::pLocalEntity->GetVelocity() * 0.21875f));
			Vector OriginExtr = ((g::pLocalEntity->GetOrigin() + (g::pLocalEntity->GetVelocity() * 0.21875f)) + Vector(0, 0, 8));

			for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
			{
				C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

				if (!pPlayerEntity
					|| !pPlayerEntity->IsAlive()
					|| pPlayerEntity->IsDormant()
					|| pPlayerEntity == g::pLocalEntity
					|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam()
					|| !LagComp::get().pMatrix[pPlayerEntity->EntIndex()])
					continue;

				Vector EnemyHead = { pPlayerEntity->GetOrigin().x, pPlayerEntity->GetOrigin().y, (pPlayerEntity->GetHitboxPosition(0, LagComp::get().pMatrix[pPlayerEntity->EntIndex()]).z + 10.f) };

				if (abs(g::pLocalEntity->GetVelocity().Length2D()) > .1f && (Autowall::get().CanHitFloatingPoint(HeadExtr, EnemyHead) || Autowall::get().CanHitFloatingPoint(OriginExtr, EnemyHead)))
				{
					if (!Hitinit)
					{
						canHit = true;
						Hitinit = true;
					}
					HitCheck = true;
				}
			}

			if (!HitCheck)
				Hitinit = false;

			if (canHit)
			{
				g::bSendPacket = true;
				SkipTick = true;
				g::LagPeek = true;
				canHit = false;
			}

			if (g::LagPeek && !SkipTick)
			{
				if (NetChannel->m_nChokedPackets < 14)
					g::bSendPacket = false;
				else
					g::LagPeek = false;
			}

			if (!g::LagPeek && !SkipTick)
			{
				if (abs(g::pLocalEntity->GetVelocity().Length2D()) < .1f && (g::pLocalEntity->GetFlags() & FL_ONGROUND))
					g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.StandingLag);
				else if (abs(g::pLocalEntity->GetVelocity().Length2D()) >= .1f && (g::pLocalEntity->GetFlags() & FL_ONGROUND))
					g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.MovingLag);
				else if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND))
					g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.InAirLag);
			}

		}
		else
		{
			Hitinit = false;
			SkipTick = false;
			canHit = false;
			g::LagPeek = false;

			if (abs(g::pLocalEntity->GetVelocity().Length2D()) < .1f && (g::pLocalEntity->GetFlags() & FL_ONGROUND))
				g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.StandingLag);
			else if (abs(g::pLocalEntity->GetVelocity().Length2D()) >= .1f && (g::pLocalEntity->GetFlags() & FL_ONGROUND))
				g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.MovingLag);
			else if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND))
				g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.InAirLag);
		}

	}
};