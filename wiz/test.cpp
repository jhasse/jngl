#include "../jngl.hpp"

#include <boost/lexical_cast.hpp>

int main()
{
	jngl::ShowWindow("", 320, 240);
	while(jngl::Running())
	{
		jngl::Draw("test.png", 10, 10);
		jngl::Print(std::string("FPS: ") + boost::lexical_cast<std::string>(jngl::FPS()), 10, 200);
		jngl::Print("Press A to play test.ogg", 10, 220);
		jngl::SwapBuffers();
		if(jngl::KeyPressed(jngl::key::WizMenu))
		{
			jngl::Quit();
		}
		if(jngl::KeyPressed(jngl::key::WizA))
		{
			jngl::Play("test.ogg");
		}
	}
}
