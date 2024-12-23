#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "bindings.h"
#include "utility.h"

#include <map>
#include <string>
#include <string_view>
#include <initializer_list>

namespace slots::graphics
{
	struct Rect
	{
		sdl::FPoint size     = {};
		sdl::FPoint position = {};
		sdl::FPoint scaling  = {/*.x =*/ 1.F, /*.y =*/ 1.F};

		Rect() = default;
		Rect(const Rect&) = default;
		Rect(Rect&&) = default;

		auto operator=(const Rect&) -> Rect& = default;
		auto operator=(Rect&&)      -> Rect& = default;

		operator sdl::FRect() const;

		bool contains(sdl::FPoint _point) const;
		bool contains(sdl::Point _point) const;
	};

	struct Texture
	{
		Rect destination;

		sdl::Texture* ptr     = nullptr;
		sdl::Color    color   = {/*.r =*/ 255, /*.g =*/ 255, /*.b =*/ 255, /*.a =*/ 255};

		void draw(sdl::Renderer* _renderer) const;
		void draw(sdl::Renderer* _renderer, sdl::Rect _source) const;
	};

	class TexturePool
	{
		sdl::Renderer* renderer = nullptr;
		std::map<std::string, sdl::Texture*> dict;

	public:
		TexturePool(sdl::Renderer* _renderer) : renderer(_renderer) {};
		~TexturePool();

		void add(std::string_view _identifier, std::string_view _filename);
		auto operator[](std::string_view _identifier) const -> sdl::Texture*;
	};

	struct Drawable : virtual Rect
	{
		virtual void init(const graphics::TexturePool& _texture_pool) = 0;
		virtual void draw(sdl::Renderer* _renderer) const = 0;
		virtual void update() = 0;
	};

	class Frame
	{
		sdl::Window*   window   = nullptr;
		sdl::Renderer* renderer = nullptr;

	public:
		const sdl::Point size = {};

	public:
		Frame(sdl::Window* _window, sdl::Renderer* _renderer, sdl::Point _size) :
			window(_window), renderer(_renderer), size(_size) {}

	private:
		template <typename _Func,typename... _Drawable, util::require<std::is_invocable_v<_Func, Drawable&>, util::derived<_Drawable, Drawable>...> = 0>
		void apply(_Func _func, _Drawable&... _objects) const
		{
			(void)std::initializer_list<int>{(_func(_objects), 0)...};
		}

		template <typename _Func, typename... _Drawable, util::require<std::is_invocable_v<_Func, const Drawable&>, util::derived<_Drawable, Drawable>...> = 0>
		void apply(_Func _func, const _Drawable&... _objects) const
		{
			(void)std::initializer_list<int>{(_func(_objects), 0)...};
		}

	public:
		auto scaling() const -> sdl::FPoint;

		template <typename... _Drawable, util::require<util::derived<_Drawable, Drawable>...> = 0>
		void scale(_Drawable&... _objects) const
		{
			apply([this](Drawable& _obj){ _obj.scaling = scaling(); }, _objects...);
		}
		template <typename... _Drawable, util::require<util::derived<_Drawable, Drawable>...> = 0>
		void update(_Drawable&... _objects) const
		{
			apply([this](Drawable& _obj){ _obj.update(); }, _objects...);
		}
		template <typename... _Drawable, util::require<util::derived<_Drawable, Drawable>...> = 0>
		void draw(const _Drawable&... _objects) const
		{
			apply([this](const Drawable& _obj){ _obj.draw(renderer); }, _objects...);
		}

		void present() const;

		void clear(sdl::Color _color) const;
		void clear() const;
	};

	namespace type
	{
		using textures = std::map<std::string, std::string>;
		using function = void (&)(const Frame&, const TexturePool&);
	}

	struct WindowData
	{
		std::string title;
		sdl::Rect   rect;
		struct {
			Uint32 sdl;
			Uint32 img;
			Uint32 window;
			Uint32 renderer;
		} flags;
	};

	void draw(sdl::Renderer* _renderer, const Rect& _rect, sdl::Color _color = sdl::env::white);
	void fill(sdl::Renderer* _renderer, const Rect& _rect, sdl::Color _color = sdl::env::white);

	auto motion(const sdl::Event& _event) -> sdl::FPoint;

	void context(const WindowData& _window_data, const type::textures& _textures, type::function _function);
}

#endif