#include "stdafx.h"
#include "hooks.h"

VOID CreateDebugConsole()
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	cout << "Hello" << endl;
}

VOID Main() {
	//CreateDebugConsole();
	
	MH_Initialize();

	MH_CreateHook(&GetWindowTextA, GetWindowTextAHook, reinterpret_cast<PVOID*>(&GetWindowTextAOriginal));
	MH_EnableHook(&GetWindowTextA);

	MH_CreateHook(&GetWindowTextW, GetWindowTextWHook, reinterpret_cast<PVOID*>(&GetWindowTextWOriginal));
	MH_EnableHook(&GetWindowTextW);

	MH_CreateHook(&CreateFileA, CreateFileAHook, reinterpret_cast<PVOID*>(&CreateFileAOriginal));
	MH_EnableHook(&CreateFileA);

	MH_CreateHook(&CreateFileW, CreateFileWHook, reinterpret_cast<PVOID*>(&CreateFileWOriginal));
	MH_EnableHook(&CreateFileW);

	MH_CreateHook(&DeviceIoControl, DeviceIoControlHook, reinterpret_cast<PVOID*>(&DeviceIoControlOriginal));
	MH_EnableHook(&DeviceIoControl);

	MH_CreateHook(&SetupDiGetDevicePropertyW, SetupDiGetDevicePropertyWHook, reinterpret_cast<PVOID*>(&SetupDiGetDevicePropertyWOriginal));
	MH_EnableHook(&SetupDiGetDevicePropertyW);

	MH_CreateHook(&SetupDiGetDeviceRegistryPropertyW, SetupDiGetDeviceRegistryPropertyWHook, reinterpret_cast<PVOID*>(&SetupDiGetDeviceRegistryPropertyWOriginal));
	MH_EnableHook(&SetupDiGetDeviceRegistryPropertyW);

	MH_CreateHook(&SetupDiGetDeviceInstanceIdW, SetupDiGetDeviceInstanceIdWHook, reinterpret_cast<PVOID*>(&SetupDiGetDeviceInstanceIdWOrignal));
	MH_EnableHook(&SetupDiGetDeviceInstanceIdW);

	//Registry
	/*MH_CreateHook(&RegOpenKeyExA, RegOpenKeyExAHook, reinterpret_cast<PVOID*>(&RegOpenKeyExAOriginal));
	MH_EnableHook(&RegOpenKeyExA);

	MH_CreateHook(&RegQueryValueExA, RegQueryValueExAHook, reinterpret_cast<PVOID*>(&RegQueryValueExAOriginal));
	MH_EnableHook(&RegQueryValueExA);

	MH_CreateHook(&RegCloseKey, RegCloseKeyHook, reinterpret_cast<PVOID*>(&RegCloseKeyOriginal));
	MH_EnableHook(&RegCloseKey);

	MH_CreateHook(&RegCreateKeyExW, RegCreateKeyExWHook, reinterpret_cast<PVOID*>(&RegCreateKeyExWOriginal));
	MH_EnableHook(&RegCreateKeyExW);

	MH_CreateHook(&RegOpenKeyExW, RegOpenKeyExWHook, reinterpret_cast<PVOID*>(&RegOpenKeyExWOriginal));
	MH_EnableHook(&RegOpenKeyExW);

	MH_CreateHook(&RegQueryValueExW, RegQueryValueExWHook, reinterpret_cast<PVOID*>(&RegQueryValueExWOriginal));
	MH_EnableHook(&RegQueryValueExW);

	MH_CreateHook(&RegSetValueExW, RegSetValueExWHook, reinterpret_cast<PVOID*>(&RegSetValueExWOriginal));
	MH_EnableHook(&RegSetValueExW);

	MH_CreateHook(&RegGetValueW, RegGetValueWHook, reinterpret_cast<PVOID*>(&RegGetValueWOriginal));
	MH_EnableHook(&RegGetValueW);

	MH_CreateHook(&RegDeleteKeyW, RegDeleteKeyWHook, reinterpret_cast<PVOID*>(&RegDeleteKeyWOriginal));
	MH_EnableHook(&RegDeleteKeyW);

	MH_CreateHook(&RegDeleteValueW, RegDeleteValueWHook, reinterpret_cast<PVOID*>(&RegDeleteValueWOriginal));
	MH_EnableHook(&RegDeleteValueW);

	MH_CreateHook(&RegEnumKeyExW, RegEnumKeyExWHook, reinterpret_cast<PVOID*>(&RegEnumKeyExWOriginal));
	MH_EnableHook(&RegEnumKeyExW);

	MH_CreateHook(&RegEnumValueW, RegEnumValueWHook, reinterpret_cast<PVOID*>(&RegEnumValueWOriginal));
	MH_EnableHook(&RegEnumValueW);

	MH_CreateHook(&RegQueryInfoKeyA, RegQueryInfoKeyAHook, reinterpret_cast<PVOID*>(&RegQueryInfoKeyAOriginal));
	MH_EnableHook(&RegQueryInfoKeyA);

	MH_CreateHook(&RegEnumKeyExA, RegEnumKeyExAHook, reinterpret_cast<PVOID*>(&RegEnumKeyExAOriginal));
	MH_EnableHook(&RegEnumKeyExA);*/

	if (!utils::Initialize())
		return;

	if (!Offsets::Initialize())
		return;

	if (!Settings::Initialize())
		return;

	if (!core::init())
		return;

	if (!Render::Initialize())
		return;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		Main();
	}

	return TRUE;
}