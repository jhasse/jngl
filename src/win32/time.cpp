/*
Copyright (C) 2007  Jan Niklas Hasse <jhasse@gmail.com>

This file is part of jngl.

jngl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jngl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>

namespace jngl
{
	struct Query
	{
		__int64 frequency_;
		__int64 start_;
		Query()
		{
			QueryPerformanceFrequency((LARGE_INTEGER *) &frequency_);
			QueryPerformanceCounter((LARGE_INTEGER *) &start_);
		}
	};
	double getTime()
	{
		static Query timer;
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER *) &time);
		return static_cast<double>(time - timer.start_) / static_cast<double>(timer.frequency_);
	}
	void sleep(int milliseconds)
	{
		HANDLE handle = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(handle)
		{
			WaitForSingleObject(handle, milliseconds);
			CloseHandle(handle);
		}
		else
		{
			::Sleep(milliseconds); // Use WinAPI's Sleep function
		}
	}
}

