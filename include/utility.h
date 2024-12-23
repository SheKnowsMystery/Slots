#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#include "bindings.h"

#include <initializer_list>
#include <type_traits>
#include <random>

namespace util
{
	template <bool... _pred>
	using require = std::enable_if_t<(_pred || ...), int>;

	template <typename _Derived, typename _Base>
	static constexpr bool derived = std::conjunction<std::is_base_of<_Base, _Derived>, std::is_convertible<const volatile _Derived*, const volatile _Base*>>::value;

	template <typename _Range>
	static constexpr bool range = std::is_same_v<std::void_t<decltype(std::begin(std::declval<_Range>())), decltype(std::end(std::declval<_Range>())), typename _Range::value_type>, void>;

	template <typename _Type>
	auto get(const std::initializer_list<_Type>& _list, size_t _index) -> const _Type&
	{
		return *(_list.begin() + _index);
	}

	template <typename _Range, typename _Type, require<range<_Range>, std::is_same_v<std::void_t<decltype(std::declval<_Range>().find(std::declval<_Type>()))>, void>> = 0>
	auto contains(const _Range& _range, const _Type& _value)
	{
		return _range.find(_value) != std::end(_range);
	}

	template <typename _Type>
	auto random(std::uniform_int_distribution<_Type>& _distribution) -> _Type
	{
		static auto device = std::random_device();
		static auto engine = std::default_random_engine(device());

		return _distribution(engine);
	}

	template <typename _Type, require<std::is_arithmetic_v<_Type>> = 0>
	auto operator+(sdl::FPoint _point, _Type _val) -> sdl::FPoint
	{
		return {/*.x =*/ _point.x + _val, /*.y =*/ _point.y + _val};
	}

	template <typename _Type, require<std::is_arithmetic_v<_Type>> = 0>
	auto operator+=(sdl::FPoint& _point, _Type _val) -> sdl::FPoint&
	{
		_point.x += _val;
		_point.y += _val;
		return _point;
	}

	template <typename _Type, require<std::is_arithmetic_v<_Type>> = 0>
	auto operator*(sdl::FPoint _point, _Type _val) -> sdl::FPoint
	{
		return {/*.x =*/ _point.x * _val, /*.y =*/ _point.y * _val};
	}

	template <typename _Type, require<std::is_arithmetic_v<_Type>> = 0>
	auto operator*=(sdl::FPoint& _point, _Type _val) -> sdl::FPoint&
	{
		_point.x *= _val;
		_point.y *= _val;
		return _point;
	}

	auto operator+(sdl::FPoint _p1, sdl::FPoint _p2) -> sdl::FPoint;
	auto operator+=(sdl::FPoint& _p1, sdl::FPoint _p2) -> sdl::FPoint;
	auto operator*(sdl::FPoint _p1, sdl::FPoint _p2) -> sdl::FPoint;
	auto operator*=(sdl::FPoint& _p1, sdl::FPoint _p2) -> sdl::FPoint;
}

#endif