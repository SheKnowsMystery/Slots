#include "bindings.h"
#include "utility.h"
#include "graphics.h"
#include "elements.h"
#include "states.h"
#include "lists.h"

#include <algorithm>
#include <iterator>

namespace slots::env
{
	auto operator++(state& _state) -> state
	{
		return _state = static_cast<state>((static_cast<state_t>(_state) + 1) % static_cast<state_t>(state::last));
	}

	auto operator++(state&& _state) -> state
	{
		return _state = static_cast<state>((static_cast<state_t>(_state) + 1) % static_cast<state_t>(state::last));
	}

	auto operator++(state& _state, int) -> state
	{
		state temp = _state;
		++_state;
		return temp;
	}
}

namespace slots
{

	void State::scale(Scale _data)
	{
		auto [interface, frame] = _data;
		interface.scale(frame.scaling());
	}

	// -----------------------------------------

	void Wait::begin(Begin _data)
	{
		auto [interface] = _data;
		interface.reward.value = 0;
		interface.start.reset();
		interface.stop.reset();
		interface.start.activate();
		updated = 0;
	}

	void Wait::handle(Handle _data)
	{
		auto [interface, event] = _data;
		sdl::FPoint mouse;
		switch (event.type)
		{
		case sdl::EventType::SDL_MOUSEBUTTONDOWN:
			mouse = slots::graphics::motion(event);
			if (interface.start.contains(mouse))
				interface.start.press();
			break;
		default:
			break;
		}
	}

	void Wait::update(Update _data)
	{
		auto [interface, frame] = _data;
		frame.update(interface.barrels, interface.start, interface.stop);
		updated++;
	}

	void Wait::draw(Draw _data)
	{
		auto [interface, frame] = _data;
		frame.draw(interface.barrels, interface.start, interface.stop);
	}

	bool Wait::end(End _data)
	{
		auto [interface] = _data;
		return interface.start.pressed();
	}

	// -----------------------------------------

	void Accelerate::begin(Begin _data)
	{
		auto [interface] = _data;
		interface.start.reset();
		updated = 0;
	}

	void Accelerate::handle(Handle _data)
	{
		auto [interface, event] = _data;
		interface.start.reset();
		interface.stop.reset();
	}

	void Accelerate::update(Update _data)
	{
		auto [interface, frame] = _data;

		interface.barrels.accelerate();
		interface.barrels.spin();

		frame.update(interface.barrels, interface.start, interface.stop);
		updated++;
	}

	void Accelerate::draw(Draw _data)
	{
		auto [interface, frame] = _data;
		frame.draw(interface.barrels, interface.start, interface.stop);
	}

	bool Accelerate::end(End _data)
	{
		auto [interface] = _data;
		return interface.barrels.accelerated();
	}

	// -----------------------------------------

	void Spin::begin(Begin _data)
	{
		auto [interface] = _data;
		interface.start.reset();
		interface.stop.reset();
		interface.stop.activate();
		updated = 0;
	}

	void Spin::handle(Handle _data)
	{
		auto [interface, event] = _data;
		sdl::FPoint mouse;
		switch (event.type)
		{
		case sdl::EventType::SDL_MOUSEBUTTONDOWN:
			mouse = slots::graphics::motion(event);
			if (interface.stop.contains(mouse))
				interface.stop.press();
			break;
		default:
			break;
		}
	}

	void Spin::update(Update _data)
	{
		auto [interface, frame] = _data;

		interface.barrels.spin();

		if (updated == threshold)
			interface.stop.press();

		frame.update(interface.barrels, interface.start, interface.stop);
		updated++;
	}

	void Spin::draw(Draw _data)
	{
		auto [interface, frame] = _data;
		frame.draw(interface.barrels, interface.start, interface.stop);
	}

	bool Spin::end(End _data)
	{
		auto [interface] = _data;
		return interface.stop.pressed();
	}

	// -----------------------------------------

	void Decelerate::begin(Begin _data)
	{
		auto [interface] = _data;
		interface.start.reset();
		interface.stop.reset();
		updated = 0;
	}

	void Decelerate::handle(Handle _data)
	{
		auto [interface, event] = _data;
	}

	void Decelerate::update(Update _data)
	{
		auto [interface, frame] = _data;

		interface.barrels.decelerate();
		interface.barrels.spin();

		frame.update(interface.barrels, interface.start, interface.stop);
		updated++;
	}

	void Decelerate::draw(Draw _data)
	{
		auto [interface, frame] = _data;
		frame.draw(interface.barrels, interface.start, interface.stop);
	}

	bool Decelerate::end(End _data)
	{
		auto [interface] = _data;
		return interface.barrels.stopped();
	}

	// -----------------------------------------

	void Show::begin(Begin _data)
	{
		auto [interface] = _data;

		size_t count[env::cats.size()] = {};
		for (const auto& barrel : interface.barrels.array)
			count[barrel.symbol()]++;

		auto*  elem = std::max_element(std::begin(count), std::end(count));
		size_t id   = std::distance(std::begin(count), elem);


		interface.reward.value = std::pow(*elem, id) * Reward::multiplier;

		interface.start.reset();
		interface.stop.reset();

		interface.start.activate();

		updated = 0;
	}

	void Show::handle(Handle _data)
	{
		auto [interface, event] = _data;
		sdl::FPoint mouse;
		switch (event.type)
		{
		case sdl::EventType::SDL_MOUSEBUTTONDOWN:
			mouse = slots::graphics::motion(event);
			if (interface.start.contains(mouse))
				interface.start.press();
			break;
		default:
			break;
		}
	}

	void Show::update(Update _data)
	{
		auto [interface, frame] = _data;
		frame.update(interface.barrels, interface.start, interface.stop, interface.reward);
		updated++;
	}

	void Show::draw(Draw _data)
	{
		auto [interface, frame] = _data;
		frame.draw(interface.barrels, interface.start, interface.stop, interface.reward);
	}

	bool Show::end(End _data)
	{
		auto [interface] = _data;
		return interface.start.pressed();
	}

	// -----------------------------------------

		namespace st
		{
			auto state(env::state _type) -> State*
			{
				static auto wait = Wait();
				static auto accelerate = Accelerate();
				static auto spin = Spin();
				static auto decelerate = Decelerate();
				static auto show = Show();

				switch (_type)
				{
				case env::state::wait:
					return &wait;
				case env::state::accelerate:
					return &accelerate;
				case env::state::spin:
					return &spin;
				case env::state::decelerate:
					return &decelerate;
				case env::state::show:
					return &show;
				default:
					return nullptr;
				}
			}
		}

	void StateMachine::insert(env::state _type)
	{
		states[_type] = st::state(_type);
	}

	StateMachine::StateMachine()
	{
		insert(env::state::wait);
		insert(env::state::accelerate);
		insert(env::state::spin);
		insert(env::state::decelerate);
		insert(env::state::show);
	}

	auto StateMachine::current() const -> State*
	{
		if (util::contains(states, state))
			return states.at(state);
		return nullptr;
	}

	void StateMachine::next()
	{
		state++;
	}
}
