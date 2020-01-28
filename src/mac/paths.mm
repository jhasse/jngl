// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../paths.hpp"

#import <Foundation/NSFileManager.h>

std::string jngl::getSystemConfigPath() {
	NSArray* const paths =
	    NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	return [[paths firstObject] UTF8String];
}
