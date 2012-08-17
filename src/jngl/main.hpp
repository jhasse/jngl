#pragma once

#include "work.hpp"

namespace jngl {
	template<class T>
	class MainCreator {
	public:
		MainCreator() {
			auto t = new T;
			jngl::setWork(t);
		}
	};

	template<class T>
	class Main : public Work {
	public:
		virtual void step() override = 0;
		virtual void draw() const override = 0;
	private:
		static MainCreator<T> _;
	};
}