// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"

#import <CoreText/CoreText.h>

#import <Foundation/Foundation.h>

namespace jngl {

std::string Window::GetFontFileByName(const std::string& fontname) {
	CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize(
	    CFStringCreateWithCString(kCFAllocatorDefault, fontname.c_str(), kCFStringEncodingUTF8),
	    12);
	CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute);
	if (!url) {
		if (fontname == "sans-serif") {
			return GetFontFileByName("Arial");
		}
		throw std::runtime_error(std::string("Couldn't load " + fontname));
	}
	NSString* fontPath = [NSString stringWithString:[(NSURL*)CFBridgingRelease(url) path]];
	return [fontPath UTF8String];
}

} // namespace jngl
