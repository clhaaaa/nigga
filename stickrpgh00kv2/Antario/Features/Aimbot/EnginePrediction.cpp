#include "..\Features.h"

//outlassn https://www.unknowncheats.me/forum/counterstrike-global-offensive/235537-reversed-runcommand-engine-prediction.html

CMoveData m_MoveData;
static char* cMoveData = nullptr;

void EnginePrediction::Start() // this shit p
{
	if (!g_pMoveHelper)
		return;

	if (g::CheatType != 1)
		return;

	iFlagsBackup = g::pLocalEntity->GetFlags();
	iButtonsBackup = g::pCmd->buttons;

	g::pLocalEntity->SetCurrentCommand(g::pCmd);

	static bool bInit = false;
	if (!bInit) {
		m_pPredictionRandomSeed = *reinterpret_cast<int**>(Utils::FindSignature("client_panorama.dll", "A3 ? ? ? ? 66 0F 6E 86") + 1);
		m_pSetPredictionPlayer = *reinterpret_cast<int**>(Utils::FindSignature("client_panorama.dll", "89 35 ? ? ? ? F3 0F 10 48 20") + 2);
		bInit = true;
	}

	if (!cMoveData)
		cMoveData = reinterpret_cast<char*>(calloc(1, sizeof(CMoveData)));

	g_pMoveHelper->SetHost(g::pLocalEntity);
	*m_pPredictionRandomSeed = MD5_PseudoRandom(g::pCmd->command_number) & 0x7FFFFFFF;
	*m_pSetPredictionPlayer = reinterpret_cast<uintptr_t>(g::pLocalEntity);

	g_pGlobalVars->curtime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;
	g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

	// all of these offsets are prob outdated but im 2 dumb to find them

	g::pCmd->buttons |= *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x3310);

	if (g::pCmd->impulse)
		*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31EC) = g::pCmd->impulse;

	int buttonsChanged = g::pCmd->buttons ^ *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31E8);
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31DC) = (reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31E8);
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31E8) = g::pCmd->buttons;
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31E0) = g::pCmd->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(g::pLocalEntity) + 0x31E4) = buttonsChanged & ~g::pCmd->buttons; //m_afButtonReleased ~ The ones not down are "released"

	g_pMovement->StartTrackPredictionErrors(g::pLocalEntity);

	iTickBaseBackup = g::pLocalEntity->GetTickBase();

	g_pPrediction->SetupMove(g::pLocalEntity, g::pCmd, g_pMoveHelper, reinterpret_cast<CMoveData*>(cMoveData));
	g_pMovement->ProcessMovement(g::pLocalEntity, reinterpret_cast<CMoveData*>(cMoveData));
	g_pPrediction->FinishMove(g::pLocalEntity, g::pCmd, reinterpret_cast<CMoveData*>(cMoveData));

	g::pLocalEntity->SetTickBase(iTickBaseBackup);

	g_pMovement->FinishTrackPredictionErrors(g::pLocalEntity);

	g::pLocalEntity->SetCurrentCommand(nullptr);
	*m_pPredictionRandomSeed = -1;
	*m_pSetPredictionPlayer = 0;
	g_pMoveHelper->SetHost(0);

	g::pLocalEntity->GetFlags() = iFlagsBackup;
	g::pCmd->buttons = iButtonsBackup;
}

void EnginePrediction::End()
{
	if (g::CheatType != 1)
		return;
}