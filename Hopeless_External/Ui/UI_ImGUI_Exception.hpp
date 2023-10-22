#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <exception>


namespace UIImGui
{
	class UIException : public std::exception
	{
	public:
		UIException() :Error_("[UI-Exception] Unkown Error") {}
		UIException(std::string Error) :Error_("[UI-Exception] " + Error) {}
		char const* what() const throw()
		{
			return Error_.c_str();
		}
	private:
		std::string Error_ = "";
	};
}