#include "includes.h"
#include "hooks/hooks.h"
#include "utils/utils.h"

int main(const HMODULE base)
{
	utils::open_console();
	printf("[ main ] console initialization was successful\n");

	printf("[ info ] developer github: https://github.com/W1lliam1337\n");
	printf("[ info ] 'del' - close the console\n");

	printf("[ main ] modules initialization...\n");
	utils::init_modules(base);

	printf("[ main ] hooks initialization...\n");
	hooks::instance();

	while (!GetAsyncKeyState(VK_DELETE))
	{
		_getwch();
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);
			CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), hModule, 0, nullptr);
			return TRUE;
		}
	/*case DLL_PROCESS_DETACH:
		{
			utils::shutdown();
	        return TRUE;
		}*/
	default: break;
	}

	return TRUE;
}
