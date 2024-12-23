#include "utility.h"

namespace util
{
	auto operator+(sdl::FPoint _p1, sdl::FPoint _p2) -> sdl::FPoint
	{
		return {/*.x =*/ _p1.x + _p2.x, /*.y =*/ _p1.y + _p2.y};
	}

	auto operator+=(sdl::FPoint& _p1, sdl::FPoint _p2) -> sdl::FPoint
	{
		_p1.x += _p2.x;
		_p1.y += _p2.y;
		return _p1;
	}

	auto operator*(sdl::FPoint _p1, sdl::FPoint _p2) -> sdl::FPoint
	{
		return {/*.x =*/ _p1.x * _p2.x, /*.y =*/ _p1.y * _p2.y};
	}

	auto operator*=(sdl::FPoint& _p1, sdl::FPoint _p2) -> sdl::FPoint
	{
		_p1.x *= _p2.x;
		_p1.y *= _p2.y;
		return _p1;
	}
}