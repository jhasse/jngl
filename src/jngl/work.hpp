#pragma once

#include <boost/shared_ptr.hpp>

namespace jngl {
	class Work {
	public:
		virtual void step() = 0;
		virtual void draw() const = 0;
		virtual void onQuitEvent();
		virtual void onLoad();
		virtual ~Work();
	};

	boost::shared_ptr<Work> getWork();

	void setWork(boost::shared_ptr<Work> work);

	void setWork(Work*);
}