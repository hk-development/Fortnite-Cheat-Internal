#include "utils.h"
#include "offsets.h"
#include "MinHook/include/MinHook.h"
#include "stdafx.h"

namespace core
{
	PVOID(*ProcessEvent)(PVOID, PVOID, PVOID, PVOID) = nullptr;
	PVOID player_controller = nullptr;
	INT(*GetViewPoint)(PVOID, FMinimalViewInfo*, BYTE) = nullptr;

	PVOID ProcessEventHook(UObject* object, UObject* func, PVOID params, PVOID result)
	{
		if (object && func)
		{
			//ProcessEvent(core::player_controller, );

			//auto obj_name = Utils::get_name(reinterpret_cast<uint64_t>(object));
			//wcout << "Process event! " << "(Obj -> " << obj_name << ")" << endl;
		}

		return ProcessEvent(object, func, params, result);
	}

	INT GetViewPointHook(PVOID player, FMinimalViewInfo* viewInfo, BYTE stereoPass) {
		auto ret = GetViewPoint(player, viewInfo, stereoPass);

		if (settings.FOVChanger)
		{
			auto fov = settings.CameraFOV;
			viewInfo->FOV = fov;
		}

		return ret;
	}


	BOOLEAN init()
	{
		/*auto addr = Utils::FindPattern(
			XorStr("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x8B\x41\x0C\x45\x33\xF6\x3B\x05\x00\x00\x00\x00\x4D\x8B\xF8\x48\x8B\xF2\x4C\x8B\xE1\x41\xB8\x00\x00\x00\x00\x7D\x2A").c_str(), 
			XorStr("xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxxxxxx????xxxxxxxxxxx????xx").c_str());
		if (!addr) {
			MessageBox(0, XorStr(L"Failed to find ProcessEvent").c_str(), L"Failure", 0);
			return FALSE;
		}

		MH_CreateHook(addr, ProcessEventHook, (PVOID*)&ProcessEvent);
		MH_EnableHook(addr);*/

		auto addr = utils::FindPattern(
			_xor("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\xD9\x41\x8B\xF0\x48\x8B\x49\x30\x48\x8B\xFA\xE8\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\xC8").c_str(), 
			_xor("xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????x????xxx").c_str());
		if (!addr) {
			MessageBox(0, _xor(L"Failed to find CalculateShot").c_str(), _xor(L"Failure").c_str(), 0);
			return FALSE;
		}

		MH_CreateHook(addr, GetViewPointHook, (PVOID*)&GetViewPoint);
		MH_EnableHook(addr);

		return TRUE;
	}
}