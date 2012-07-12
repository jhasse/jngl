/*
Copyright 2007 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib.h>
#include <unistd.h>

namespace jngl
{
	struct Timer
	{
		Timer() : gtimer_(g_timer_new()) {}
		~Timer() { g_timer_destroy(gtimer_); }
		GTimer* gtimer_;
	};
	double time()
	{
		static Timer timer;
		return g_timer_elapsed(timer.gtimer_, NULL);
	}
	void sleep(int milliseconds)
	{
		usleep(milliseconds * 1000);
	}
}

