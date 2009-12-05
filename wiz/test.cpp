#include <jngl.hpp>
#include <iostream>
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <wizGLES.h>
#include <boost/lexical_cast.hpp>

int main()
{
	jngl::ShowWindow("", 320, 240);
	jngl::SetFont("Arial.ttf");

	int counter = 0;

	while(jngl::Running())
	{
		jngl::SetBackgroundColor(++counter % 256, 100, 100);
		jngl::Draw("test.png", 50, 10);
		jngl::SetColor(255, 255, 255);
		jngl::DrawLine(10, 10, 400, 400);
		jngl::SetFontColor(255, 255, 255);
		jngl::Print("test", 10, 200);
		jngl::Print(boost::lexical_cast<std::string>(jngl::Time()), 100, 200);
		if(jngl::KeyDown(jngl::key::WizMenu))
		{
			jngl::Quit();
		}
		jngl::SwapBuffers();
	}
}
