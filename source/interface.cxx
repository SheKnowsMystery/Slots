#include "bindings.h"
#include "graphics.h"
#include "elements.h"
#include "interface.h"

namespace slots
{
	void Interface::init(const graphics::TexturePool& _texture_pool)
	{
		barrels.init(_texture_pool);
		start.init(_texture_pool);
		start.set(true);
		stop.init(_texture_pool);
		stop.set(false);
		reward.init(_texture_pool);
	}

	void Interface::place()
	{
		(graphics::Rect&)(barrels) = parameters.barrel;
		(graphics::Rect&)(start)   = parameters.start;
		(graphics::Rect&)(stop)    = parameters.stop;
		(graphics::Rect&)(reward)  = parameters.reward;
	}

	void Interface::layout(int _x, int _y)
	{
		float unit = _x * .1F;

		float barrels_width  = barrels_count * unit;
		float barrels_height = Barrel::strip * unit;

		float height = (_y - barrels_height - unit) / 2;
		float width  = (_x - unit * 2 - barrels_width) / 3;

		float buttons_pos_x = width + barrels_width + width;

		float reward_pos_x = width + barrels_width;

		parameters.barrel = {
			/*.size     =*/ {/*.x =*/ unit,  /*.y =*/ unit  },
			/*.position =*/ {/*.x =*/ width, /*.y =*/ height},
		};
		parameters.start = {
			/*.size     =*/ {/*.x =*/ unit * 2,      /*.y =*/ unit  },
			/*.position =*/ {/*.x =*/ buttons_pos_x, /*.y =*/ height},
		};
		parameters.stop = {
			/*.size     =*/ {/*.x =*/ unit * 2,      /*.y =*/ unit               },
			/*.position =*/ {/*.x =*/ buttons_pos_x, /*.y =*/ height + (unit * 2)},
		};
		parameters.reward = {
			/*.size     =*/ {/*.x =*/ unit / 2,     /*.y =*/ unit / 2                            },
			/*.position =*/ {/*.x =*/ reward_pos_x, /*.y =*/ height + barrels_height + (unit / 2)},
		};
	}

	void Interface::layout(sdl::Point _size)
	{
		layout(_size.x, _size.y);
	}

	void Interface::scale(sdl::FPoint _scale)
	{
		barrels.scaling = _scale;
		start.scaling   = _scale;
		stop.scaling    = _scale;
		reward.scaling  = _scale;
	}
}