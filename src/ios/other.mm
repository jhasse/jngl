// Copyright 2020-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"
#include "../jngl/Controller.hpp"

#include <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace jngl {

std::string getBinaryPath() {
	return getPrefix(); // better than nothing
}

std::string getPreferredLanguage() {
	return [[[NSLocale preferredLanguages] firstObject] UTF8String];
}

void openURL(const std::string& url) {
	[[UIApplication sharedApplication]
	    openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
}

} // namespace jngl
