#pragma once
#include "Game.h"
#include "Bone.h"
#include "UI_ImGui_Base.h"

namespace MenuConfig
{
	inline bool Debug = true;
	inline bool ShowBoneESP = true;
	inline bool ShowBoxESP = true;
	inline bool ShowHealthBar = true;
	
	// 0: normal 1: dynamic
	inline int  BoxType = 0;
	// 0: Vertical 1: Horizontal
	inline int  HealthBarType = 0;

	inline ImColor BoneColor = ImColor(255, 255, 255, 255);
	inline ImColor BoxColor = ImColor(255, 255, 255, 255);

	inline bool AimSwitch = true;
	inline int AimHotKey = 0;
	// 0: head 1: neck 3: spine
	inline int  AimPosition = 0;
	inline bool ShowAimFovRange = true;
	inline ImColor AimFovRangeColor = ImColor(230, 230, 230, 255);
	inline DWORD  AimPositionIndex = BONEINDEX::head;
	inline bool VisibleCheck = true;


	inline bool ShowMenu = true;
	inline bool BunnyHop = false;
	inline bool TeamCheck = true;
	inline bool OBSBypass = false;

}