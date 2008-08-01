#include <jngl.hpp>
#include <sstream>

int main()
{    
    jngl::ShowWindow("Font Test", 800, 600);
    while(jngl::Running())
    {
        jngl::BeginDraw();
       	int move = 0;
       	double fps = jngl::FPS();
        for(int i = 5; i < 27; ++i)
        {
	        jngl::FontSize(i);
        	jngl::Color(0, 0, 0);
	        std::stringstream sstream;
	        sstream << "Hello world! ßäöü - " << i << " - FPS: " << fps;
	        jngl::Print(sstream.str(), 10, 10 + move);
	        move += 1.5 * i;
        }
        jngl::Color(255, 0, 0);
        jngl::Print("Red", 10, 530);
        jngl::Color(0, 255, 0);
        jngl::Print("Green", 160 , 530);
        jngl::Color(0, 0, 255);
        jngl::Print("Blue", 310 , 530);
        jngl::EndDraw();
    }
}
