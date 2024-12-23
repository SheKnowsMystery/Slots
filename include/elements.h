#pragma once

#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "bindings.h"
#include "graphics.h"
#include "utility.h"
#include "lists.h"

#include <type_traits>
#include <vector>
#include <array>

namespace slots
{
	class Barrel : public graphics::Drawable
	{
	public:
		static constexpr size_t length   = 10;
		static constexpr size_t strip    = 3;
		static constexpr size_t viewable = strip + 1;
		static constexpr size_t target   = 2;

		static constexpr sdl::Color border = {/*.r =*/ 200, /*.g =*/ 200, /*.b =*/ 200, /*.a =*/ 255};

		static constexpr struct {
			size_t min;
			size_t max;
		} segmentation = {
			/*.min =*/ 0x04,
			/*.max =*/ 0x80,
		};
		static constexpr size_t acceleration = segmentation.min;

	private:
		using array_t = std::array<std::pair<size_t, graphics::Texture>, length>;

		array_t symbols;

		size_t current   = 0;
		size_t scrolling = 0;
		size_t segment   = segmentation.max;

		auto index(size_t _i) const -> size_t;

	public:
		Barrel() = default;

		void init(const graphics::TexturePool& _texture_pool) override;
		void draw(sdl::Renderer* _renderer) const override;
		void update() override;

		void accelerate();
		void spin();
		void decelerate();

		bool stopped() const;
		bool accelerated() const;
		auto symbol() const -> size_t;

	private:
		template <typename _Func, util::require<std::is_invocable_v<_Func, graphics::Texture&>> = 0>
		void apply(_Func _func);
		template <typename _Func, util::require<std::is_invocable_v<_Func, const graphics::Texture&>> = 0>
		void apply(_Func _func) const;
		template <typename _Func, util::require<std::is_invocable_v<_Func, graphics::Texture&, size_t>> = 0>
		void apply(_Func _func);
		template <typename _Func, util::require<std::is_invocable_v<_Func, const graphics::Texture&, size_t>> = 0>
		void apply(_Func _func) const;
	};

	template <size_t _count>
	struct Barrels : public graphics::Drawable
	{
		using array_t = std::array<Barrel, _count>;

		array_t array;

		Barrels() = default;

		void init(const graphics::TexturePool& _texture) override
		{
			for (auto& barrel : array)
				barrel.init(_texture);
		}
		void draw(sdl::Renderer* _renderer) const override
		{
			for (auto& barrel : array)
				barrel.draw(_renderer);

			graphics::Rect rect = *this;

			rect.size.x = size.x * _count;
			rect.size.y = size.y * Barrel::strip;
			graphics::draw(_renderer, rect, Barrel::border);

			rect.size.y = size.y;

			rect.position.y = position.y - size.y;
			graphics::fill(_renderer, rect, sdl::env::black);

			rect.position.y = position.y + size.y * Barrel::strip;
			graphics::fill(_renderer, rect, sdl::env::black);
		}
		void update() override
		{
			for (size_t i = 0; i < array.size(); i++)
			{
				(graphics::Rect&)array[i] = *this;
				array[i].position.x += size.x * i;
				array[i].update();
			}
		}

		static auto speed(size_t _index) -> size_t
		{
			constexpr size_t half = _count / 2;
			return (_index % 2 * half) + (_index / 2) + 1;
		}
		void accelerate()
		{
			for (auto& barrel : array)
				if (!barrel.accelerated())
				{
					barrel.accelerate();
					return;
				}
		}
		void spin()
		{
			for (auto& barrel : array)
				barrel.spin();
		}
		void decelerate()
		{
			for (auto& barrel : array)
				if (!barrel.stopped())
				{
					barrel.decelerate();
					return;
				}
		}

		bool stopped() const
		{
			bool result = true;
			for (const auto& barrel : array)
				result &= barrel.stopped();
			return result;
		}
		bool accelerated() const
		{
			bool result = true;
			for (const auto& barrel : array)
				result &= barrel.accelerated();
			return result;
		}
	};

	class Button : public graphics::Drawable
	{
		static constexpr size_t type_count = env::buttons.size();

		static constexpr sdl::Color gray{/*.r =*/ 76, /*.g =*/ 150, /*.b =*/ 29, /*.a =*/ 255};

		using textures_t = std::array<graphics::Texture, type_count>;

		textures_t textures;

		size_t type    = 0;

		bool active_  = false;
		bool pressed_ = false;

		void color(sdl::Color _color);

	public:
		Button() = default;

		void init(const graphics::TexturePool& _texture_pool) override;
		void draw(sdl::Renderer* _renderer) const override;
		void update() override;

		void set(size_t _type);

		void reset();

		void press();
		void release();
		bool pressed() const;

		void activate();
		void deactivate();
		bool active() const;
	};

	class Reward : public graphics::Drawable
	{
	public:
		static constexpr size_t alphabet_size = env::symbols.size();
		static constexpr size_t digits_count = env::symbols.size() - 1;

		static constexpr size_t multiplier = 13;

	private:
		using alphabet_t = std::array<sdl::Texture*, alphabet_size>;
		using string_t   = std::vector<graphics::Texture>;

		alphabet_t alphabet;
		string_t   string;

		void insert(sdl::Texture* _texture);

	public:
		size_t value;

		Reward() = default;

		void init(const graphics::TexturePool& _texture_pool) override;
		void draw(sdl::Renderer* _renderer) const override;
		void update() override;

		auto length() const -> size_t;
	};
}

#endif