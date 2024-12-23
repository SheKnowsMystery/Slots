#include "bindings.h"
#include "graphics.h"
#include "elements.h"
#include "utility.h"
#include "lists.h"

#include <limits>
#include <random>

namespace slots
{
	auto Barrel::index(size_t _i) const -> size_t
	{
		if ((current + _i) < target)
			return (length - target + current + _i) % length;
		else
			return (current + _i - 2) % length;
	}

	void Barrel::init(const graphics::TexturePool& _texture_pool)
	{
		static constexpr struct {
			Uint8 min   = std::numeric_limits<Uint8>::max() / 2;
			Uint8 max   = std::numeric_limits<Uint8>::max();
			size_t size = sizeof(Uint8) * 3;
		} rgb;

		static auto id    = std::uniform_int_distribution<Uint16>(0, rgb.size);
		static auto max   = std::uniform_int_distribution<Uint16>(false, true);
		static auto color = std::uniform_int_distribution<Uint16>(rgb.min, rgb.max);

		static auto name_id = std::uniform_int_distribution<size_t>(0, slots::env::cats.size() - 1);

		auto normalized = []() -> sdl::Color
		{
			Uint8 array[rgb.size];
			size_t channel = util::random(id);
			array[channel] = util::random(color);
			for (size_t i = 0; i < rgb.size; i++)
				if (i != channel)
					array[i] = util::random(max) ? rgb.max : rgb.min;
			auto [r, g, b] = array;
			return {/*.r =*/ r, /*.g =*/ g, /*.b =*/ b, /*.a =*/ rgb.max};
		};

		for (auto& [id, symbol] : symbols)
		{
			id = util::random(name_id);
			std::string_view name = util::get(slots::env::cats, id);
			if (sdl::Texture* texture = _texture_pool[name]; texture)
			{
				symbol.ptr   = texture;
				symbol.color = normalized();
			}
		}
	}

	void Barrel::draw(sdl::Renderer* _renderer) const
	{
		apply(
			[this, _renderer](const graphics::Texture& _symbol, size_t _index)
			{
				_symbol.draw(_renderer);
				graphics::Rect rect = *this;
				rect.position.y += size.y * _index - size.y + size.y / segment * scrolling;
				graphics::draw(_renderer, rect, border);
			}
		);
	}

	void Barrel::update()
	{
		apply(
			[this](graphics::Texture& _texture, size_t _index)
			{
				using util::operator+;
				using util::operator*;
				using util::operator+=;
				using util::operator*=;
				_texture.destination = *this;
				_texture.destination.size *= .8F;
				_texture.destination.position += size * .1F;
				_texture.destination.position.y -= size.y;
				_texture.destination.position.y += size.y * _index + size.y / segment * scrolling;
			}
		);
	}

	void Barrel::accelerate()
	{
		if (scrolling % acceleration == 0)
			if (segment > segmentation.min)
			{
				segment   >>= 1;
				scrolling >>= 1;
			}
	}

	void Barrel::spin()
	{
		if (segment == segmentation.max)
			return;

		scrolling++;
		if (scrolling == segment)
		{
			scrolling = 0;

			if (current)
				current--;
			else
				current = length - 1;
		}
	}

	void Barrel::decelerate()
	{
		if (scrolling == 0)
			if (segment < segmentation.max)
				segment <<= 1;
	}

	bool Barrel::stopped() const
	{
		return scrolling == 0 && segment == segmentation.max;
	}

	bool Barrel::accelerated() const
	{
		return segment == segmentation.min;
	}

	auto Barrel::symbol() const -> size_t
	{
		const auto& [id, texture] = symbols[current];
		return id;
	}

	template <typename _Func, util::require<std::is_invocable_v<_Func, graphics::Texture&>>>
	void Barrel::apply(_Func _func)
	{
		for (size_t i = 0; i < viewable; i++)
			_func(symbols[index(i)].second);
	}

	template <typename _Func, util::require<std::is_invocable_v<_Func, const graphics::Texture&>>>
	void Barrel::apply(_Func _func) const
	{
		for (size_t i = 0; i < viewable; i++)
			_func(symbols[index(i)].second);
	}

	template <typename _Func, util::require<std::is_invocable_v<_Func, graphics::Texture&, size_t>>>
	void Barrel::apply(_Func _func)
	{
		for (size_t i = 0; i < viewable; i++)
			_func(symbols[index(i)].second, i);
	}

	template <typename _Func, util::require<std::is_invocable_v<_Func, const graphics::Texture&, size_t>>>
	void Barrel::apply(_Func _func) const
	{
		for (size_t i = 0; i < viewable; i++)
			_func(symbols[index(i)].second, i);
	}

	// -----------------------------------------

	void Button::color(sdl::Color _color)
	{
		for (auto& texture : textures)
			texture.color = _color;
	}

	void Button::init(const graphics::TexturePool& _texture_pool)
	{
		for (size_t i = 0; i < type_count; i++)
			textures[i].ptr = _texture_pool[util::get(env::buttons, i)];
	}

	void Button::draw(sdl::Renderer* _renderer) const
	{
		textures[type].draw(_renderer);
	}

	void Button::update()
	{
		textures[type].destination = *this;
	}

	void Button::set(size_t _type)
	{
		type = _type % type_count;
	}

	void Button::reset()
	{
		release();
		deactivate();
	}

	void Button::press()
	{
		pressed_ = true;
	}

	void Button::release()
	{
		pressed_ = false;

	}

	bool Button::pressed() const
	{
		return pressed_;
	}

	void Button::activate()
	{
		active_ = true;
		color(sdl::env::white);
	}

	void Button::deactivate()
	{
		pressed_ = false;
		color(gray);
	}

	bool Button::active() const
	{
		return active_;
	}

	// -----------------------------------------

	void Reward::insert(sdl::Texture* _texture)
	{
		string.emplace(
			string.begin(),
			graphics::Texture{
				/*.destination =*/ *this,
				/*.ptr         =*/ _texture,
			}
		);
	}

	void Reward::init(const graphics::TexturePool& _texture_pool)
	{
		for (size_t i = 0; i < alphabet_size; i++)
			alphabet[i] = _texture_pool[util::get(env::symbols, i)];
	}

	void Reward::draw(sdl::Renderer* _renderer) const
	{
		for (const auto& texture : string)
			texture.draw(_renderer);
	}

	void Reward::update()
	{
		string.clear();

		for (size_t remaining = value; remaining > 0; remaining /= digits_count)
			if (sdl::Texture* texture = alphabet[remaining % digits_count])
				insert(texture);

		if (value == 0)
			insert(alphabet[0]);

		insert(alphabet.back());

		for (size_t i = 0; i < string.size(); i++)
			string[i].destination.position.x += size.x * i - size.x * length();
	}

	auto Reward::length() const -> size_t
	{
		return string.size();
	}
}