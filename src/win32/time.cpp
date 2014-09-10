/*
Copyright 2007-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl/time.hpp"

#include <windows.h>

namespace jngl {
	struct Query {
		__int64 frequency_;
		__int64 start_;
		Query() {
			QueryPerformanceFrequency((LARGE_INTEGER *) &frequency_);
			QueryPerformanceCounter((LARGE_INTEGER *) &start_);
		}
	};

	double getTime() {
		static Query timer;
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER *) &time);
		return static_cast<double>(time - timer.start_) / static_cast<double>(timer.frequency_);
	}

	void sleep(int milliseconds) {
		HANDLE handle = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (handle) {
			WaitForSingleObject(handle, milliseconds);
			CloseHandle(handle);
		} else {
			Sleep(milliseconds); // Use WinAPI's Sleep function
		}
	}
}
