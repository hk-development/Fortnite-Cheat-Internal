#pragma once

using namespace std;

namespace core
{
	extern PVOID(*ProcessEvent)(PVOID, PVOID, PVOID, PVOID);
	extern PVOID player_controller;
	
	BOOLEAN init();
}
