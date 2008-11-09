#include <jngl.hpp>
#include <cmath>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

void DrawBackground();
void DrawMouse();
int performance = 1;

double abs(double v)
{
	return v < 0 ? -v : v;
}

int main()
{
	try
	{
		jngl::ShowWindow("JNGL Test Application", 800, 600);
		jngl::MouseVisible(false);
//		jngl::SetFont("Arial.ttf");
		double rotate = 0.0;
		while(jngl::Running())
		{
			jngl::BeginDraw();
			for(int i = 0; i < 10; ++i)
			{
				if(jngl::KeyDown(boost::lexical_cast<char>(i)))
				{
					performance = i == 0 ? 10 : i;
				}
			}
//			jngl::Scale(40, 40);
			jngl::Print("U", 0, 0);
			DrawBackground();
			jngl::Color(0,0,0,255);
			jngl::PushMatrix();
			jngl::Translate(650, 450);
			jngl::Rotate(rotate);
			jngl::DrawLine(-50, -50, 50, 50);
			jngl::PopMatrix();
			jngl::Translate(jngl::ScaleWidth() / 2, jngl::ScaleHeight() / 2);
			jngl::Rotate(rotate);
			rotate += 0.1;
			if(rotate > 360)
			{
				rotate = 0;
			}
			double factor = sin(rotate / 360 * M_PI);
			jngl::Color(255, 255, 255, static_cast<unsigned char>(abs(factor * 255)));
			jngl::DrawScaled("jngl.png",
							 -jngl::Width("jngl.png")  * factor,
							 -jngl::Height("jngl.png") * factor,
							 factor * 2);
			jngl::Color(0, 0, 0);
			jngl::DrawRect(-125, 100, 250, 28);
			jngl::FontColor(255, 255, 255);
			jngl::Print("White text on black background", -115, 105);
			jngl::FontColor(255, 255, 255);
			jngl::FontSize(20);
			jngl::Print("White text on white background", -115, 135);
			jngl::FontSize(12);
			jngl::Reset();
			std::stringstream sstream;
			sstream << "FPS: " << jngl::FPS() << "\nFactor: " << factor << "\nSize of double: " << sizeof(double);
			jngl::Color(0, 0, 0);
			jngl::DrawRect(0, 0, 200, 62);
			jngl::FontColor(static_cast<unsigned char>(255 * (1 - factor)), static_cast<unsigned char>(255 * factor), 255);
			jngl::Print(sstream.str(), 5, 5);
			jngl::FontColor(0,0,0);
			jngl::Print("Black text on white background", 5, 75);
			jngl::FontSize(20);
			jngl::Print("UTF-8:   ä ö ü ß Ĉ Ψ ≈", 5, 105);
			jngl::FontSize(12);
			jngl::Print("Press 1-9 to test the performance", 5, 135);
			jngl::Color(0,0,255,128);
			jngl::BeginPolygon();
			jngl::Vertex(60, 590);
			jngl::Vertex(110, 490);
			jngl::Vertex(160, 390);
			jngl::Vertex(210, 390);
			jngl::Vertex(260, 490);
			jngl::Vertex(310, 490);
			jngl::Vertex(360, 390);
			jngl::Vertex(410, 490);
			jngl::Vertex(460, 490);
			jngl::Vertex(510, 590);
			jngl::EndPolygon();
			jngl::FontColor(255, 0, 0);
			jngl::Print("A", 60, 590);
			jngl::Print("B", 110, 490);
			jngl::Print("C", 160, 390);
			jngl::Print("D", 210, 390);
			jngl::Print("E", 260, 490);
			jngl::Print("F", 310, 490);
			jngl::Print("G", 360, 390);
			jngl::Print("H", 410, 490);
			jngl::Print("I", 460, 490);
			jngl::Print("J", 510, 590);
			DrawMouse();
			jngl::EndDraw();
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void DrawBackground()
{
	jngl::Color(255, 255, 255, 100);
	for(int x = 0; x < performance; ++x)
	{
		for(int y = 0; y < performance; ++y)
		{
			jngl::DrawScaled("jngl.png",
							 x * jngl::ScaleWidth() / performance,
							 y * jngl::ScaleHeight() / performance,
							 (double)jngl::ScaleWidth() / performance / jngl::Width("jngl.png"),
							 (double)jngl::ScaleHeight() / performance / jngl::Height("jngl.png"));
		}
	}
	jngl::Color(255, 0, 0, 100);
	jngl::DrawRect(600, 30, 100, 100);
	jngl::Color(0, 255, 0, 100);
	jngl::DrawRect(600, 400, 100, 100);
	jngl::Color(0, 0, 255, 100);
	jngl::DrawRect(30, 400, 100, 100);
}

void DrawMouse()
{
	jngl::Translate(jngl::MouseX(), jngl::MouseY());
	jngl::Rotate(-45);
	jngl::FontSize(30);
	jngl::FontColor(10, 10, 200);	jngl::Print("↑", -11, -9);
	jngl::FontSize(12);
	jngl::Reset();
}
