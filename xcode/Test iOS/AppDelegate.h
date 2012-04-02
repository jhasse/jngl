#import <UIKit/UIKit.h>

#import "JNGLView.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate> {
	JNGLView* view;
	UIViewController* viewController;
}

@property (strong, nonatomic) UIWindow *window;

@end
