#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H

#include "bindings.h"
#include "graphics.h"
#include "elements.h"

namespace slots
{
	struct Interface
	{
		static constexpr size_t barrels_count = 5;

		struct {
			graphics::Rect barrel;
			graphics::Rect start;
			graphics::Rect stop;
			graphics::Rect reward;
		} parameters;

		Barrels<barrels_count> barrels;

		Button start;
		Button stop;

		Reward reward;

		void init(const graphics::TexturePool& _texture_pool);
		void place();

		void layout(int _x, int _y);
		void layout(sdl::Point _size);

		void scale(sdl::FPoint _scale);
	};
}

#endif