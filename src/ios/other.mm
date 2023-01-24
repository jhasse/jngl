// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include "../jngl/message.hpp"
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

void errorMessage(const std::string& text) {
	printMessage(text); // also print the message in case the message box won't show (it doesn't if
	                    // called to early).
	UIAlertController* alertController =
	    [UIAlertController alertControllerWithTitle:@"Error"
	                                        message:[NSString stringWithUTF8String:text.c_str()]
	                                 preferredStyle:UIAlertControllerStyleAlert];
	UIAlertAction* actionOk = [UIAlertAction actionWithTitle:@"Ok"
	                                                   style:UIAlertActionStyleDefault
	                                                 handler:nil];
	[alertController addAction:actionOk];
	[[UIApplication sharedApplication].delegate.window.rootViewController
	    presentViewController:alertController
	                 animated:YES
	               completion:nil];
}

void setVerticalSync(bool) {
}

bool getVerticalSync() {
	return true;
}

} // namespace jngl
