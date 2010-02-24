#include <jngl.hpp>
#include <cmath>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

void DrawBackground();
void DrawMouse();
void DrawTess();
void TestKeys();
int performance = 1;

double absolute(double v)
{
	return v < 0 ? -v : v;
}

int main()
{
	try
	{
		std::cout << "Size of jngl.png: " << jngl::GetWidth("jngl.png")
		          << "x" << jngl::GetHeight("jngl.png") << std::endl;
		jngl::ShowWindow("JNGL Test Application", 800, 600);
		jngl::SetBackgroundColor(255, 255, 255);
		jngl::SetMouseVisible(false);
		double rotate = 0.0;
		int frameNumber = 0;
		double frameTime = jngl::Time();
		double lastTime = jngl::Time();
		while(jngl::Running())
		{
			double timeSinceLastFrame = jngl::Time() - lastTime;
			lastTime = jngl::Time();
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
			rotate += 90 * timeSinceLastFrame; // 90 degree per second
			if(rotate > 360)
			{
				rotate = 0;
			}
			double factor = sin(rotate / 360 * M_PI);
			jngl::SetSpriteColor(255, 255, 255, static_cast<unsigned char>(absolute(factor * 255)));
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
			jngl::Print(" $", static_cast<int>(jngl::GetTextWidth("UTF-8:   ä ö ü ß Ĉ Ψ ≈") + 5), 105);
			jngl::SetFontSize(12);
			jngl::Print("Press 1-9 to test the performance\nPress E to show a error box.", 5, 135);
			if(jngl::KeyPressed('e'))
			{
				jngl::ErrorMessage("Hello World!");
			}
			jngl::Print("Press A to toggle Anti-Aliasing.", 5, 510);
			if(jngl::KeyPressed('a'))
			{
				jngl::SetAntiAliasing(!jngl::GetAntiAliasing());
			}
			jngl::Print("Press F11 to switch fullscreen mode.", 5, 530);
			if(jngl::KeyPressed(jngl::key::F11))
			{
				jngl::ShowWindow("JNGL Test Application", 800, 600, !jngl::GetFullscreen());
			}
			jngl::Print("Press K to test key codes.", 5, 550);
			jngl::Print("Press P to play a sound.", 6, 570);
			if(jngl::KeyPressed('p'))
			{
				jngl::Stop("test.ogg");
				jngl::Play("test.ogg");
			}
			jngl::SetColor(0,0,255,128);
			DrawMouse();
			jngl::EndDraw();
			if(++frameNumber == 500)
			{
				std::cout << "It took " << jngl::Time() - frameTime << " seconds to render 500 frames." << std::endl;
				frameNumber = 0;
				frameTime = jngl::Time();
			}
			if(jngl::KeyDown('k'))
			{
				TestKeys();
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void DrawBackground()
{
	jngl::SetSpriteColor(255, 255, 255, 100);
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
	jngl::DrawEllipse(80, 400, 50, 80);
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
	jngl::Vertex(80, 580);
	jngl::Vertex(130, 480);
	jngl::Vertex(180, 380);
	jngl::Vertex(230, 380);
	jngl::Vertex(280, 480);
	jngl::Vertex(330, 480);
	jngl::Vertex(380, 380);
	jngl::Vertex(430, 480);
	jngl::Vertex(480, 480);
	jngl::Vertex(530, 580);
	jngl::EndPolygon();
	jngl::SetFontColor(255, 0, 0);
	jngl::Print("A", 80, 580);
	jngl::Print("B", 130, 480);
	jngl::Print("C", 180, 380);
	jngl::Print("D", 230, 380);
	jngl::Print("E", 280, 480);
	jngl::Print("F", 330, 480);
	jngl::Print("G", 380, 380);
	jngl::Print("H", 430, 480);
	jngl::Print("I", 480, 480);
	jngl::Print("J", 530, 580);
}

class RecentlyPressedKey {
public:
	RecentlyPressedKey(std::string name, int x, int y) : name_(name), alpha_(255), x_(x), y_(y), lastTime_(jngl::Time()) {}
	void Draw()
	{
		double timeSinceLastFrame = jngl::Time() - lastTime_;
		lastTime_ = jngl::Time();
		alpha_ -= timeSinceLastFrame * 60;
		x_ += timeSinceLastFrame * 40;
		jngl::SetFontColor(0, 0, 0, static_cast<unsigned char>(alpha_));
		jngl::Print(name_, int(x_), int(y_));
		jngl::SetFontColor(0, 0, 0, 255);
	}
	int GetAlpha() const
	{
		return int(alpha_);
	}
private:
	std::string name_;
	double alpha_, x_, y_, lastTime_;
};

void TestKeys()
{
	typedef std::map<std::string, jngl::key::KeyType> MapType;
	MapType keys;
	keys["Left"] = jngl::key::Left;
	keys["Up"] = jngl::key::Up;
	keys["Right"] = jngl::key::Right;
	keys["Down"] = jngl::key::Down;
	keys["PageUp"] = jngl::key::PageUp;
	keys["PageDown"] = jngl::key::PageDown;
	keys["Home"] = jngl::key::Home;
	keys["End"] = jngl::key::End;
	keys["BackSpace"] = jngl::key::BackSpace;
	keys["Tab"] = jngl::key::Tab;
	keys["Clear"] = jngl::key::Clear;
	keys["Return"] = jngl::key::Return;
	keys["Pause"] = jngl::key::Pause;
	keys["Escape"] = jngl::key::Escape;
	keys["Delete"] = jngl::key::Delete;
	keys["ControlL"] = jngl::key::ControlL;
	keys["ControlR"] = jngl::key::ControlR;
	keys["CapsLock"] = jngl::key::CapsLock;
	keys["AltL"] = jngl::key::AltL;
	keys["AltR"] = jngl::key::AltR;
	keys["SuperL"] = jngl::key::SuperL;
	keys["SuperR"] = jngl::key::SuperR;
	keys["Space"] = jngl::key::Space;
	keys["ShiftL"] = jngl::key::ShiftL;
	keys["ShiftR"] = jngl::key::ShiftR;
	keys["F1"] = jngl::key::F1;
	keys["F2"] = jngl::key::F2;
	keys["F3"] = jngl::key::F3;
	keys["F4"] = jngl::key::F4;
	keys["F5"] = jngl::key::F5;
	keys["F6"] = jngl::key::F6;
	keys["F7"] = jngl::key::F7;
	keys["F8"] = jngl::key::F8;
	keys["F9"] = jngl::key::F9;
	keys["F10"] = jngl::key::F10;
	keys["F11"] = jngl::key::F11;
	keys["F12"] = jngl::key::F12;
	keys["Any"] = jngl::key::Any;
	std::vector<RecentlyPressedKey> recentlyPressedKeys;
	while(jngl::Running())
	{
		jngl::BeginDraw();
		jngl::SetFontSize(10);
		int y = 10;
		for(MapType::iterator it = keys.begin(); it != keys.end(); ++it)
		{
			if(jngl::KeyDown(it->second))
			{
				jngl::SetFontColor(0, 0, 0);
			}
			else
			{
				jngl::SetFontColor(150, 150, 150);
			}
			jngl::Print(it->first, 100, y);
			if(jngl::KeyPressed(it->second))
			{
				recentlyPressedKeys.push_back(RecentlyPressedKey(it->first, 100, y));
			}
			y += 15;
		}
		y = 10;
		for(char c = '0'; c <= 'z'; ++c)
		{
			char cString[2];
			cString[0] = c;
			cString[1] = 0;
			if(jngl::KeyDown(c))
			{
				jngl::SetFontColor(0, 0, 0);
			}
			else
			{
				jngl::SetFontColor(150, 150, 150);
			}
			jngl::Print(cString, 500, y);
			if(jngl::KeyPressed(c))
			{
				recentlyPressedKeys.push_back(RecentlyPressedKey(cString, 500, y));
			}
			y += 15;
			if(c == '9')
			{
				c = 'a' - 1;
			}
		}
		std::vector<RecentlyPressedKey>::iterator end = recentlyPressedKeys.end();
		for(std::vector<RecentlyPressedKey>::iterator it = recentlyPressedKeys.begin(); it != end; ++it)
		{
			if(it->GetAlpha() <= 0)
			{
				it = recentlyPressedKeys.erase(it);
			}
			else
			{
				it->Draw();
			}
		}
		jngl::EndDraw();
	}
}
