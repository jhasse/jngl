#import <UIKit/UIKit.h>

#import "JNGLView.h"

@interface SceneDelegate : UIResponder <UIWindowSceneDelegate> {
	JNGLView* view;
}

@property(strong, nonatomic) UIWindow* window;

@end
