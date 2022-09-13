#include "stdafx.h"
#include "imgui/imgui_impl_dx11.h"

IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int player_list_size = 0;
static int lobby_list_size = 0;

#define MAX_PLAYERS 500

struct players
{
	uint64_t aactor;
	bool bot;
};

static struct players player_list[MAX_PLAYERS];

static void load_player_into_list(uint64_t aactor, bool bot)
{
	if (player_list_size == MAX_PLAYERS) // Dont slot anymore players.
		return;

	player_list[player_list_size].aactor = aactor;
	player_list[player_list_size].bot = bot;

	player_list_size++;

	return;
}

static void clear_player_list(void)
{
	int i = 0;

	for (i = 0; i < player_list_size; i++)
	{
		player_list[i].aactor = 0;
		player_list[i].bot = false;
	}

	player_list_size = 0;

	return;
}

static float width = 0;
static float height = 0;
static HWND hWnd = 0;
float Pitch = 0;
FMinimalViewInfo myinfo;

uintptr_t GetObjectNameAddr, FnFreeAddr, GetNameByIndexAddr, player_controller = 0;

void(__fastcall * csr_func)(uint64_t, FRotator, bool) = nullptr;
int tabb = 0;
int AimbotBoneIndex = 36;

void init_csr(ULONGLONG PlayerController) {
	auto PlayerControllerVTable = *(ULONGLONG*)(PlayerController);
	if (!valid_pointer((void*)PlayerControllerVTable)) return;

	csr_func =
		(*(void(__fastcall**)(uint64_t, FRotator, bool))(PlayerControllerVTable + Offsets::ClientSetRotation));
}

VOID AddLine(ImGuiWindow& window, FVector a, FVector b, ImU32 color, float& minX, float& maxX, float& minY, float& maxY) {
	FVector2D ac = utils::w2s(a, myinfo);
	FVector2D bc = utils::w2s(b, myinfo);
	if (true) {
		if (settings.ESP.BoneESP) {
			window.DrawList->AddLine(ImVec2(ac.x, ac.y), ImVec2(bc.x, bc.y), color, 1.0f);
		}

		minX = min(ac.x, minX);
		minX = min(bc.x, minX);

		maxX = max(ac.x, maxX);
		maxX = max(bc.x, maxX);

		minY = min(ac.y, minY);
		minY = min(bc.y, minY);

		maxY = max(ac.y, maxY);
		maxY = max(bc.y, maxY);
	}
}

void InitAddresses() {
	uint64_t Base = (uint64_t)GetModuleHandleW(nullptr);
	Offsets::trampoline = (PVOID)utils::FindPattern(_xor("\x41\xFF\x26").c_str(), _xor("xxx").c_str());

	GetObjectNameAddr = ((ULONGLONG)Base + Offsets::GetObjectName);
	GetNameByIndexAddr = ((ULONGLONG)Base + Offsets::GetNameByIndex);
	FnFreeAddr = ((ULONGLONG)Base + Offsets::FnFree);
}

void decrypt_cam(ULONGLONG PlayerCameraManager) {
	auto PlayerCameraManagerVTable = *(ULONGLONG*)(PlayerCameraManager);
	if (!valid_pointer((void*)PlayerCameraManagerVTable)) return;

	uint64_t(__fastcall * func)(uint64_t, FMinimalViewInfo*) =
		(*(uint64_t(__fastcall*)(uint64_t, FMinimalViewInfo*))(*(uint64_t*)(PlayerCameraManagerVTable + Offsets::CameraDecrypt)));
	utils::spoof_call(Offsets::trampoline, func, (uint64_t)PlayerCameraManager, &myinfo);
	Pitch = myinfo.Rotation.pitch;
}

float RenderText(ImGuiWindow* window, const std::string& text, const ImVec2& position, float size, uint32_t color, bool center)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());

			window->DrawList->AddText(ImGui::GetFont(), size, { position.x - textSize.x / 2.0f, position.y + textSize.y * i }, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), line.c_str());
		}
		else
		{
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());
			window->DrawList->AddText(ImGui::GetFont(), size, { (position.x) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, a / 255.0f }), line.c_str());

			window->DrawList->AddText(ImGui::GetFont(), size, { position.x, position.y + textSize.y * i }, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), line.c_str());
		}

		y = position.y + textSize.y * (i + 1);
		i++;
	}

	return y;
}

void RenderRect(ImGuiWindow* window, const ImVec2& from, const ImVec2& to, uint32_t color, float rounding, uint32_t roundingCornersFlags, float thickness)
{
	float a = (color >> 24) & 0xFF;
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = (color) & 0xFF;

	window->DrawList->AddRect(ImVec2(from.x + 1.0f, from.y + 1.0f), ImVec2(to.x + 1.0f, to.y + 1.0f), ImGui::GetColorU32({ 0, 0, 0, 0.8f }), rounding, roundingCornersFlags, thickness);
	window->DrawList->AddRect(ImVec2(from.x - 1.0f, from.y - 1.0f), ImVec2(to.x - 1.0f, to.y - 1.0f), ImGui::GetColorU32({ 0, 0, 0, 0.8f }), rounding, roundingCornersFlags, thickness);
	window->DrawList->AddRect(ImVec2(from.x + 1.0f, from.y - 1.0f), ImVec2(to.x + 1.0f, to.y - 1.0f), ImGui::GetColorU32({ 0, 0, 0, 0.8f }), rounding, roundingCornersFlags, thickness);
	window->DrawList->AddRect(ImVec2(from.x - 1.0f, from.y + 1.0f), ImVec2(to.x - 1.0f, to.y + 1.0f), ImGui::GetColorU32({ 0, 0, 0, 0.8f }), rounding, roundingCornersFlags, thickness);

	window->DrawList->AddRect(from, to, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), rounding, roundingCornersFlags, thickness);
}

void draw_player_warning(ImGuiWindow& window, bool lobby)
{
	char player_warning_buffer[64] = { 0 };
	sprintf_s(player_warning_buffer, sizeof(player_warning_buffer), _xor("Warning %u players").c_str(), player_list_size);
	RenderText(&window, player_warning_buffer, ImVec2(960, 90), 22.0f, 0xFFFF0000, true);
}

void player_loop(ImGuiWindow& window, int ti)
{
	int pl = 0;

	float closest_dist = FLT_MAX;
	uint64_t closest_pawn = NULL;
	uint64_t target_pawn = NULL;
	BOOLEAN success = FALSE;

	for (pl = 0; pl < player_list_size; pl++)
	{
		struct players player = player_list[pl];

		uint64_t playerstate = *(uint64_t*)(player.aactor + Offsets::PlayerState);

		if (!valid_pointer(playerstate))
			continue;

		int eti = *(int*)(playerstate + Offsets::TeamIndex);

		if (eti == ti)
			continue;

		FVector head_pos;
		if (!utils::get_bone_matrix(player.aactor, 66, &head_pos))
			continue;

		FVector neck;
		if (!utils::get_bone_matrix(player.aactor, 65, &neck))
			continue;

		FVector chest;
		if (!utils::get_bone_matrix(player.aactor, 36, &chest))
			continue;

		FVector pelvis;
		if (!utils::get_bone_matrix(player.aactor, 2, &pelvis))
			continue;

		//Arms
		FVector leftShoulder;
		if (!utils::get_bone_matrix(player.aactor, 9, &leftShoulder))
			continue;

		FVector rightShoulder;
		if (!utils::get_bone_matrix(player.aactor, 62, &rightShoulder))
			continue;

		FVector leftElbow;
		if (!utils::get_bone_matrix(player.aactor, 10, &leftElbow))
			continue;

		FVector rightElbow;
		if (!utils::get_bone_matrix(player.aactor, 38, &rightElbow))
			continue;

		FVector leftHand;
		if (!utils::get_bone_matrix(player.aactor, 11, &leftHand))
			continue;

		FVector rightHand;
		if (!utils::get_bone_matrix(player.aactor, 39, &rightHand))
			continue;

		//Bottom
		FVector leftLeg;
		if (!utils::get_bone_matrix(player.aactor, 67, &leftLeg))
			continue;

		FVector rightLeg;
		if (!utils::get_bone_matrix(player.aactor, 74, &rightLeg))
			continue;

		FVector leftThigh;
		if (!utils::get_bone_matrix(player.aactor, 73, &leftThigh))
			continue;

		FVector rightThigh;
		if (!utils::get_bone_matrix(player.aactor, 80, &rightThigh))
			continue;

		FVector leftFoot;
		if (!utils::get_bone_matrix(player.aactor, 68, &leftFoot))
			continue;

		FVector rightFoot;
		if (!utils::get_bone_matrix(player.aactor, 75, &rightFoot))
			continue;

		FVector leftFeet;
		if (!utils::get_bone_matrix(player.aactor, 71, &leftFeet))
			continue;

		FVector rightFeet;
		if (!utils::get_bone_matrix(player.aactor, 78, &rightFeet))
			continue;

		FVector leftFeetFinger;
		if (!utils::get_bone_matrix(player.aactor, 72, &leftFeetFinger))
			continue;

		FVector rightFeetFinger;
		if (!utils::get_bone_matrix(player.aactor, 79, &rightFeetFinger))
			continue;

		float minX = FLT_MAX;
		float maxX = -FLT_MAX;
		float minY = FLT_MAX;
		float maxY = -FLT_MAX;

		FVector2D chest_pos_2d = utils::w2s(chest, myinfo);

		auto color = ImGui::GetColorU32({ settings.BotColor[0], settings.BotColor[1], settings.BotColor[2], settings.BotColor[3] });

		AddLine(window, head_pos, neck, color, minX, maxX, minY, maxY);
		AddLine(window, neck, pelvis, color, minX, maxX, minY, maxY);
		AddLine(window, chest, leftShoulder, color, minX, maxX, minY, maxY);
		AddLine(window, chest, rightShoulder, color, minX, maxX, minY, maxY);
		AddLine(window, leftShoulder, leftElbow, color, minX, maxX, minY, maxY);
		AddLine(window, rightShoulder, rightElbow, color, minX, maxX, minY, maxY);
		AddLine(window, leftElbow, leftHand, color, minX, maxX, minY, maxY);
		AddLine(window, rightElbow, rightHand, color, minX, maxX, minY, maxY);
		AddLine(window, pelvis, leftLeg, color, minX, maxX, minY, maxY);
		AddLine(window, pelvis, rightLeg, color, minX, maxX, minY, maxY);
		AddLine(window, leftLeg, leftThigh, color, minX, maxX, minY, maxY);
		AddLine(window, rightLeg, rightThigh, color, minX, maxX, minY, maxY);
		AddLine(window, leftThigh, leftFoot, color, minX, maxX, minY, maxY);
		AddLine(window, rightThigh, rightFoot, color, minX, maxX, minY, maxY);
		AddLine(window, leftFoot, leftFeet, color, minX, maxX, minY, maxY);
		AddLine(window, rightFoot, rightFeet, color, minX, maxX, minY, maxY);
		AddLine(window, leftFeet, leftFeetFinger, color, minX, maxX, minY, maxY);
		AddLine(window, rightFeet, rightFeetFinger, color, minX, maxX, minY, maxY);

		//RenderRect(&window, )
		player.bot ? RenderText(&window, _xor("BOT").c_str(), ImVec2(chest_pos_2d.x, chest_pos_2d.y), 12.0f, 0xFFFF0000, true) :
			RenderText(&window, _xor("PLAYER").c_str(), ImVec2(chest_pos_2d.x, chest_pos_2d.y), 12.0f, 0xFFFF0000, true);

		auto topLeft = ImVec2(minX - 3.0f, minY - 3.0f);
		auto bottomRight = ImVec2(maxX + 3.0f, maxY + 3.0f);

		if (minX < width && maxX > 0 && minY < height && maxY > 0 && settings.ESP.BoxEsp) {
			RenderRect(&window, topLeft, bottomRight, 0xFFFF0000, 1.5f, 0, 1.0f);
		}

		//Aimbot shit
		if (settings.MemoryAimbot)
		{
			auto dx = abs(chest_pos_2d.x - 960.0f);
			auto dy = abs(chest_pos_2d.y - 540.0f);
			auto dist = utils::spoof_call(Offsets::trampoline, sqrtf, dx * dx + dy * dy);

			//RenderText(&window, to_string((int)dist), ImVec2(chest_pos_2d.x, chest_pos_2d.y - 10.0f), 12.0f, 0xFFFF0000, true);

			if ( dist < closest_dist && dist <= settings.AimbotFOV ) {
				closest_dist = dist;
				closest_pawn = player.aactor;
			}

		}

		if (settings.ESP.PlayerLines)
		{
			window.DrawList->AddLine(ImVec2(960, 540), ImVec2(chest_pos_2d.x, chest_pos_2d.y), ImGui::GetColorU32({ 1, 0, 0, 0.5 }), 1.0f);
		}
	}

	success = TRUE;

	//draw line to close pawn
	if (settings.MemoryAimbot && closest_pawn != NULL)
	{
		FVector targget_pawn_loc;

		if (!utils::get_bone_matrix((uint64_t)closest_pawn, 66, &targget_pawn_loc))
			return;

		FVector2D target_pawn_2d = utils::w2s(targget_pawn_loc, myinfo);

		window.DrawList->AddLine(ImVec2(960, 1080 / 2), ImVec2(target_pawn_2d.x, target_pawn_2d.y), ImGui::GetColorU32({ 1, 0, 0, 1 }), 3.0f);
	}

	bool pressed = utils::spoof_call(Offsets::trampoline, GetAsyncKeyState, VK_RBUTTON) < 0;

	if (settings.MemoryAimbot 
		&& closest_pawn
		&& pressed
		&& success)
	{
		pressed = true;
		target_pawn = closest_pawn;
	}
	else
	{
		target_pawn = NULL;
	}

	//set angles
	if (target_pawn && pressed)
	{
		FRotator out;
		utils::get_aim_angles(myinfo.Rotation, myinfo.Location, (uint64_t)target_pawn, AimbotBoneIndex, &out);
		csr_func(player_controller, out, false);
	}

	//debug aimbot
	char aimbot_buf[120] = { 0 };
	sprintf_s(aimbot_buf, sizeof(aimbot_buf), _xor("(tp) 0x%llx, (cp) 0x%llx, (press) %d, (dist): %d").c_str(), target_pawn, closest_pawn, pressed, (int)closest_dist);
	RenderText(&window, aimbot_buf, ImVec2(960, 400), 18.0f, 0xFFFF0000, true);
}

void update_pointers(ImGuiWindow& window)
{
	uint64_t base = (uint64_t)GetModuleHandleW(nullptr);
	uint64_t uworld = *(uint64_t*)(base + Offsets::UWorld);

	if (!valid_pointer(uworld))
	{
		//cout << "world not valid" << endl;
		return;
	}

	uint64_t game_instance = *(uint64_t*)(uworld + Offsets::OwningGameInstance);

	if (!valid_pointer(game_instance))
	{
		//cout << "game_instance not valid" << endl;
		return;
	}

	uint64_t plevel = *(uint64_t*)(uworld + Offsets::PersistentLevel);

	if (!valid_pointer(plevel))
	{
		//cout << "plevel not valid" << endl;
		return;
	}

	uint64_t actors = *(uint64_t*)(plevel + Offsets::AActors);

	if (!valid_pointer(actors))
	{
		//cout << "actors not valid" << endl;
		return;
	}

	uint32_t actor_count = *(uint32_t*)(plevel + 0xa0);
	clear_player_list();

	uint64_t local_player_array = *(uint64_t*)(game_instance + Offsets::LocalPlayers);

	if (!valid_pointer(local_player_array))
	{
		//cout << "local_player_array not valid" << endl;
		return;
	}

	uint64_t local_player = *(uint64_t*)(local_player_array + 0x0);

	if (!valid_pointer(local_player))
	{
		//cout << "local_player not valid" << endl;
		return;
	}

	player_controller = *(uint64_t*)(local_player + Offsets::PlayerController);

	core::player_controller = (PVOID)player_controller;

	init_csr(player_controller);

	if (!valid_pointer(player_controller))
	{
		//cout << "player_controller not valid" << endl;
		return;
	}

	uint64_t player_cam_manager = *(uint64_t*)(player_controller + Offsets::PlayerCameraManager);

	if (!valid_pointer(player_cam_manager))
	{
		//cout << "player_cam_manager not valid" << endl;
		return;
	}

	decrypt_cam(player_cam_manager);

	uint64_t acknowledged_pawn = *(uint64_t*)(player_controller + Offsets::AcknowledgedPawn);

	if (!valid_pointer(acknowledged_pawn))
	{
		//cout << "acknowledged_pawn not valid" << endl;
		for (size_t i = 0; i < actor_count; i++)
		{
			auto object = *(uint64_t*)(actors + i * 0x8);

			if (!valid_pointer(object))
			{
				continue;
			}

			auto fGetObjName2 = reinterpret_cast<FString * (__fastcall*)(FString*, uintptr_t)>(base + Offsets::GetObjectName);

			if (!valid_pointer(fGetObjName2))
				continue;

			FString result;
			fGetObjName2(&result, object);

			if (!result.IsValid())
				continue;

			auto object_name = result.c_str();
			if (wcsstr(object_name, L"PlayerPawn_Athena_C"))
			{
				load_player_into_list(object, false);
			}
		}
		draw_player_warning(window, true);
		return;
	}

	uint64_t playerstate = *(uint64_t*)(acknowledged_pawn + Offsets::PlayerState);

	if (!valid_pointer(playerstate))
	{
		// cout ps not valid
		return;
	}

	int ti = *(int*)(playerstate + Offsets::TeamIndex);

	//Do aimbot
	for (size_t i = 0; i < actor_count; i++)
	{
		auto object = *(uint64_t*)(actors + i * 0x8);

		if (!valid_pointer(object) || object == acknowledged_pawn)
		{
			continue;
		}

		auto fGetObjName2 = reinterpret_cast<FString * (__fastcall*)(FString*, uintptr_t)>(base + Offsets::GetObjectName);

		if (!valid_pointer(fGetObjName2))
			continue;

		FString result;
		fGetObjName2(&result, object);

		if (!result.IsValid())
			continue;

		auto object_name = result.c_str();
		if (wcsstr(object_name, L"PlayerPawn_Athena_C"))
		{
			load_player_into_list(object, false);
		}
		else if (wcsstr(object_name, L"PlayerPawn_Athena_Phoebe_C"))
		{
			load_player_into_list(object, true);
		}
	}

	player_loop(window, ti);
	draw_player_warning(window, false);
}

namespace Render {
	BOOLEAN showMenu = FALSE;

	WNDPROC WndProcOriginal = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	HRESULT(*PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
	HRESULT(*ResizeOriginal)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;

	ImGuiWindow& BeginScene() {
		ImGui_ImplDX11_NewFrame();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::Begin(_xor("##scene").c_str(), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

		auto& io = ImGui::GetIO();
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

		return *ImGui::GetCurrentWindow();
	}

	void AddTab(size_t Index, const char* Text)
	{
		static const size_t TabWidth = 100;
		static const size_t TabHeight = 20;

		ImGui::PushID(Index);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

		if (Index == 1)
			ImGui::SameLine(Index * (TabWidth + 5));
		else if (Index > 1)
			ImGui::SameLine(Index * (TabWidth + 4 - Index));

		if (tabb == Index)
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(66, 58, 76));			// Color on tab open
		else
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(30, 30, 30));			// Color on tab closed

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(40, 40, 40));			// Color on mouse hover in tab
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(35, 35, 35));			// Color on click tab

		if (ImGui::Button(Text, ImVec2(TabWidth, TabHeight)))	// If tab clicked
			tabb = Index;

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}

	VOID EndScene(ImGuiWindow& window) {
		window.DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.17f, 0.18f, 0.2f, 1.0f));

		if (showMenu) {
			ImGui::Begin(_xor("##menu").c_str(), reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoCollapse);
			int imgui_width = 314, imgui_height = 450;
			ImGui::SetWindowSize(ImVec2(imgui_width, imgui_height), ImGuiCond_FirstUseEver);
			ImGui::TextColored(ImVec4(0.66f, 0.58f, 0.76f, 1.0f), _xor("PureSkill").c_str());

			AddTab(0, _xor("Aimbot").c_str());
			AddTab(1, _xor("Visuals").c_str());
			AddTab(2, _xor("Debug").c_str());

			if (tabb == 0) {
				ImGui::Checkbox(_xor("Aimbot").c_str(), &settings.MemoryAimbot);
				ImGui::Checkbox(_xor("Aimbot FOV").c_str(), &settings.FOV);
				if (settings.FOV) {
					ImGui::SliderFloat(_xor("Aimbot FOV##slider").c_str(), &settings.AimbotFOV, 0.0f, 1000.0f, _xor("%.2f").c_str());
				}
				string items[] = { _xor("Chest").c_str(), _xor("Neck").c_str(), _xor("Head").c_str() };

				ImGui::Checkbox(_xor("FOVChanger").c_str(), &settings.FOVChanger);

				if(settings.FOVChanger)
					ImGui::SliderFloat(_xor("Camera FOV##slider").c_str(), &settings.CameraFOV, 40.0f, 120.0f, _xor("%.2f").c_str());

				if (ImGui::BeginCombo("Aimbot Bone", settings.Aimbot.BoneName.c_str()))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (settings.Aimbot.BoneName == items[n]);
						if (ImGui::Selectable(items[n].c_str(), is_selected))
							settings.Aimbot.BoneName = items[n];
							if (settings.Aimbot.BoneName == _xor("Head").c_str())
								AimbotBoneIndex = 66;
							else if (settings.Aimbot.BoneName == _xor("Neck").c_str())
								AimbotBoneIndex = 65;
							else
								AimbotBoneIndex = 36;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			else if (tabb == 1) {
				ImGui::Checkbox(_xor("Player Lines").c_str(), &settings.ESP.PlayerLines);
				if (settings.ESP.PlayerLines) {
					ImGui::Spacing();
					ImGui::Checkbox(_xor("ToBots").c_str(), &settings.ESP.PlayerLineToBots);
					ImGui::SameLine();
					ImGui::Checkbox(_xor("ToPlayers").c_str(), &settings.ESP.PlayerLineToPlayers);
					ImGui::Spacing();
				}
				ImGui::Checkbox(_xor("Bone ESP").c_str(), &settings.ESP.BoneESP);
				ImGui::Checkbox(_xor("Box ESP").c_str(), &settings.ESP.BoxEsp);
				ImGui::Checkbox(_xor("Players Around").c_str(), &settings.PlayersAround);
				ImGui::Checkbox(_xor("MarkBots").c_str(), &settings.MarkBots);
				ImGui::ColorEdit4(_xor("NotVisibleColor").c_str(), settings.NotVisibleColor, ImGuiColorEditFlags_NoInputs);
				ImGui::ColorEdit4(_xor("BotColor").c_str(), settings.BotColor, ImGuiColorEditFlags_NoInputs);
			}
			else if (tabb == 2) {
				ImGui::Text(_xor("Pitch: %f\n").c_str(), Pitch);
				ImGui::Text(_xor("Pawns: %u\n").c_str(), player_list_size);
				ImGui::Text(_xor("Trampoline: 0x%llx\n").c_str(), Offsets::trampoline);
			}

			ImGui::End();
		}

		ImGui::PopStyleColor();

		ImGui::Render();
	}

	LRESULT CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_KEYUP && (wParam == VK_INSERT || (showMenu && wParam == VK_ESCAPE))) {
			showMenu = !showMenu;
			ImGui::GetIO().MouseDrawCursor = showMenu;
		}
		else if (msg == WM_QUIT && showMenu) {
			ExitProcess(0);
		}

		if (showMenu) {
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
			return TRUE;
		}

		return CallWindowProc(WndProcOriginal, hwnd, msg, wParam, lParam);
	}

	HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
		if (!device) {
			swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
			device->GetImmediateContext(&immediateContext);

			ID3D11Texture2D* renderTarget = nullptr;
			swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
			device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
			renderTarget->Release();

			ID3D11Texture2D* backBuffer = 0;
			swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
			D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
			backBuffer->GetDesc(&backBufferDesc);

			hWnd = FindWindow(_xor(L"UnrealWindow").c_str(), _xor(L"Fortnite  ").c_str());
			if (!width) {
				WndProcOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
			}

			width = (float)backBufferDesc.Width;
			height = (float)backBufferDesc.Height;
			backBuffer->Release();

			HWND targetWindow = 0;
			EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
				DWORD pid = 0;
				GetWindowThreadProcessId(hWnd, &pid);
				if (pid == GetCurrentProcessId()) {
					*reinterpret_cast<HWND*>(lParam) = hWnd;
					return FALSE;
				}

				return TRUE;
				}, reinterpret_cast<LPARAM>(&targetWindow));

			ImGui_ImplDX11_Init(targetWindow, device, immediateContext);
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImFont* font = io.Fonts->AddFontFromFileTTF(_xor("c:\\Windows\\Fonts\\Verdana.ttf").c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
			IM_ASSERT(font != NULL);
			ImGui_ImplDX11_CreateDeviceObjects();

			InitAddresses();
			utils::LoadStyle();
		}

		immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

		auto& window = BeginScene();

		if (settings.FOV) {
			window.DrawList->AddCircle(ImVec2(960, 540), settings.AimbotFOV, ImGui::GetColorU32({ 0.66f, 0.58f, 0.76f, 1.0f }), 64, 1.5f);
		}

		update_pointers(window);

		Render::EndScene(window);

		return PresentOriginal(swapChain, syncInterval, flags);
	}

	__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
		ImGui_ImplDX11_Shutdown();
		renderTargetView->Release();
		immediateContext->Release();
		device->Release();
		device = nullptr;

		return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
	}

	BOOLEAN Initialize() {
		IDXGISwapChain* swapChain = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		auto                 featureLevel = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.OutputWindow = FindWindowA(_xor("UnrealWindow").c_str(), _xor("Fortnite  ").c_str());
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;

		if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
			MessageBox(0, _xor(L"Failed to create D3D11 device and swap chain").c_str(), _xor(L"Failure").c_str(), MB_ICONERROR);
			return FALSE;
		}

		auto table = *reinterpret_cast<PVOID**>(swapChain);
		auto present = table[8];
		auto resize = table[13];

		context->Release();
		device->Release();
		swapChain->Release();

		MH_CreateHook(present, PresentHook, reinterpret_cast<PVOID*>(&PresentOriginal));
		MH_EnableHook(present);

		MH_CreateHook(resize, ResizeHook, reinterpret_cast<PVOID*>(&ResizeOriginal));
		MH_EnableHook(resize);

		return TRUE;
	}
}
