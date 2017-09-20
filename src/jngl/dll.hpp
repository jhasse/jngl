/*
Copyright 2014-2017 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#ifndef JNGLDLL_API
	#ifdef _MSC_VER
		#ifdef JNGLDLL_EXPORTS
			#define JNGLDLL_API __declspec(dllexport)
		#else
			#define JNGLDLL_API __declspec(dllimport)
		#endif
	#else
		#define JNGLDLL_API
	#endif
#endif
