#include "stdafx.h"

#include <Psapi.h>
#include <algorithm>
#include <sstream>
#include <string_view>
#include <array>
#include <vector>

ULONGLONG GetBoneMatrixAddress = 0;

namespace utils {
	const wchar_t* get_name(uint64_t object)
	{
		uint64_t base = (uint64_t)GetModuleHandleW(nullptr);
		auto fGetObjName2 = reinterpret_cast<FString * (__fastcall*)(FString*, uintptr_t)>(base + Offsets::GetObjectName);

		if (!valid_pointer(fGetObjName2))
			return L"";

		FString result;
		fGetObjName2(&result, object);

		if (!result.IsValid())
			return L"";

		return result.c_str();
	}

	bool get_aim_angles(FRotator cam_rotation, FVector cam_location, uint64_t aactor, int bone, FRotator* out)
	{
		FVector vec;
		float x = 0, y = 0, z = 0;
		float distance = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		FVector pos;

		if (!valid_pointer((void*)aactor))
		{
			return false;
		}

		if (!get_bone_matrix(aactor, bone, &vec))
		{
			return false;
		}

		x = abs(cam_location.x - vec.x);
		y = abs(cam_location.y - vec.y);
		z = abs(cam_location.z - vec.z);

		distance = sqrtf(x * x +  y * y + z * z);

		pos.x = vec.x - cam_location.x;
		pos.y = vec.y - cam_location.y;
		pos.z = vec.z - cam_location.z;

		pitch = -((acos(pos.z / distance) * 180.0f / (float)M_PI) - 90.0f);
		yaw = (atan2(pos.y, pos.x) * 180.0f) / (float)M_PI;

		out->pitch = cam_rotation.pitch + (pitch - cam_rotation.pitch);
		out->yaw = cam_rotation.yaw + (yaw - cam_rotation.yaw);
		out->roll = 0.0f;

		return true;
	}

	bool LoadStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(10, 10);
		style->WindowRounding = 0.0f;
		style->FramePadding = ImVec2(4, 4);
		style->FrameRounding = 0.0f;
		style->ItemSpacing = ImVec2(8, 8);
		style->ItemInnerSpacing = ImVec2(8, 8);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 0.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 0.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.66f, 0.58f, 0.76f, 1.0f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.66f, 0.58f, 0.76f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.58f, 0.76f, 1.0f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.66f, 0.58f, 0.76f, 1.0f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CloseButton] = ImVec4(0.66f, 0.58f, 0.76f, 1.0f);
		style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(.30f, .30f, .30f, 0.39f);
		style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		return true;
	}

	D3DMATRIX GetMatrix(FRotator rot, FVector origin = FVector(0, 0, 0))
	{
		float radPitch = (rot.pitch * float(M_PI) / 180.f);
		float radYaw = (rot.yaw * float(M_PI) / 180.f);
		float radRoll = (rot.roll * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	FVector2D w2s(FVector WorldLocation, FMinimalViewInfo info)
	{
		FVector2D Screenlocation = FVector2D(0, 0);

		FRotator Rotation = info.Rotation; // FRotator

		D3DMATRIX tempMatrix = GetMatrix(Rotation); // Matrix

		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		FVector vDelta = WorldLocation - info.Location;
		FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		float FovAngle = info.FOV;
		float ScreenCenterX = 960.0f;
		float ScreenCenterY = 540.0f;

		Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
		Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

		return Screenlocation;
	}

	bool get_bone_matrix(uint64_t actor, unsigned int index, FVector* Out) {
		uint64_t ActorMesh = 0;
		FMatrix matrix = { 0 };
		FMatrix* tmp = nullptr;

		ActorMesh = *(ULONGLONG*)(actor + Offsets::SkeletalMeshComponent);
		if (!valid_pointer((void*)ActorMesh)) {
			return false;
		}

		FMatrix* (__fastcall * funcmatrix)(void*, void*, int) = (FMatrix * (__fastcall *)(void*, void*, int))GetBoneMatrixAddress;
		//tmp = func((void*)ActorMesh, (void*)&matrix, index);
		tmp = utils::spoof_call(Offsets::trampoline, funcmatrix, (void*)ActorMesh, (void*)&matrix, (int)index);

		Out->x = matrix.WPlane.x;
		Out->y = matrix.WPlane.y;
		Out->z = matrix.WPlane.z;

		return true;
	}

	BOOLEAN Initialize()
	{
		ULONGLONG Base = (ULONGLONG)GetModuleHandleA(nullptr);
		GetBoneMatrixAddress = (Base + Offsets::GetBoneMatrix);

		/*bool debug = true;
		if (debug)
		{
			cout << "BoneMatrix: 0x" << hex << GetBoneMatrixAddress << endl;
		}*/

		return TRUE;
	}

	BOOLEAN MaskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask) {
		for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
			if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
				return FALSE;
			}
		}

		return TRUE;
	}

	PBYTE FindPattern(PVOID base, DWORD size, LPCSTR pattern, LPCSTR mask) {
		size -= static_cast<DWORD>(strlen(mask));

		for (auto i = 0UL; i < size; ++i) {
			auto addr = reinterpret_cast<PBYTE>(base) + i;
			if (MaskCompare(addr, pattern, mask)) {
				return addr;
			}
		}

		return NULL;
	}

	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask) {
		MODULEINFO info = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, pattern, mask);
	}

#define FIND_NT_HEADER(x) reinterpret_cast<PIMAGE_NT_HEADERS>( uint64_t(x) + reinterpret_cast<PIMAGE_DOS_HEADER>(x)->e_lfanew )

	template <size_t N>
	uint8_t* find_signature(const std::string_view module, const char(&signature)[N])
	{
		std::array<uint8_t, N> signature_bytes{ };

		{
			std::vector<std::string> signature_chunks{ };
			std::string current_chunk{ };

			std::istringstream string_stream{ signature };

			while (std::getline(string_stream, current_chunk, ' '))
				signature_chunks.push_back(current_chunk);

			std::transform(signature_chunks.cbegin(), signature_chunks.cend(), signature_bytes.begin(), [](const std::string& val) -> uint8_t
				{
					return val.find('?') != std::string::npos ? 0ui8 : static_cast<uint8_t>(std::stoi(val, nullptr, 16));
				});
		}

		uint8_t* found_bytes = nullptr;

		{
			const auto image_start = reinterpret_cast<uint8_t*>(GetModuleHandleA(module.data()));
			const auto image_end = image_start + FIND_NT_HEADER(image_start)->OptionalHeader.SizeOfImage;

			const auto result = std::search(image_start, image_end, signature_bytes.cbegin(), signature_bytes.cend(), [](uint8_t left, uint8_t right) -> bool
				{
					return right == 0ui8 || left == right;
				});

			found_bytes = (result != image_end) ? result : nullptr;
		}

		return found_bytes;
	}
}
