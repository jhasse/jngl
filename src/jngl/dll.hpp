// Copyright 2014-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#ifndef JNGLDLL_API
	#ifdef JNGLDLL_EXPORTS
		#define JNGLDLL_API __declspec(dllexport)
	#else
		#define JNGLDLL_API
	#endif
#endif
