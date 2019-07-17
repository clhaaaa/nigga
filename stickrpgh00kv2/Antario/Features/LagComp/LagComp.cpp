#include "..\Features.h"

float LagComp::LerpTime() // glad
{
	int updaterate = g_pCvar->FindVar("cl_updaterate")->GetInt();
	ConVar* minupdate = g_pCvar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = g_pCvar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetInt();

	float ratio = g_pCvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_pCvar->FindVar("cl_interp")->GetFloat();
	ConVar* cmin = g_pCvar->FindVar("sv_client_min_interp_ratio");
	ConVar* cmax = g_pCvar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = clamp(ratio, cmin->GetFloat(), cmax->GetFloat());

	return max(lerp, (ratio / updaterate));
}

bool LagComp::ValidTick(float simtime) 
{
	auto nci = g_pEngine->GetNetChannelInfo();

	if (!nci)
		return false;

	const auto sv_maxunlag = g_pCvar->FindVar("sv_maxunlag");

	auto Correct = clamp(nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING), 0.f, sv_maxunlag->GetFloat());

	return (fabs(Correct - (g_pGlobalVars->curtime - simtime)) <= 0.2f);
}

void LagComp::CreateRecord(C_BaseEntity* pEnt)
{
	TickInfo Setup;
	bool Shot;

	BYTE Baclp;

	if (g::CheatType == 1)
	{
		if (pEnt->GetActiveWeapon() && !pEnt->IsKnifeorNade())
		{
			if (ShotTime[pEnt->EntIndex()] != pEnt->GetActiveWeapon()->GetLastShotTime())
			{
				if (Setup.Pitch != pEnt->GetEyeAngles().x)
					Shot = true;
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

		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pEnt) + 0xA30) = g_pGlobalVars->framecount;
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pEnt) + 0xA28) = 0;

		Baclp = *reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(pEnt) + 0x274);
		*reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(pEnt) + 0x274) = 0;
	}

	pEnt->InvalidateBoneCache();

	pEnt->SetupBones(pMatrix[pEnt->EntIndex()], 128, 0x0007FF00, g_pGlobalVars->curtime);

	if (g::CheatType == 1)
	{
		*reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(pEnt) + 0x274) = Baclp;
	}

	Setup.Pitch = pEnt->GetEyeAngles().x;
	Setup.SimTime = pEnt->GetSimulationTime();
	Setup.Head = pEnt->GetHitboxPosition(HITBOX_HEAD, pMatrix[pEnt->EntIndex()]);
	Setup.Shot = Shot;
	Setup.Velocity = abs(pEnt->GetVelocity().Length2D());
	memcpy(Setup.Matrix, pMatrix[pEnt->EntIndex()], (sizeof(matrix3x4_t) * 128));
	PlayerRecord[pEnt->EntIndex()].push_back(Setup);
}

void LagComp::FrameStage(ClientFrameStage_t stage)
{
	if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
		return;

	C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocalEntity)
		return;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

	 	if (g::CheatType == 1 && stage == FRAME_RENDER_START && pPlayerEntity && !pPlayerEntity->IsDormant() && pPlayerEntity->GetClientClass()->ClassID == (int)EClassIds::CCSRagdoll)
			pPlayerEntity->SetSequence(1); // allow game to setupbones when the player is a ragdoll

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant())
		{
			ClearRecords(i);
			continue;
		}

		if (stage == FRAME_NET_UPDATE_END && pPlayerEntity != pLocalEntity)
		{
			auto VarMap = reinterpret_cast<uintptr_t>(pPlayerEntity) + 36;
			auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

			for (auto index = 0; index < VarMapSize; index++)
				*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0;
		}
	
		if (stage == FRAME_RENDER_START)
		{
			Resolver::get().HandleHits(pPlayerEntity, pLocalEntity);
			Resolver::get().AnimationFix(pPlayerEntity, pLocalEntity);

			if (PlayerRecord[i].size() == 0 || (g::CheatType == 1 && !g_Menu.Config.FakeLagModel && pPlayerEntity == pLocalEntity))
				CreateRecord(pPlayerEntity);
			else if ((PlayerRecord[i].size() != 0) && PlayerRecord[i].at(PlayerRecord[i].size() - 1).SimTime != pPlayerEntity->GetSimulationTime())
				CreateRecord(pPlayerEntity);

			if (g::CheatType == 1 && pPlayerEntity != pLocalEntity)
				pPlayerEntity->SetSequence(-1); // dont allow the game to setup bones for other entitys *rly cancer and most likely very inproper and fux bouding box

			ShotTick[i] = -1;

			if (PlayerRecord[i].size() != 0)
				for (int tick = 0; tick < PlayerRecord[i].size(); tick++)
					if (!ValidTick(PlayerRecord[i].at(tick).SimTime))
						PlayerRecord[i].erase(PlayerRecord[i].begin() + tick);
					else if (g::CheatType == 1 && PlayerRecord[i].at(tick).Shot)
						ShotTick[i] = tick;
		}
		
	}
}

void LagComp::ClearRecords(int i)
{
	if (PlayerRecord[i].size() > 0)
		for (int tick = 0; tick < PlayerRecord[i].size(); tick++)
			PlayerRecord[i].erase(PlayerRecord[i].begin() + tick);

	ShotTick[i] = -1;
	ShotTime[i] = 0.f;
}

template<class T, class U>
T LagComp::clamp(T in, U low, U high) // just a clamping func but didnt feel like making one so stole it from glad
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}