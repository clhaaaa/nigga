#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Utils\GlobalVars.h"

// stole my DLLMain from duCCAcACACAraraaii https://github.com/DucaRii/csgo_modest/blob/master/csgo/main.cpp

DWORD WINAPI entry(LPVOID lpThreadParameter)
{
//	try
//	{
		Hooks::Init();

		while (!GetAsyncKeyState(VK_END))
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
//	}
//	catch (const std::runtime_error& err)
//	{
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	Hooks::Restore();

	FreeLibraryAndExitThread(static_cast<HMODULE>(lpThreadParameter), EXIT_SUCCESS);
}

BOOL APIENTRY DllMain(_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
		DisableThreadLibraryCalls(hinstDLL);

		if (auto handle = CreateThread(nullptr, NULL, entry, hinstDLL, NULL, nullptr))
		{
			CloseHandle(handle);
		}
    }

    return TRUE;
}

