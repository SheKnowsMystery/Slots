#pragma once

#ifndef STATES_H
#define STATES_H

#include "bindings.h"
#include "graphics.h"
#include "interface.h"

#include <type_traits>

namespace slots::env
{
	enum class state : std::uint8_t
	{
		wait, accelerate, spin, decelerate, show,
		last
	};

	using state_t = std::underlying_type_t<state>;

	auto operator++(state& _state) -> state;
	auto operator++(state&& _state) -> state;
	auto operator++(state& _state, int) -> state;
}

namespace slots
{
	struct State
	{
		struct Begin {
			Interface& interface;
		};

		struct Handle {
			Interface& interface;
			const sdl::Event& event;
		};

		struct Scale {
			Interface& interface;
			const graphics::Frame& frame;
		};

		struct Update {
			Interface& interface;
			const graphics::Frame& frame;
		};

		struct Draw {
			const Interface& interface;
			const graphics::Frame& frame;
		};

		struct End {
			const Interface& interface;
		};

		size_t updated = 0;
		const env::state type;
		State(env::state _type) : type(_type) {}

		virtual void begin(Begin _data) = 0;
		virtual void handle(Handle _data) = 0;
		virtual void scale(Scale _data);
		virtual void update(Update _data) = 0;
		virtual void draw(Draw _data) = 0;
		virtual bool end(End _data) = 0;
	};

	struct Wait : State
	{
		Wait() : State(env::state::wait) {}

		void begin(Begin _data) override;
		void handle(Handle _data) override;
		void update(Update _data) override;
		void draw(Draw _data) override;
		bool end(End _data) override;
	};

	struct Accelerate : State
	{
		Accelerate() : State(env::state::spin) {}

		void begin(Begin _data) override;
		void handle(Handle _data) override;
		void update(Update _data) override;
		void draw(Draw _data) override;
		bool end(End _data) override;
	};

	struct Spin : State
	{
		Spin() : State(env::state::spin) {}

		static constexpr size_t threshold = 5 * 60;

		void begin(Begin _data) override;
		void handle(Handle _data) override;
		void update(Update _data) override;
		void draw(Draw _data) override;
		bool end(End _data) override;
	};

	struct Decelerate : State
	{
		Decelerate() : State(env::state::spin) {}

		void begin(Begin _data) override;
		void handle(Handle _data) override;
		void update(Update _data) override;
		void draw(Draw _data) override;
		bool end(End _data) override;
	};

	struct Show : State
	{
		Show() : State(env::state::show) {}

		void begin(Begin _data) override;
		void handle(Handle _data) override;
		void update(Update _data) override;
		void draw(Draw _data) override;
		bool end(End _data) override;
	};

	class StateMachine
	{
		env::state state = env::state::wait;

		std::map<env::state, State*> states;

		void insert(env::state _type);

	public:
		StateMachine();

		auto current() const -> State*;
		void next();
	};
}

#endif