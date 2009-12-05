/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

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

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define TIMER_BASE3 0x1980
#define TIMER_REG(x) memregl[(TIMER_BASE3 + x) >> 2]

namespace jngl
{
	struct Timer
	{
		Timer()
		{
			int memdev = open("/dev/mem", O_RDWR);
			memregl = reinterpret_cast<volatile unsigned int*>(mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, memdev, 0xc0000000));
		    TIMER_REG(0x44) = 0x922;
        	TIMER_REG(0x40) = 0x0c;
	        TIMER_REG(0x08) = 0x6b;
		}
		~Timer()
		{
		    TIMER_REG(0x40) = 0x0c;
		    TIMER_REG(0x08) = 0x23;
		    TIMER_REG(0x00) = 0;
		    TIMER_REG(0x40) = 0;
		    TIMER_REG(0x44) = 0;
		}
		double GetSeconds() const
		{
		    TIMER_REG(0x08) = 0x4b;
		    return double(TIMER_REG(0)) / 1000000.0;
		}
		volatile unsigned int *memregl;
	};
	double Time()
	{
		static Timer timer;
		return timer.GetSeconds();
	}
	void Sleep(int milliseconds)
	{
		usleep(milliseconds * 1000);
	}
}

