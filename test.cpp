#include <jngl.hpp>
#include <cmath>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

void DrawBackground();
void DrawMouse();
void DrawTess();
int performance = 1;

double abs(double v)
{
	return v < 0 ? -v : v;
}

int main()
{
	try
	{
		std::cout << "Size of jngl.png: " << jngl::GetWidth("jngl.png")
		          << "x" << jngl::GetHeight("jngl.png") << std::endl;
		jngl::SetBackgroundColor(255, 255, 255);
		jngl::ShowWindow("JNGL Test Application", 800, 600);
		jngl::SetMouseVisible(false);
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
			DrawBackground();
			DrawTess();
			jngl::SetColor(0,0,0,255);
			jngl::PushMatrix();
			jngl::Translate(650, 450);
			jngl::Rotate(rotate);
			jngl::DrawLine(-50, -50, 50, 50);
			jngl::PopMatrix();
			jngl::Translate(jngl::GetWindowWidth() / 2, jngl::GetWindowHeight() / 2);
			jngl::Rotate(rotate);
			rotate += 0.1;
			if(rotate > 360)
			{
				rotate = 0;
			}
			double factor = sin(rotate / 360 * M_PI);
			jngl::SetColor(255, 255, 255, static_cast<unsigned char>(abs(factor * 255)));
			jngl::DrawScaled("jngl.png",
							 -jngl::GetWidth("jngl.png")  * factor,
							 -jngl::GetHeight("jngl.png") * factor,
							 factor * 2);
			jngl::SetColor(0, 0, 0);
			jngl::DrawRect(-125, 100, 250, 28);
			jngl::SetFontColor(255, 255, 255);
			jngl::Print("White text on black background", -115, 105);
			jngl::SetFontColor(255, 255, 255);
			jngl::SetFontSize(20);
			jngl::Print("White text without background", -115, 135);
			jngl::SetFontSize(12);
			jngl::Reset();
			std::stringstream sstream;
			sstream << "FPS: " << jngl::FPS() << "\nFactor: " << factor << "\nSize of double: " << sizeof(double);
			jngl::SetColor(0, 0, 0);
			jngl::DrawRect(0, 0, 200, 62);
			jngl::SetFontColor(static_cast<unsigned char>(255 * (1 - factor)), static_cast<unsigned char>(255 * factor), 255);
			jngl::SetFontByName("Courier New");
			jngl::Print(sstream.str(), 5, 5);
			jngl::SetFontByName("sans-serif");
			jngl::SetFontColor(0,0,0);
			jngl::SetFontByName("Times New Roman");
			jngl::Print("Black text on white background", 5, 75);
			jngl::SetFontByName("Arial");
			jngl::SetFontSize(20);
			jngl::Print("UTF-8:   ä ö ü ß Ĉ Ψ ≈", 5, 105);
			jngl::Print(" $", jngl::GetTextWidth("UTF-8:   ä ö ü ß Ĉ Ψ ≈") + 5, 105);
			jngl::SetFontSize(12);
			jngl::Print("Press 1-9 to test the performance\nPress E to show a error box.", 5, 135);
			if(jngl::KeyPressed('e'))
			{
				jngl::ErrorMessage("Hello World!");
			}
			jngl::Print("Press A to toggle Anti-Aliasing.", 5, 530);
			if(jngl::KeyPressed('a'))
			{
				jngl::SetAntiAliasing(!jngl::GetAntiAliasing());
			}
			jngl::Print("Press F11 to switch fullscreen mode.", 5, 550);
			if(jngl::KeyPressed(jngl::key::F11))
			{
				jngl::ShowWindow("JNGL Test Application", 800, 600, !jngl::GetFullscreen());
			}
			jngl::SetColor(0,0,255,128);
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
	jngl::SetColor(255, 255, 255, 100);
	if(performance > 1)
	{
		for(int x = 0; x < performance; ++x)
		{
			for(int y = 0; y < performance; ++y)
			{
				jngl::DrawScaled("jngl.png",
								 x * jngl::GetWindowWidth() / performance,
								 y * jngl::GetWindowHeight() / performance,
								 (double)jngl::GetWindowWidth() / performance / jngl::GetWidth("jngl.png"),
								 (double)jngl::GetWindowHeight() / performance / jngl::GetHeight("jngl.png"));
			}
		}
	}
	else
	{
		jngl::Draw("jngl.png",
		           jngl::GetWindowWidth() / 2- jngl::GetWidth("jngl.png") / 2,
		           jngl::GetWindowHeight() / 2- jngl::GetHeight("jngl.png") / 2);
	}
	jngl::SetColor(255, 0, 0, 100);
	jngl::DrawRect(600, 30, 100, 100);
	jngl::SetColor(0, 255, 0, 100);
	jngl::DrawRect(600, 400, 100, 100);
	jngl::SetColor(0, 0, 255, 100);
	jngl::DrawEllipse(80, 450, 50, 80);
}

void DrawMouse()
{
	unsigned char red, green, blue;
	jngl::ReadPixel(jngl::GetMouseX(), jngl::GetMouseY(), red, green, blue);
	std::stringstream sstream;
	sstream << "R: " << static_cast<int>(red)
	      << "\nG: " << static_cast<int>(green)
	      << "\nB: " << static_cast<int>(blue);
	jngl::SetFontSize(8);
	jngl::SetFontColor(0, 255, 0, 200);
	jngl::Print(sstream.str(), jngl::GetMouseX() + 30, jngl::GetMouseY() + 10);
	jngl::Translate(jngl::GetMouseX(), jngl::GetMouseY());
	jngl::Rotate(-45);
	jngl::SetFontSize(30);
	jngl::SetFontColor(10, 10, 200);
	jngl::Print("↑", -8, -2);
	jngl::SetFontSize(12);
	jngl::Reset();
}
void DrawTess()
{
	jngl::SetColor(3, 10, 89, 50);
	jngl::BeginPolygon();
	jngl::Vertex(80, 590);
	jngl::Vertex(130, 490);
	jngl::Vertex(180, 390);
	jngl::Vertex(230, 390);
	jngl::Vertex(280, 490);
	jngl::Vertex(330, 490);
	jngl::Vertex(380, 390);
	jngl::Vertex(430, 490);
	jngl::Vertex(480, 490);
	jngl::Vertex(530, 590);
	jngl::EndPolygon();
	jngl::SetFontColor(255, 0, 0);
	jngl::Print("A", 80, 590);
	jngl::Print("B", 130, 490);
	jngl::Print("C", 180, 390);
	jngl::Print("D", 230, 390);
	jngl::Print("E", 280, 490);
	jngl::Print("F", 330, 490);
	jngl::Print("G", 380, 390);
	jngl::Print("H", 430, 490);
	jngl::Print("I", 480, 490);
	jngl::Print("J", 530, 590);
}
