#include <jngl.hpp>
#include <sstream>

#include "constants.hpp"
#include "vector2d.hpp"
#include "bike.hpp"
#include "base.hpp"

#include <iostream>

int main()
{
	jngl::ShowWindow("Bike", screenWidth, screenHeight);
	Base base;
	double oldTime = jngl::Time();
	bool needDraw = true;
	while(jngl::Running())
	{
		if(jngl::Time() - oldTime > timePerFrame)
		{
			// This stuff needs to be done 100 times per second
			oldTime += timePerFrame;
			needDraw = true;
			base.DoFrame();
		}
		else
		{
			if(needDraw)
			{
				needDraw = false;
				// This needs to be done when "needDraw" is true
				jngl::BeginDraw();
				base.Draw();
				std::stringstream sstream;
				sstream << "FPS: " << static_cast<int>(jngl::FPS());				
				jngl::Print(sstream.str(), 10, 10);
				jngl::EndDraw();
			}
			else
			{
				// Nothing to do? Okay let's Sleep.
				jngl::Sleep(10);
			}
		}
	}
}
