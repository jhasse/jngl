#include <jngl.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class Wuerfel
{
public:
	inline static std::string GetName()
	{
		return name_;
	}

	inline static int GetHeight()
	{
		return height_;
	}

	inline static int GetWidth()
	{
		return width_;
	}

	inline static int GetFrontHeight()
	{
		return frontHeight_;
	}

	inline static int GetFrontWidth()
	{
		return frontWidth_;
	}

	inline static double GetFrontFaktor()
	{
		return frontFaktor_;
	}

	static void SetWuerfel(std::string name, double faktor = .666666)
	{
		name_ = name;
		frontFaktor_ = faktor;
		height_ = jngl::Height(name);
		width_ = jngl::Width(name);
		frontHeight_ = static_cast<int>((height_ * faktor) + .5);
		frontWidth_ = static_cast<int>((width_ * faktor) + .5);
	}

private:
	static std::string name_;
	static int height_;
	static int width_;
	static int frontHeight_;
	static int frontWidth_;
	static double frontFaktor_;
};

std::string Wuerfel::name_ = "";
int Wuerfel::height_ = 0;
int Wuerfel::width_ = 0;
int Wuerfel::frontHeight_ = 0;
int Wuerfel::frontWidth_ = 0;
double Wuerfel::frontFaktor_ = 0;

int main()
{

	std::cout << "Wie viele Ebenen sollen angezeigt werden (max 25)?" << std::endl;
	int ebenen = 0;
	std::cin >> ebenen;

	int kantenlaengeUnten = ebenen * 2 - 1;

	int ausgleich = (ebenen - 1) * (Wuerfel::GetHeight() - Wuerfel::GetFrontHeight());

	jngl::ShowWindow("Pyramide", 1024, 768);
	Wuerfel::SetWuerfel("wuerfel.png");

	jngl::BackgroundColor(0,0,0);

//	jngl::FontColor(255, 255, 255);

	while (jngl::Running())
	{
		if (jngl::KeyDown(jngl::key::Escape))
			jngl::Quit();

		jngl::BeginDraw();
		long long count = 0;
		for (int ebeneAktuell = ebenen; ebeneAktuell > 0; --ebeneAktuell)
		{
			kantenlaengeUnten = ebeneAktuell * 2 - 1;
			for (int y = 0; y < kantenlaengeUnten; ++y)
			{
				for (int x = 0; x < kantenlaengeUnten; ++x)
				{
					++count;
					jngl::Draw(Wuerfel::GetName(), \
					(ebenen - ebeneAktuell) * Wuerfel::GetWidth() + (kantenlaengeUnten - 1 - y) * (Wuerfel::GetWidth() - Wuerfel::GetFrontWidth()) + x * Wuerfel::GetFrontWidth(), \
					ausgleich + y * Wuerfel::GetFrontHeight() - (y * (Wuerfel::GetHeight() - Wuerfel::GetFrontHeight())) - (ebenen - ebeneAktuell) * (Wuerfel::GetHeight() - Wuerfel::GetFrontHeight()));
				}
			}
		}
		std::stringstream sstream;
		sstream << "FPS: " << jngl::FPS() << " Wuerfel: " << count;
		jngl::SetTitle(sstream.str());
		jngl::EndDraw();
	}
}
