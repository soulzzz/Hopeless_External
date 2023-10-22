#include "Cheats.h"
#include "Render.hpp"
#include <chrono>
#include "MenuConfig.hpp"
#include <iostream>
//#include "Utils/ConfigMenu.hpp"
//#include "Utils/ConfigSaver.hpp"

void Cheats::Menu()
{
	static bool IsMenuInit = false;
	if (!IsMenuInit)
	{
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.75;
		IsMenuInit = true;
	}

	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::BeginTabBar("Cheat");
		// esp menu
		if (ImGui::BeginTabItem("ESP"))
		{
			Gui.MyCheckBox("BoxESP", &MenuConfig::ShowBoxESP);
			ImGui::SameLine();
			ImGui::ColorEdit4("##BoxColor", reinterpret_cast<float*>(&MenuConfig::BoxColor), ImGuiColorEditFlags_NoInputs);

			ImGui::Combo("BoxType", &MenuConfig::BoxType, "Normal\0Dynamic");

			Gui.MyCheckBox("BoneESP", &MenuConfig::ShowBoneESP);
			ImGui::SameLine();
			ImGui::ColorEdit4("##BoneColor", reinterpret_cast<float*>(&MenuConfig::BoneColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox("HealthBar", &MenuConfig::ShowHealthBar);
			ImGui::Combo("HealthBarType", &MenuConfig::HealthBarType, "Vetical\0Horizontal");

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("AIM"))
		{
			Gui.MyCheckBox("Enable", &MenuConfig::AimSwitch);
			if (ImGui::Combo("AimKey", &MenuConfig::AimHotKey, "MENU\0RBUTTON\0XBUTTON1\0XBUTTON2\0CAPITAL\0SHIFT\0CONTROL"))
			{
				AimControl::SetHotKey(MenuConfig::AimHotKey);
			}

			float FovMin = 0.1f, FovMax = 89.f;
			float SmoothMin = 0.1f, SmoothMax = 1.f;
			Gui.SliderScalarEx1("AimFov", ImGuiDataType_Float, &AimControl::AimFov, &FovMin, &FovMax, "%.1f", ImGuiSliderFlags_None);
			Gui.MyCheckBox("FovCircle", &MenuConfig::ShowAimFovRange);
			ImGui::SameLine();
			ImGui::ColorEdit4("##FovCircleColor", reinterpret_cast<float*>(&MenuConfig::AimFovRangeColor), ImGuiColorEditFlags_NoInputs);

			Gui.SliderScalarEx1("Smooth", ImGuiDataType_Float, &AimControl::Smooth, &SmoothMin, &SmoothMax, "%.1f", ImGuiSliderFlags_None);
			if (ImGui::Combo("AimPos", &MenuConfig::AimPosition, "Head\0Neck\0Spine"))
			{
				switch (MenuConfig::AimPosition)
				{
				case 0:
					MenuConfig::AimPositionIndex = BONEINDEX::head;
					break;
				case 1:
					MenuConfig::AimPositionIndex = BONEINDEX::neck_0;
					break;
				case 2:
					MenuConfig::AimPositionIndex = BONEINDEX::spine_1;
					break;
				default:
					break;
				}
			}
			int BulletMin = 1, BulletMax = 6;
			float RecoilMin = 0.f, RecoilMax = 2.f;
			Gui.SliderScalarEx1("Start Bullet", ImGuiDataType_U32, &AimControl::RCSBullet, &BulletMin, &BulletMax, "%d", ImGuiSliderFlags_None);
			Gui.SliderScalarEx1("RCS Yaw", ImGuiDataType_Float, &AimControl::RCSScale.x, &RecoilMin, &RecoilMax, "%.1f", ImGuiSliderFlags_None);
			Gui.SliderScalarEx1("RCS Pitch", ImGuiDataType_Float, &AimControl::RCSScale.y, &RecoilMin, &RecoilMax, "%.1f", ImGuiSliderFlags_None);
			Gui.MyCheckBox("VisibleCheck", &MenuConfig::VisibleCheck);

			ImGui::EndTabItem();
		}

		// TeamCheck
		Gui.MyCheckBox("TeamCheck", &MenuConfig::TeamCheck);
		ImGui::SameLine();

		// OBS Bypass
		Gui.MyCheckBox("OBSBypass", &MenuConfig::OBSBypass);

		//Bunnyhopping
		Gui.MyCheckBox("Bunnyhop ", &MenuConfig::BunnyHop);
		ImGui::SameLine();
	

		ImGui::Text("[HOME] HideMenu");

		ImGui::EndTabBar();
	}ImGui::End();
}

void Cheats::Run()
{
	// Show menu
	static std::chrono::time_point LastTimePoint = std::chrono::steady_clock::now();
	auto CurTimePoint = std::chrono::steady_clock::now();

	if (GetAsyncKeyState(VK_HOME)
		&& CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150))
	{
		// Check key state per 150ms once to avoid loop.
		MenuConfig::ShowMenu = !MenuConfig::ShowMenu;
		LastTimePoint = CurTimePoint;
	}

	if (MenuConfig::ShowMenu)
		Menu();

	// Update matrix
	if (!ProcessMgr.ReadMemory(gGame.GetViewMatrixAddress(), gGame.View.Matrix, 64))
		return;

	// Update EntityList Entry
	gGame.UpdateEntityListEntry();

	DWORD64 LocalControllerAddress = 0;
	DWORD64 LocalPawnAddress = 0;

	if (!ProcessMgr.ReadMemory(gGame.GetLocalControllerAddress(), LocalControllerAddress))
		return;
	if (!ProcessMgr.ReadMemory(gGame.GetLocalPawnAddress(), LocalPawnAddress))
		return;

	//// LocalEntity
	CEntity LocalEntity;
	static int LocalPlayerControllerIndex = 1;
	if (!LocalEntity.UpdateController(LocalControllerAddress))
		return;
	if (!LocalEntity.UpdatePawn(LocalPawnAddress))
		return;

	// HealthBar Map
	static std::map<DWORD64, Render::HealthBar> HealthBarMap;

	//// AimBot data
	float DistanceToSight = 0;
	float MaxAimDistance = 100000;
	Vec3  HeadPos{ 0,0,0 };
	Vec3  AimPos{ 0,0,0 };

	//// Radar Data
	//Base_Radar Radar;
	//if (MenuConfig::ShowRadar)
	//	RadarSetting(Radar);

	for (int i = 0; i < 64; i++)
	{
		CEntity Entity;
		DWORD64 EntityAddress = 0;
		if (!ProcessMgr.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (i + 1) * 0x78, EntityAddress))
			continue;

		if (EntityAddress == LocalEntity.Controller.Address)
		{
			LocalPlayerControllerIndex = i;
			continue;
		}

		if (!Entity.UpdateController(EntityAddress))
			continue;
		if (!Entity.UpdatePawn(Entity.Pawn.Address))
			continue;
		if (MenuConfig::TeamCheck && Entity.Controller.TeamID == LocalEntity.Controller.TeamID)
			continue;

		if (!Entity.IsAlive())
			continue;

		//	// Add entity to radar
		//	if (MenuConfig::ShowRadar)
		//		Radar.AddPoint(LocalEntity.Pawn.Pos, LocalEntity.Pawn.ViewAngle.y, Entity.Pawn.Pos, ImColor(237, 85, 106, 200), MenuConfig::RadarType, Entity.Pawn.ViewAngle.y);

		if (!Entity.IsInScreen())
			continue;
		//	// Bone Debug
		///*	for (int BoneIndex = 0; BoneIndex < Entity.BoneData.BonePosList.size(); BoneIndex++)
		//	{
		//		Vec2 ScreenPos{};
		//		if (gGame.View.WorldToScreen(Entity.BoneData.BonePosList[BoneIndex].Pos, ScreenPos))
		//		{
		//			Gui.Text(std::to_string(BoneIndex), ScreenPos, ImColor(255, 255, 255, 255));
		//		}
		//	}*/

		DistanceToSight = Entity.GetBone().BonePosList[BONEINDEX::head].ScreenPos.DistanceTo({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });


		if (DistanceToSight < MaxAimDistance)
		{
			MaxAimDistance = DistanceToSight;
			// From: https://github.com/redbg/CS2-Internal/blob/fc8e64430176a62f8800b7467884806708a865bb/src/include/Cheats.hpp#L129
			if (!MenuConfig::VisibleCheck ||
				Entity.Pawn.bSpottedByMask & (DWORD64(1) << (LocalPlayerControllerIndex)) ||
				LocalEntity.Pawn.bSpottedByMask & (DWORD64(1) << (i)))
			{
				AimPos = Entity.GetBone().BonePosList[MenuConfig::AimPositionIndex].Pos;
				if (MenuConfig::AimPositionIndex == BONEINDEX::head)
					AimPos.z -= 1.f;
			}
		}

		// Draw Bone
		if (MenuConfig::ShowBoneESP)
			Render::DrawBone(Entity, MenuConfig::BoneColor, 1.3);

		//	// Draw eyeRay
		//	if (MenuConfig::ShowEyeRay)
		//		Render::ShowLosLine(Entity, 50, MenuConfig::EyeRayColor, 1.3);

			// Box
		ImVec4 Rect;
		switch (MenuConfig::BoxType)
		{
		case 0:
			Rect = Render::Get2DBox(Entity);
			break;
		case 1:
			Rect = Render::Get2DBoneRect(Entity);
			break;
		default:
			break;
		}

		//	// Line to enemy
		//	if (MenuConfig::ShowLineToEnemy)
		//		Render::LineToEnemy(Rect, MenuConfig::LineToEnemyColor, 1.2);

			// Draw Box
		if (MenuConfig::ShowBoxESP)
			Gui.Rectangle({ Rect.x,Rect.y }, { Rect.z,Rect.w }, MenuConfig::BoxColor, 1.3);

		// Draw HealthBar
		if (MenuConfig::ShowHealthBar)
		{
			ImVec2 HealthBarPos, HealthBarSize;
			if (MenuConfig::HealthBarType == 0)
			{
				// Vertical
				HealthBarPos = { Rect.x - 7.f,Rect.y };
				HealthBarSize = { 7 ,Rect.w };
			}
			else
			{
				// Horizontal
				HealthBarPos = { Rect.x + Rect.z / 2 - 70 / 2,Rect.y - 13 };
				HealthBarSize = { 70,8 };
			}
			Render::DrawHealthBar(EntityAddress, 100, Entity.Pawn.Health, HealthBarPos, HealthBarSize, MenuConfig::HealthBarType);
		}

		//	// Draw weaponName
		//	if (MenuConfig::ShowWeaponESP)
		//		Gui.StrokeText(Entity.Pawn.WeaponName, { Rect.x,Rect.y + Rect.w }, ImColor(255, 255, 255, 255), 14);

		//	if (MenuConfig::ShowDistance)
		//		Render::DrawDistance(LocalEntity, Entity, Rect);

		//	if (MenuConfig::ShowPlayerName)
		//	{
		//		if (MenuConfig::HealthBarType == 0)
		//			Gui.StrokeText(Entity.Controller.PlayerName, { Rect.x + Rect.z / 2,Rect.y - 14 }, ImColor(255, 255, 255, 255), 14, true);
		//		else
		//			Gui.StrokeText(Entity.Controller.PlayerName, { Rect.x + Rect.z / 2,Rect.y - 13 - 14 }, ImColor(255, 255, 255, 255), 14, true);
		//	}

		//}

		//// Fov line
		//if (MenuConfig::ShowFovLine)
		//	Render::DrawFov(LocalEntity, MenuConfig::FovLineSize, MenuConfig::FovLineColor, 1);

		//// Radar render
		//if (MenuConfig::ShowRadar)
		//{
		//	Radar.Render();
		//	//End for radar window
		//	ImGui::End();
		//}

		//// TriggerBot
		//if (MenuConfig::TriggerBot && GetAsyncKeyState(TriggerBot::HotKey))
		//	TriggerBot::Run(LocalEntity);

		//// HeadShoot Line
		//if (MenuConfig::ShowHeadShootLine)
		//	Render::HeadShootLine(LocalEntity, MenuConfig::HeadShootLineColor);

		//// CrossHair
		//if (MenuConfig::ShowCrossHair)
		//	Render::DrawCrossHair();

		//// Fov circle
		//if (MenuConfig::ShowAimFovRange)
		//	Render::DrawFovCircle(LocalEntity);

		if (MenuConfig::BunnyHop)
			Bunnyhop::Run(LocalEntity);

		if (MenuConfig::AimSwitch && GetAsyncKeyState(AimControl::HotKey))
		{
			if (AimPos != Vec3(0, 0, 0))
			{
				AimControl::AimBot(LocalEntity, LocalEntity.Pawn.CameraPos, AimPos);
			}
		}
	}
}