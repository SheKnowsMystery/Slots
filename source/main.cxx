
#include "bindings.h"
#include "graphics.h"
#include "interface.h"
#include "states.h"
#include "lists.h"

#include <SDL2/SDL_main.h>

#include <chrono>
#include <thread>

void loop(const slots::graphics::Frame& _frame, const slots::graphics::TexturePool& _texture_pool)
{
	struct {
		using point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using unit_t  = std::chrono::nanoseconds;

		const size_t fps = 60;
		const unit_t duration = std::chrono::duration_cast<unit_t>(std::chrono::seconds(1)) / 60;

		point_t start;
		point_t end;
		unit_t  delta;
	} time;

	sdl::Event          event;
	slots::Interface    interface;
	slots::StateMachine state_machine;

	interface.init(_texture_pool);
	interface.layout(_frame.size);
	interface.place();

	state_machine.current()->begin(
		{
			/*.interface =*/ interface,
		}
	);

	for (bool running = true; running;)
	{
		time.start = std::chrono::high_resolution_clock::now();

		while (SDL_PollEvent(&event))
			switch (event.type)
			{
			case sdl::EventType::SDL_QUIT:
				running = false;
				break;
			case sdl::EventType::SDL_WINDOWEVENT:
				if (event.window.event == sdl::win::event::RESIZED)
					state_machine.current()->scale(
						{
							/*.interface =*/ interface,
							/*.frame     =*/ _frame,
						}
					);
				break;
			default:
				state_machine.current()->handle(
					{
						/*.interface =*/ interface,
						/*.event     =*/ event,
					}
				);
				break;
			}

		state_machine.current()->update(
			{
				/*.interface =*/ interface,
				/*.frame     =*/ _frame,
			}
		);

		_frame.clear(sdl::env::black);

		state_machine.current()->draw(
			{
				/*.interface =*/ interface,
				/*.frame     =*/ _frame,
			}
		);

		_frame.present();

		bool next = state_machine.current()->end(
			{
				/*.interface =*/ interface,
			}
		);

		if (next)
		{
			state_machine.next();
			state_machine.current()->begin(
				{
					/*.interface =*/ interface,
				}
			);
		}
		time.end   = std::chrono::high_resolution_clock::now();
		time.delta = time.end - time.start;

		std::this_thread::sleep_for(
			std::max(
				time.duration - time.delta,
				std::chrono::nanoseconds::zero()
			)
		);
	}
}

namespace tex
{
	auto pair(std::string_view _identifier) -> slots::graphics::type::textures::value_type
	{
		using std::operator""s;
		return {_identifier.data(), "../assets/"s + _identifier.data() + ".png"};
	}
}

int main(int _argc, char** _argv)
{
	auto window_data = slots::graphics::WindowData{
		/*.title =*/ "Slots",
		/*.rect  =*/ {/*.x =*/ 200, /*.y =*/ 200, /*.w =*/ 1000, /*.h =*/ 600},
		/*.flags =*/ {
			/*.sdl       =*/ sdl::init::VIDEO,
			/*.img       =*/ img::init::PNG,
			/*.window    =*/ sdl::win::init::RESIZABLE,
			/*.renderer  =*/ sdl::renderer::ACCELERATED,
		},
	};

	auto textures = slots::graphics::type::textures();

	for (const auto& name : slots::env::cats)
		textures.insert(tex::pair(name));

	for (const auto& name : slots::env::symbols)
		textures.insert(tex::pair(name));

	for (const auto& name : slots::env::buttons)
		textures.insert(tex::pair(name));

	slots::graphics::context(window_data, textures, loop);

	return 0;
}