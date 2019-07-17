#include <thread>
#include "Hooks.h"
#include "Features\Features.h"
#include "SDK\Hitboxes.h"
#include "libloaderapi.h"
#include <intrin.h>

Hooks    g_Hooks;
Event    g_Event;

void Warning(const char* msg, ...) // wintergang https://www.unknowncheats.me/forum/1923881-post1.html
{
	if (msg == nullptr)
		return;
	typedef void(__cdecl* MsgFn)(const char* msg, va_list);
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Warning");
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list);
}

void Hooks::Init()
{
    // Get window handle
    while (!(g_Hooks.hCSGOWindow = FindWindowA("Valve001", nullptr)))
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(50ms);
    }

    interfaces::Init();                         // Get interfaces
    g_pNetvars = std::make_unique<NetvarTree>();// Get netvars after getting interfaces as we use them

    if (g_Hooks.hCSGOWindow)        // Hook WNDProc to capture mouse / keyboard input
        g_Hooks.pOriginalWNDProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC,
                                                                              reinterpret_cast<LONG_PTR>(g_Hooks.WndProc)));

    // VMTHooks
	g_Hooks.pClientHook     = std::make_unique<VMTHook>(g_pClientDll);
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);
    g_Hooks.pSurfaceHook	= std::make_unique<VMTHook>(g_pSurface);
	g_Hooks.pPanelHook		= std::make_unique<VMTHook>(g_pPanel);
	g_Hooks.pModelHook      = std::make_unique<VMTHook>(g_pModelRender);
	g_Hooks.pRenderViewHook = std::make_unique<VMTHook>(g_pRenderView);

    // Hook the table functions
	g_Hooks.pClientHook    ->Hook(vtable_indexes::frameStage, Hooks::FrameStageNotify);
	g_Hooks.pClientModeHook->Hook(vtable_indexes::createMove, Hooks::CreateMove);
	g_Hooks.pClientModeHook->Hook(vtable_indexes::view, Hooks::OverrideView);
    g_Hooks.pSurfaceHook   ->Hook(vtable_indexes::lockCursor, Hooks::LockCursor);
	g_Hooks.pPanelHook	   ->Hook(vtable_indexes::paint, Hooks::PaintTraverse);
	g_Hooks.pModelHook	   ->Hook(vtable_indexes::dme, Hooks::DrawModelExecute);

//	g_Hooks.pModelHook	   ->Hook(vtable_indexes::dms, Hooks::DrawModelSetup);
//	g_Hooks.pRenderViewHook->Hook(vtable_indexes::sceneEnd, Hooks::SceneEnd);

	g_Event.Init();

	g::CourierNew = g_pSurface->FontCreate();
	g_pSurface->SetFontGlyphSet(g::CourierNew, "Courier New", 14, 300, 0, 0, FONTFLAG_OUTLINE);
	g::Tahoma = g_pSurface->FontCreate();
	g_pSurface->SetFontGlyphSet(g::Tahoma, "Tahoma", 12, 700, 0, 0, FONTFLAG_DROPSHADOW);

    Utils::Log("Hooking completed!");
}

void Hooks::Restore()
{
	Utils::Log("Unhooking in progress...");
    {   // Unhook every function we hooked and restore original one
		g_Hooks.pClientHook->Unhook(vtable_indexes::frameStage);
        g_Hooks.pClientModeHook->Unhook(vtable_indexes::createMove);
		g_Hooks.pClientModeHook->Unhook(vtable_indexes::view);
        g_Hooks.pSurfaceHook->Unhook(vtable_indexes::lockCursor);
		g_Hooks.pPanelHook->Unhook(vtable_indexes::paint);
		g_Hooks.pModelHook->Unhook(vtable_indexes::dme);

        SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_Hooks.pOriginalWNDProc));

        g_pNetvars.reset();   /* Need to reset by-hand, global pointer so doesnt go out-of-scope */
    }
    Utils::Log("Unhooking succeded!");
}

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage)
{
	static auto oFrameStage = g_Hooks.pClientHook->GetOriginal<FrameStageNotify_t>(vtable_indexes::frameStage);

	if (g::CheatType != -1)
	{
		Misc::get().ThirdPerson(curStage);
		//	Resolver::get().FrameStage(curStage);
		LagComp::get().FrameStage(curStage);
	}

	oFrameStage(curStage);
}

C_BaseEntity* UTIL_PlayerByIndex(int index)
{
	typedef C_BaseEntity*(__fastcall* PlayerByIndex)(int);
	static PlayerByIndex UTIL_PlayerByIndex = reinterpret_cast<PlayerByIndex>(Utils::FindSignature("server.dll", "85 C9 7E 2A A1"));

	if (!UTIL_PlayerByIndex)
		return false;

	return UTIL_PlayerByIndex(index);
}

bool __fastcall Hooks::CreateMove(IClientMode* thisptr, void* EDX, float sample_frametime, CUserCmd* pCmd)
{
	static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(vtable_indexes::createMove);

	static bool init = false;

    if (!pCmd || !pCmd->command_number)
		return oCreateMove(thisptr, EDX, sample_frametime, pCmd);

    // Get globals
    g::pCmd         = pCmd;
    g::pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	g::tickCount	= pCmd->tick_count;
	g::bSendPacket  = true;

	if (!g::pLocalEntity)
		return oCreateMove(thisptr, EDX, sample_frametime, pCmd);

	if (g::CheatType == -1)
		return false;

	uintptr_t *framePtr;
	__asm mov framePtr, ebp;

	g::OriginalView = g::pCmd->viewangles;

	Misc::get().OnCreateMove();

//	Resolver::get().OnCreateMove();

	EnginePrediction::get().Start();
	AntiAim::get().OnCreateMove();
	RageBot::get().OnCreateMove();

	Legitbot::get().OnCreateMove();

	Misc::get().MovementFix(g::OriginalView);

	if (g_Menu.Config.AntiUntrusted)
		g_Math.Clamp(g::pCmd->viewangles);

	if (g::bSendPacket)
		g::RealAngle = g::pCmd->viewangles;

	if (g::CheatType == 1)
	{
		*(bool*)(*framePtr - 0x1C) = g::bSendPacket;
		g::pCmd->buttons |= IN_BULLRUSH; // hehe
	}

    return false;
}

bool resetMaterials = true;

void Event::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (g::CheatType != 1)
		return;

	if (strstr(event->GetName(), "round_start") || strstr(event->GetName(), "round_start") || strstr(event->GetName(), "round_prestart") || strstr(event->GetName(), "client_disconnect") || strstr(event->GetName(), "cs_game_disconnected")) // trying everything
	{
		resetMaterials = true;
		return;
	}

	C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocalEntity)
	{
		resetMaterials = true;
		return;
	}

	if (strstr(event->GetName(), "player_death"))
	{
		auto attacker = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(event->GetInt("attacker")));

		if (attacker)
		{
			if (attacker == pLocalEntity)
			{
				int index = g_pEngine->GetPlayerForUserID(event->GetInt("userid"));
				g::Hit[index] = true;
			}
		}
	}

	if (strstr(event->GetName(), "player_hurt"))
	{
		auto attacker = g_pEntityList->GetClientEntity(g_pEngine->GetPlayerForUserID(event->GetInt("attacker")));

		if (attacker)
		{
			if (attacker == pLocalEntity)
			{
				int index = g_pEngine->GetPlayerForUserID(event->GetInt("userid"));

			//	C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(index);

			//	if (pPlayerEntity && pPlayerEntity->IsPlayer())
			//	{
				//	PlayerInfo_t pInfo;
				//	g_pEngine->GetPlayerInfo(index, &pInfo);

					g::Hit[index] = true;
			//	}

				g_pEngine->ExecuteClientCmd("play physics\\metal\\paintcan_impact_hard3.wav");
			}
		}
	}
};

void __fastcall Hooks::SceneEnd(void *ECX, void *EDX)
{
	static auto oSceneEnd = g_Hooks.pRenderViewHook->GetOriginal<SceneEnd_t>(vtable_indexes::sceneEnd);
	oSceneEnd(ECX, EDX);

	static IMaterial* Material = nullptr;

	if (g::CheatType == -1 || g::CheatType == 1)
		return;

	if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
		return;

	C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocalEntity)
	{
		Material = g_pMaterialSys->FindMaterial("FlatChams", "Model textures");
		return;
	}

	if (resetMaterials)
	{
		Material = g_pMaterialSys->FindMaterial("FlatChams", "Model textures");
		resetMaterials = false;
		return;
	}

	if (Material && g_Menu.Config.Chams)
	{
		for (int i = 1; i <= g_pEngine->GetMaxClients(); ++i)
		{
			C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

			if (!pPlayerEntity
				|| !pPlayerEntity->IsPlayer()
				|| !pPlayerEntity->IsAlive()
				|| pPlayerEntity->IsDormant()
				|| pPlayerEntity->GetTeam() == pLocalEntity->GetTeam())
				continue;

			Material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
			Material->AlphaModulate(1.f);
			Material->ColorModulate(g_Menu.Config.ChamsColor.red / 255.f, g_Menu.Config.ChamsColor.green / 255.f, g_Menu.Config.ChamsColor.blue / 255.f);

			g_pModelRender->ForcedMaterialOverride(Material);
			pPlayerEntity->DrawModel(0x1, pPlayerEntity->GetModelInstance());
			g_pModelRender->ForcedMaterialOverride(nullptr);
		}
	}
}

/*
enum Hitbox
{
	HEAD = 8,
	BODY = 6,
	RIGHT_HAND = 39,
	LEFT_HAND = 13,
	RIGHT_LEG = 73,
	LEFT_LEG = 66,
	RIGHT_FOOT = 74,
	LEFT_FOOT = 67
};
*/

bool __fastcall Hooks::DrawModelSetup(void* ECX, void* EDX, int* unk1, ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t** ppBoneToWorldOut)
{
	static auto oDrawModelSetup = g_Hooks.pModelHook->GetOriginal<DrawModelSetup_t>(vtable_indexes::dms);

	if (g::CheatType == -1 || g::CheatType != 1)
		return oDrawModelSetup(ECX, unk1, pInfo, pState, ppBoneToWorldOut);

	const char* ModelName = g_pModelInfo->GetModelName((model_t*)pInfo.pModel);

	C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(pInfo.index);

	bool ret;

	if (pPlayerEntity && pPlayerEntity->EntIndex() != g_pEngine->GetLocalPlayer() && pPlayerEntity->IsAlive() && !pPlayerEntity->IsDormant() && LagComp::get().pMatrix[pInfo.index] && LagComp::get().PlayerRecord[pInfo.index].size() != 0 && strstr(ModelName, "models/player"))
	{
		pPlayerEntity->SetSequence(LagComp::get().Sequence[pPlayerEntity->EntIndex()]); 
		ret = oDrawModelSetup(ECX, unk1, pInfo, pState, ppBoneToWorldOut);
		pPlayerEntity->SetSequence(-1);
		*ppBoneToWorldOut = LagComp::get().pMatrix[pInfo.index];
/*
		DrawModelState_t &state = *pState;

		if (state.m_pStudioHdr->numbodyparts == 0)
			return false;

		if (!pInfo.pModelToWorld)
		{
			return false;
		}

		state.m_pStudioHWData = g_pIMDLCache->GetHardwareData((MDLHandle_t)pInfo.pModel->studio);
		if (!state.m_pStudioHWData)
			return false;


		ppBoneToWorldOut = LagComp::get().pMatrix[pInfo.index];
		*/
		return ret;
	}
	else
		return oDrawModelSetup(ECX, unk1, pInfo, pState, ppBoneToWorldOut);
}

void __fastcall Hooks::DrawModelExecute(void* ECX, void* EDX, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto oDrawModelExecute = g_Hooks.pModelHook->GetOriginal<DrawModelExecute_t>(vtable_indexes::dme);

	if (g::CheatType == -1 || g::CheatType != 1)
	{
		oDrawModelExecute(ECX, context, state, info, matrix);
		return;
	}

	bool OriginalModel = true;
	const char* ModelName = g_pModelInfo->GetModelName((model_t*)info.pModel);

	C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(info.index);

	if (g_Menu.Config.WeaponChange && strstr(ModelName, "models/weapons") && !strstr(ModelName, "models/player") && !strstr(ModelName, "models/weapons/v_models/arms"))
	{
		OriginalModel = false;
		float color[3] = { g_Menu.Config.WeaponColor.red, g_Menu.Config.WeaponColor.green, g_Menu.Config.WeaponColor.blue };
		static float clr[3];
		g_pRenderView->GetColorModulation(clr);
		g_pRenderView->SetColorModulation(color);
		g_pRenderView->SetBlend(1.f);
		oDrawModelExecute(ECX, context, state, info, matrix);
		g_pRenderView->SetColorModulation(clr);
		return;
	}

	if (g_Menu.Config.ViewModelChange && strstr(ModelName, "models/weapons/v_models/arms") && !strstr(ModelName, "models/player"))
	{
		OriginalModel = false;
		float color[3] = { g_Menu.Config.ViewModelColor.red, g_Menu.Config.ViewModelColor.green, g_Menu.Config.ViewModelColor.blue };
		g_pRenderView->SetColorModulation(color);
		g_pRenderView->SetBlend(1.f);
		oDrawModelExecute(ECX, context, state, info, matrix);
		return;
	}

	if (pPlayerEntity && pPlayerEntity->IsAlive() && !pPlayerEntity->IsDormant() && LagComp::get().pMatrix[info.index] && LagComp::get().PlayerRecord[info.index].size() != 0 && strstr(ModelName, "models/player"))
	{
		OriginalModel = false;

		matrix3x4_t* PlMAtrix = LagComp::get().PlayerRecord[info.index].at(LagComp::get().PlayerRecord[info.index].size() - 1).Matrix;

		C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		if ( g_Menu.Config.HeadStealer && pLocalEntity && pLocalEntity->IsAlive() && pLocalEntity != pPlayerEntity) // head stealer mode ;)
		{
			Vector ViewAngles, ViewForward;
			g_pEngine->GetViewAngles(ViewAngles);

			g_Math.AngleVectors(ViewAngles, &ViewForward);
			g_Math.NormalizeAngles(ViewForward);

			ViewForward = pLocalEntity->GetEyePosition() + (ViewForward * 40);

			PlMAtrix[8].SetOrigin(ViewForward);
		}

		oDrawModelExecute(ECX, context, state, info, PlMAtrix);
		return;
	}

	if (strstr(ModelName, "models/player/contactshadow"))
	{
		OriginalModel = false;
		return;
	}

	if (OriginalModel)
		oDrawModelExecute(ECX, context, state, info, matrix);
}

static bool startUp = true;

void __fastcall Hooks::PaintTraverse(PVOID pPanels, int EDX, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	static auto oPaintTraverse = g_Hooks.pPanelHook->GetOriginal<PaintTraverse_t>(vtable_indexes::paint);
	static unsigned int panelID, panelHudID;

	if (!panelHudID)
		if (!strcmp("HudZoom", g_pPanel->GetName(vguiPanel)))
		{
			panelHudID = vguiPanel;
		}

	C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer()); // i put this everywhere that isnt cm because im gay and i think that the global can cause crashes outside where the localent is init

	if (panelHudID == vguiPanel && pLocalEntity && pLocalEntity->IsAlive() && g_Menu.Config.NoScope)
	{
		if (pLocalEntity->IsScoped())
			return;
	}

	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	if (!panelID)
		if (!strcmp("MatSystemTopPanel", g_pPanel->GetName(vguiPanel)))
		{
			panelID = vguiPanel;
			g_Hooks.bInitializedDrawManager = true;
		}
			
	if (panelID == vguiPanel) 
	{
		g_pEngine->GetScreenSize(g_Menu.iScreenWidth, g_Menu.iScreenHeight);

		if (startUp)
		{
			static bool Bruh = true;
			if (Bruh)
			{
				g_pSurface->UnLockCursor();
				Bruh = false;
			}

			g_pRender->FilledRect(0, 0, g_Menu.iScreenWidth, g_Menu.iScreenHeight, Color(0, 0, 0, 250));
			g_pRender->FilledRect((g_Menu.iScreenWidth / 2) - 50, (g_Menu.iScreenHeight / 2) - 50, 100, 20, Color(119, 119, 119, 250));
			g_pRender->FilledRect((g_Menu.iScreenWidth / 2) - 50, (g_Menu.iScreenHeight / 2) + 30, 100, 20, Color(119, 119, 119, 250));
			g_pRender->DrawT((g_Menu.iScreenWidth / 2), (g_Menu.iScreenHeight / 2) - 70, Color(255, 255, 255, 255), g::CourierNew, true, "cheat type");
			g_pRender->DrawT((g_Menu.iScreenWidth / 2), (g_Menu.iScreenHeight / 2) - 46, Color(255, 255, 255, 255), g::CourierNew, true, "LEGIT ONLY");
			g_pRender->DrawT((g_Menu.iScreenWidth / 2), (g_Menu.iScreenHeight / 2) + 34, Color(255, 255, 255, 255), g::CourierNew, true, "RAGE ONLY");

			if (g_pRender->MouseInRegion((g_Menu.iScreenWidth / 2) - 50, (g_Menu.iScreenHeight / 2) - 50, 100, 20) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			{
				g::CheatType = 2;
				startUp = false;
			}
			else if (g_pRender->MouseInRegion((g_Menu.iScreenWidth / 2) - 50, (g_Menu.iScreenHeight / 2) + 30, 100, 20) && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			{
				g::CheatType = 1;
				startUp = false;
			}
		}

		ESP::get().Render();
		Misc::get().Crosshair();
		g_Menu.Render();
		g_Menu.PopUpRender();

		if (false && pLocalEntity && pLocalEntity->IsAlive()) // server hitboxes *forgot to credit chad for sending me the sig for this thank you
		{
			static uintptr_t pCall = (uintptr_t)Utils::FindSignature("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");

			static float fDuration = -1.f;

			PVOID pEntity = nullptr;
			pEntity = UTIL_PlayerByIndex(pLocalEntity->EntIndex());

			if (pEntity)
			{
				__asm
				{
					pushad
					movss xmm1, fDuration
					push 0 //bool monoColor
					mov ecx, pEntity
					call pCall
					popad
				}
			}
		}
	}
}

void __fastcall Hooks::OverrideView(void* ECX, void* EDX, CViewSetup* pSetup)
{
	static auto oOverrideView = g_Hooks.pClientModeHook->GetOriginal<OverrideView_t>(vtable_indexes::view);
	oOverrideView(ECX, EDX, pSetup);

	if (g::CheatType != -1 && g::CheatType == 1)
	{
		if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			C_BaseEntity* pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

			if (!pLocalEntity)
				return;

			if (!pLocalEntity->IsAlive())
				return;

			if (g_Menu.Config.NoRecoil && !GetKeyState(g_Menu.Config.ThirdPersonKey))
			{
				Vector viewPunch = pLocalEntity->GetViewPunchAngle();
				Vector aimPunch = pLocalEntity->GetAimPunchAngle();

				float RecoilScale = g_pCvar->FindVar("weapon_recoil_scale")->GetFloat();

				pSetup->angles -= (viewPunch + (aimPunch * RecoilScale * 0.4499999f)); // oof 1 line now ez
			}

			if (g_Menu.Config.Fov != 0 && !pLocalEntity->IsScoped())
				pSetup->fov = g_Menu.Config.Fov;

			if (g_Menu.Config.NoZoom && pLocalEntity->IsScoped())
				pSetup->fov = (g_Menu.Config.Fov == 0) ? 90 : g_Menu.Config.Fov;

			if (!GetKeyState(g_Menu.Config.ThirdPersonKey))
			{
				if (!GetAsyncKeyState(g_Menu.Config.FakeDuckKey))
					pSetup->origin = pLocalEntity->GetEyePosition();
				else
					pSetup->origin.z = pLocalEntity->GetAbsOrigin().z + 64.f;
			}
			else // best tp ever???? xd
			{
				Vector ViewAngles, ViewForward;
				g_pEngine->GetViewAngles(ViewAngles);

				pSetup->angles = ViewAngles;

				ViewAngles.x = -ViewAngles.x;
				ViewAngles.y = g_Math.NormalizeYaw(ViewAngles.y + 180);

				g_Math.AngleVectors(ViewAngles, &ViewForward);
				g_Math.NormalizeAngles(ViewForward);

				Vector Origin = pLocalEntity->GetEyePosition();

				if (GetAsyncKeyState(g_Menu.Config.FakeDuckKey))
					Origin.z = pLocalEntity->GetAbsOrigin().z + 64.f;

				C_Trace TraceInit;
				C_TraceFilter filter1(pLocalEntity);
				g_pTrace->TraceRay(C_Ray(Origin, (Origin + (ViewForward * g_Menu.Config.ThirdPersonDistance))), mask_solid, &filter1, &TraceInit);

				ViewForward = Origin + (ViewForward * (TraceInit.flFraction * g_Menu.Config.ThirdPersonDistance));

				pSetup->origin = ViewForward;
			}
		}
	}
}

void __fastcall Hooks::LockCursor(ISurface* thisptr, void* EDX)
{
    static auto oLockCursor = g_Hooks.pSurfaceHook->GetOriginal<LockCursor_t>(vtable_indexes::lockCursor);

    if (!g_Menu.menuOpened)
        return oLockCursor(thisptr, EDX);

    g_pSurface->UnLockCursor();
}

LRESULT Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // for now as a lambda, to be transfered somewhere
    // Thanks uc/WasserEsser for pointing out my mistake!
    // Working when you HOLD th button, not when you press it.
    const auto getButtonHeld = [uMsg, wParam](bool& bButton, int vKey)
    {
		if (wParam != vKey) return;

        if (uMsg == WM_KEYDOWN)
            bButton = true;
        else if (uMsg == WM_KEYUP)
            bButton = false;
    };

	const auto getButtonToggle = [uMsg, wParam](bool& bButton, int vKey)
	{
		if (wParam != vKey) return;

		if (uMsg == WM_KEYUP)
			bButton = !bButton;
	};

    if (g_Hooks.bInitializedDrawManager)
        if (g_Menu.menuOpened)
            return true;

    return CallWindowProcA(g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam);
}
