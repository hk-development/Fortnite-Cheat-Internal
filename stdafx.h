#pragma once

#include <stdio.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include "MinHook/include/MinHook.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "xorstr.h"

#include "render.h"
#include "settings.h"
#include "offsets.h"
#include "utils.h"
#include "structs.h"
#include "updater.h"
#include "core.h"

#define valid_pointer(pointer) (pointer != NULL && (uint64_t)pointer >= 0x10000 && (uint64_t)pointer >= 0x10000000000 && (uint64_t)pointer < 0x00007FFFFFFEFFFF && (uint64_t)pointer != 0xFFFFFFFF)
