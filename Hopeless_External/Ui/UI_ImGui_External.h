#pragma once
#include "UI_ImGui_Base.h"


namespace UIImGui
{
	class UIImGui_External : public UIImGui_Base
	{
	private:
		// 启动类型
		WindowType Type = NEW;
	public:
		// 创建一个新窗口
		void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack);
		// 附加到另一个窗口上
		void AttachAnotherWindow(std::string DestWindowName, std::string DestWindowClassName, std::function<void()> CallBack);
	private:
		void MainLoop();
		bool UpdateWindowData();
		bool CreateMyWindow();
		bool PeekEndMessage();
	};
}