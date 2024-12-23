#include "bindings.h"
#include "graphics.h"
#include "utility.h"

#include <SDL_log.h>
#include <string_view>
#include <stdexcept>
#include <string>

namespace slots::graphics::exc
{
	class graphics_error : public std::runtime_error
	{
	public:
		using Base = std::runtime_error;

		const int code = 0;

		graphics_error(std::string_view _message, int _code = 0) :
			Base(std::string(_message)), code(_code) {}
	};

	class sdl_error : public graphics_error
	{
	public:
		using Base = graphics_error;
		sdl_error(int _code = 0) : Base(SDL_GetError(), _code) {}
	};

	class img_error : public graphics_error
	{
	public:
		using Base = graphics_error;
		img_error(int _code = 0) : Base(IMG_GetError(), _code) {}
	};
}

namespace slots::graphics
{
	Rect::operator sdl::FRect() const
	{
		return {
			/*.x =*/ position.x * scaling.x,
			/*.y =*/ position.y * scaling.y,
			/*.w =*/ size.x * scaling.x,
			/*.h =*/ size.y * scaling.y,
		};
	}

	bool Rect::contains(sdl::FPoint _point) const
	{
		sdl::FRect rect = *this;
		return SDL_PointInFRect(&_point, &rect);
	}

	bool Rect::contains(sdl::Point _point) const
	{
		sdl::FPoint point = {/*.x =*/ (float)_point.x, /*.y =*/ (float)_point.y};
		sdl::FRect rect = *this;
		return SDL_PointInFRect(&point, &rect);
	}

	// -----------------------------------------

	void Texture::draw(sdl::Renderer* _renderer) const
	{
		auto [r, g, b, a]  = color;
		sdl::FRect dstrect = destination;
		if (int error = SDL_SetTextureColorMod(ptr, r, g, b))
			throw exc::sdl_error(error);
		if (int error = SDL_RenderCopyF(_renderer, ptr, nullptr, &dstrect))
			throw exc::sdl_error(error);
	}

	void Texture::draw(sdl::Renderer* _renderer, sdl::Rect _source) const
	{
		auto [r, g, b, a]  = color;
		sdl::FRect dstrect = destination;
		if (int error = SDL_SetTextureColorMod(ptr, r, g, b))
			throw exc::sdl_error(error);
		if (int error = SDL_RenderCopyF(_renderer, ptr, &_source, &dstrect))
			throw exc::sdl_error(error);
	}

	// -----------------------------------------

	void TexturePool::add(std::string_view _identifier, std::string_view _filename)
	{
		sdl::Texture* texture = IMG_LoadTexture(renderer, _filename.data());
		if (!texture)
			throw exc::img_error();
		dict[_identifier.data()] = texture;
	}

	TexturePool::~TexturePool()
	{
		for (auto& [filename, texture] : dict)
			SDL_DestroyTexture(texture);
	}

	auto TexturePool::operator[](std::string_view _identifier) const -> sdl::Texture*
	{
		if (util::contains(dict, _identifier.data()))
			return dict.at(_identifier.data());
		return nullptr;
	}

	// -----------------------------------------

	auto Frame::scaling() const -> sdl::FPoint
	{
		sdl::Point current;
		SDL_GetWindowSize(window, &current.x, &current.y);
		return {
			/*.x =*/ (float)current.x / (float)size.x,
			/*.y =*/ (float)current.y / (float)size.y
		};
	}

	void Frame::present() const
	{
		SDL_RenderPresent(renderer);
	}

	void Frame::clear(sdl::Color _color) const
	{
		if (int error = SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a))
			throw exc::sdl_error(error);
		SDL_RenderClear(renderer);
	}

	void Frame::clear() const
	{
		if (int error = SDL_RenderClear(renderer))
			throw exc::sdl_error(error);
	}

	// -----------------------------------------

	void draw(sdl::Renderer* _renderer, const Rect& _rect, sdl::Color _color)
	{
		auto [r, g, b, a] = _color;
		sdl::FRect rect = _rect;
		SDL_SetRenderDrawColor(_renderer, r, g, b, a);
		SDL_RenderDrawRectF(_renderer, &rect);
	}

	void fill(sdl::Renderer* _renderer, const Rect& _rect, sdl::Color _color)
	{
		auto [r, g, b, a] = _color;
		sdl::FRect rect = _rect;
		SDL_SetRenderDrawColor(_renderer, r, g, b, a);
		SDL_RenderFillRectF(_renderer, &rect);
	}

	auto motion(const sdl::Event& _event) -> sdl::FPoint
	{
		return {/*.x =*/ (float)_event.motion.x, /*.y =*/ (float)_event.motion.y};
	}

	void context(const WindowData& _window_data, const type::textures& _textures, type::function _function)
	{
		sdl::Window*   window   = nullptr;
		sdl::Renderer* renderer = nullptr;
#ifdef _DEBUG
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#else
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_ERROR);
#endif
		try
		{
			if (int error = SDL_Init(_window_data.flags.sdl))
				throw exc::sdl_error(error);
			if (int flags = IMG_Init(_window_data.flags.img); flags ^ _window_data.flags.img)
				throw exc::img_error();

			window = SDL_CreateWindow(_window_data.title.data(), _window_data.rect.x, _window_data.rect.y, _window_data.rect.w, _window_data.rect.h, _window_data.flags.window);
			if (!window)
				throw exc::sdl_error();

			renderer = SDL_CreateRenderer(window, -1, _window_data.flags.renderer);
			if (!renderer)
				throw exc::sdl_error();

			if (int error = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND))
				throw exc::sdl_error(error);

			auto texture_pool = TexturePool(renderer);
			for (const auto &[identifier, filename] : _textures)
				texture_pool.add(identifier, filename);

			auto frame = Frame(window, renderer, {/*.x =*/ _window_data.rect.w, /*.y =*/ _window_data.rect.h});
			_function(frame, texture_pool);
		}
		catch (const exc::graphics_error& error)
		{
			SDL_LogError(error.code, error.what());
		}
		if (renderer)
			SDL_DestroyRenderer(renderer);
		if (window)
			SDL_DestroyWindow(window);
		IMG_Quit();
		SDL_Quit();
	}
}