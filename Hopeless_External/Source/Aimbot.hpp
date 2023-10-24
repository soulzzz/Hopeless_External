#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Game.h"
#include "Entity.h"
#include <iostream>


namespace AimControl
{
	inline int HotKey = VK_LMENU;
	inline float AimFov = 5;
	inline float Smooth = 0.7;
	inline Vec2 RCSScale = { 1.f,1.f };
	inline int RCSBullet = 1;
	inline std::vector<int> HotKeyList{ VK_LMENU, VK_RBUTTON, VK_XBUTTON1, VK_XBUTTON2, VK_CAPITAL, VK_LSHIFT, VK_LCONTROL };

	inline void SetHotKey(int Index)
	{
		HotKey = HotKeyList.at(Index);
	}

	inline void AimBot(const CEntity& Local, Vec3 LocalPos, Vec3 AimPos)
	{
		float Yaw, Pitch;
		float Distance, Norm;
		Vec3 OppPos;
		
		OppPos = AimPos - LocalPos;

		Distance = sqrt(pow(OppPos.x, 2) + pow(OppPos.y, 2));

		Yaw = atan2f(OppPos.y, OppPos.x) * 57.295779513 - Local.Pawn.ViewAngle.y;
		Pitch = -atan(OppPos.z / Distance) * 57.295779513 - Local.Pawn.ViewAngle.x;
		Norm = sqrt(pow(Yaw, 2) + pow(Pitch, 2));
		if (MenuConfig::Debug) {
			Vec3 mViewAngle = CalculateAngle(LocalPos, AimPos, Vec3{ Local.Pawn.ViewAngle.x,Local.Pawn.ViewAngle.y,0.f });
			Gui.Text(mViewAngle.toString(), Vec2{100,340}, ImColor(255, 255, 255, 255));
			Vec2 originCalcViewAngle = Vec2{ Yaw ,Pitch };
			Gui.Text("AimViewAngle:" + originCalcViewAngle.toString(), Vec2{ 100,360 }, ImColor(255, 255, 255, 255));
			Gui.Text(std::to_string(Local.Pawn.ViewAngle.x), Vec2{ 100,400 }, ImColor(255, 255, 255, 255));
			Gui.Text(std::to_string(atan2f(OppPos.y, OppPos.x) * 57.295779513), Vec2{ 100,420 }, ImColor(255, 255, 255, 255));
			Gui.Text(std::to_string(Norm), Vec2{ 100,440 }, ImColor(255, 255, 255, 255));
		}
		if (Norm > AimFov)
			return;

		Yaw = Yaw * Smooth + Local.Pawn.ViewAngle.y;
		Pitch = Pitch * Smooth + Local.Pawn.ViewAngle.x;

		// Recoil control
		if (Local.Pawn.ShotsFired > RCSBullet)
		{
			Vec2 PunchAngle;
			if (Local.Pawn.AimPunchCache.Count <= 0 && Local.Pawn.AimPunchCache.Count > 0xFFFF)
				return;
			if (!ProcessMgr.ReadMemory<Vec2>(Local.Pawn.AimPunchCache.Data + (Local.Pawn.AimPunchCache.Count - 1) * sizeof(Vec3), PunchAngle))
				return;

			Yaw = Yaw - PunchAngle.y * RCSScale.x;
			Pitch = Pitch - PunchAngle.x * RCSScale.y;
		}
		Vec2 AimViewAngle{ Yaw ,Pitch };
		ClampAngles(AimViewAngle);
		if (MenuConfig::Debug) {
			Vec2 AimViewAngle{ Yaw ,Pitch };
			Gui.Text("AimViewAngle:" + AimViewAngle.toString(), Vec2{ 100,380 }, ImColor(255, 255, 255, 255));
		}
		gGame.SetViewAngle(AimViewAngle);
	}

	void ClampAngles(Vec2& angles)
	{
		while (angles.x > 180.0f)
			angles.x -= 360.0f;

		while (angles.x < -180.0f)
			angles.x += 360.0f;

		while (angles.y > 89.0f)
			angles.y -= 180.0f;

		while (angles.y < -89.0f)
			angles.y += 180.0f;
	}

}