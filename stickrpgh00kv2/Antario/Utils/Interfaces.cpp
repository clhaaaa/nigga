#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\IGameEvent.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\IEngineTrace.h"
#include "..\SDK\ISurfaceData.h"
#include "..\SDK\ICvar.h"
#include "..\SDK\IVModelInfo.h"
#include "..\SDK\CModelRender.h"
#include "..\SDK\IMaterial.h"
#include "..\SDK\IVRenderView.h"
#include "..\SDK\IMDLCache.h"
#include "..\SDK\IClientState.h"

#include <fstream>

#define enc_str(s) std::string(s)
#define enc_char(s) enc_str(s).c_str()
#define enc_wstr(s) std::wstring(enc_str(s).begin(), enc_str(s).end())
#define enc_wchar(s) enc_wstr(s).c_str()

// Initializing global interfaces
IBaseClientDLL*     g_pClientDll    = nullptr;
IClientMode*        g_pClientMode   = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngine       = nullptr;
CPrediction*        g_pPrediction   = nullptr;
IGameMovement*      g_pMovement     = nullptr;
IMoveHelper*        g_pMoveHelper   = nullptr;
CGlobalVarsBase*    g_pGlobalVars   = nullptr;
IGameEventManager*  g_pEventManager = nullptr;
ISurface*           g_pSurface      = nullptr;
IEngineTrace*       g_pTrace        = nullptr;
IPhysicsSurfaceProps* g_pSurfaceData= nullptr;
ICVar*              g_pCvar			= nullptr;
IPanel*				g_pPanel		= nullptr;
IVModelInfo*		g_pModelInfo	= nullptr;
CModelRender*       g_pModelRender  = nullptr;
IMaterialSystem*    g_pMaterialSys  = nullptr;
IVRenderView*       g_pRenderView   = nullptr;
IMDLCache*			g_pIMDLCache	= nullptr;
IClientState*		g_pClientState  = nullptr;

IRender* g_pRender;

namespace interfaces
{
	template< class T >
	T* CaptureInterface(std::string szModuleName, std::string szInterfaceName, bool bSkip = false) // not mine *sorry for naming it findclass
	{
		if (szModuleName.empty() || szInterfaceName.empty())
			return nullptr;
		typedef PVOID(*CreateInterfaceFn)(const char* pszName, int* piReturnCode);
		CreateInterfaceFn hInterface = nullptr;
		while (!hInterface)
		{
			hInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA(szModuleName.c_str()), "CreateInterface"));
			Sleep(5);
		}

		char pszBuffer[256];
		for (int i = 0; i < 100; i++)
		{
			sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i);
			PVOID pInterface = hInterface(pszBuffer, nullptr);

			if (pInterface && pInterface != NULL)
			{
				if (bSkip)
					sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i + 1);

				Sleep(5);
				break;
			}
		}

		return reinterpret_cast<T*>(hInterface(pszBuffer, nullptr));
	}

	template<typename T>
	T* CaptureInterfaceSpecific(const char* szModuleName, const char* szInterfaceVersion) // not mine
	{
		HMODULE moduleHandle = GetModuleHandleA(szModuleName);
		if (moduleHandle)   /* In case of not finding module handle, throw an error. */
		{
			CreateInterfaceFn pfnFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(moduleHandle, "CreateInterface"));
			return reinterpret_cast<T*>(pfnFactory(szInterfaceVersion, nullptr));
		}
		Utils::Log("Error getting interface %", szInterfaceVersion);
		return nullptr;
	}

    void Init()
    {
        g_pClientDll    = CaptureInterface<IBaseClientDLL>("client_panorama.dll", "VClient");
        g_pClientMode   = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[10] + 0x5u);  
        g_pGlobalVars   = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[0]  + 0x1Bu); 
        g_pEntityList   = CaptureInterface<IClientEntityList>("client_panorama.dll", "VClientEntityList");
        g_pEngine       = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient");
        g_pPrediction   = CaptureInterface<CPrediction>("client_panorama.dll", "VClientPrediction");
        g_pMovement     = CaptureInterface<IGameMovement>("client_panorama.dll", "GameMovement");
        g_pMoveHelper   = **reinterpret_cast<IMoveHelper***>((Utils::FindSignature("client_panorama.dll", "8B 0D ? ? ? ? 8B 46 08 68") + 0x2));  
        g_pEventManager = CaptureInterfaceSpecific<IGameEventManager>("engine.dll", "GAMEEVENTSMANAGER002");
        g_pSurface      = CaptureInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface");
		g_pTrace        = CaptureInterface<IEngineTrace>("engine.dll", "EngineTraceClient");
		g_pSurfaceData  = CaptureInterface<IPhysicsSurfaceProps>("vphysics.dll", "VPhysicsSurfaceProps");
		g_pCvar         = CaptureInterface<ICVar>("vstdlib.dll", "VEngineCvar");
		g_pPanel		= CaptureInterface<IPanel>("vgui2.dll", "VGUI_Panel");
		g_pModelInfo    = CaptureInterface<IVModelInfo>("engine.dll", "VModelInfoClient");
		g_pModelRender  = CaptureInterface<CModelRender>("engine.dll", "VEngineModel");
		g_pMaterialSys  = CaptureInterface<IMaterialSystem>("materialsystem.dll", "VMaterialSystem");
		g_pRenderView   = CaptureInterface<IVRenderView>("engine.dll", "VEngineRenderView");
		g_pIMDLCache	= CaptureInterface<IMDLCache>("datacache.dll", "MDLCache");
		g_pClientState  = **reinterpret_cast<IClientState***>((*reinterpret_cast<uintptr_t**>(g_pEngine))[12] + 0x10u);

		std::ofstream("csgo\\materials\\FlatChams.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$no_fullbright" "0"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    }
}