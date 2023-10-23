#pragma once
#include "UI_ImGui_Exception.hpp"
#include <iostream>
#include <string>
#include <functional>
#include <codecvt>
#include <vector>
#include <numbers>
#include <cmath>
#include <dwmapi.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#include <d3d11.h>
#pragma comment(lib,"dwmapi.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Vec2
{
public:
	float x, y;
public:
	Vec2() :x(0.f), y(0.f) {}
	Vec2(float x_, float y_) :x(x_), y(y_) {}
	Vec2(ImVec2 ImVec2_) :x(ImVec2_.x), y(ImVec2_.y) {}
	Vec2 operator=(ImVec2 ImVec2_)
	{
		x = ImVec2_.x;
		y = ImVec2_.y;
		return *this;
	}
	Vec2 operator+(Vec2 Vec2_)
	{
		return { x + Vec2_.x,y + Vec2_.y };
	}
	Vec2 operator-(Vec2 Vec2_)
	{
		return { x - Vec2_.x,y - Vec2_.y };
	}
	Vec2 operator*(Vec2 Vec2_)
	{
		return { x * Vec2_.x,y * Vec2_.y };
	}
	Vec2 operator/(Vec2 Vec2_)
	{
		return { x / Vec2_.x,y / Vec2_.y };
	}
	Vec2 operator*(float n)
	{
		return { x / n,y / n };
	}
	Vec2 operator/(float n)
	{
		return { x / n,y / n };
	}
	bool operator==(Vec2 Vec2_)
	{
		return x == Vec2_.x && y == Vec2_.y;
	}
	bool operator!=(Vec2 Vec2_)
	{
		return x != Vec2_.x || y != Vec2_.y;
	}
	ImVec2 ToImVec2()
	{
		return ImVec2(x, y);
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2));
	}
	float DistanceTo(const Vec2& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2));
	}
};

class Vec3
{
public:
	float x, y, z;
public:
	constexpr Vec3() :x(0.f), y(0.f), z(0.f) {}
	constexpr Vec3(float x_,float y_, float z_) :x(x_), y(y_), z(z_) {}
	constexpr const Vec3 operator+(const Vec3& Vec3_) const
	{
		return Vec3{ x + Vec3_.x,y + Vec3_.y,z + Vec3_.z };
	}
	constexpr const Vec3 operator-(const Vec3& Vec3_) const
	{
		return Vec3{ x - Vec3_.x,y - Vec3_.y,z - Vec3_.z };
	}
	constexpr const Vec3 operator*(const Vec3& Vec3_) const
	{
		return Vec3{ x * Vec3_.x,y * Vec3_.y,z * Vec3_.z };
	}
	constexpr const Vec3 operator/(const Vec3& Vec3_) const
	{
		return Vec3{ x / Vec3_.x,y / Vec3_.y,z / Vec3_.z };
	}
	constexpr const Vec3 operator*(const float n) const
	{
		return Vec3{ x * n,y * n,z * n };
	}
	constexpr const Vec3 operator/(const float n) const
	{
		return Vec3{ x / n,y / n,z / n };
	}
	bool operator==(const Vec3& Vec3_)
	{
		return x == Vec3_.x && y == Vec3_.y && z == Vec3_.z;
	}
	bool operator!=(const Vec3& Vec3_)
	{
		return x != Vec3_.x || y != Vec3_.y || z != Vec3_.z;
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}
	float DistanceTo(const Vec3& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2) + powf(Pos.z - z, 2));
	}
	constexpr const Vec3 ToAngle() const {
		return Vec3{
			std::atan2(-z,std::hypot(x,y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y,x) * (180.0f / std::numbers::pi_v<float>),
			0.0f
		};
	}
};
constexpr const Vec3 CalculateAngle(const Vec3& localPosition, const Vec3& enemyPosition, const Vec3& viewAngle)
{
	return (enemyPosition - localPosition).ToAngle() - viewAngle;
}

template <typename T>
class Singleton
{
public:
	static T& get()
	{
		static T instance;
		return instance;
	}
};

namespace UIImGui
{
	class D3DDevice
	{
	public:
		ID3D11Device* g_pd3dDevice = nullptr;
		ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
		IDXGISwapChain* g_pSwapChain = nullptr;
		ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
#ifdef _CONSOLE
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
#endif
	};

	static D3DDevice g_Device;

	enum WindowType
	{
		NEW,
		ATTACH
	};

	class WindowData
	{
	public:
		HWND  hWnd = NULL;
		HINSTANCE hInstance = nullptr;
		std::string Name;
		std::wstring wName;
		std::string ClassName;
		std::wstring wClassName;
		Vec2 Pos;
		Vec2 Size;
		ImColor BgColor{ 255, 255, 255 };
	};

	class UIImGui_Base
	{
	public:
		// 回调函数
		std::function<void()> CallBackFn = nullptr;
		// 退出标识
		bool EndFlag = false;
	public:
		// 窗口数据
		WindowData Window;
		// 目标窗口数据
		WindowData DestWindow;
	public:
		// 创建一个新窗口
		virtual void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack) = 0;
		// 退出
		virtual void Quit() { EndFlag = true; };
	public:
		virtual bool CreateMyWindow() = 0;
		virtual void MainLoop() {};
		bool InitImGui(ID3D11Device* device, ID3D11DeviceContext* device_context);
		void CleanImGui();
		std::wstring StringToWstring(std::string& str);
	};
}