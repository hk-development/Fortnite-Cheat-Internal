#pragma once

#include <string>

struct SETTINGS
{
	bool MemoryAimbot;
	bool Debug;
	bool FOV;
	bool PlayersAround;
	bool MarkBots;
	float AimbotFOV;

	float CameraFOV;
	bool FOVChanger;

	struct _Aimbot
	{
		std::string BoneName;
	} Aimbot;
	struct {
		bool BoneESP;
		bool BoxEsp;
		bool PlayerLines; ##removed features for ban risk :9
	} ESP;
	float NotVisibleColor[4];
	float BotColor[4];
};

extern SETTINGS settings;

namespace Settings {
	bool Initialize();
}
