/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#ifndef __JNGL_HPP__
#define __JNGL_HPP__

#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace jngl
{
	void showWindow(const std::string& title,
	                int width,
	                int heigt,
	                bool fullscreen = false);

	void HideWindow();

	bool Running();

	void UpdateInput();

	void SwapBuffers();

	void Quit();

	void Continue();

	void Draw(const std::string& filename,
	          double xposition,
	          double yposition);

	template<class Vect>
	void draw(const std::string& filename, Vect pos) {
		Draw(filename, pos.x, pos.y);
	}

	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float xfactor,
	                float yfactor);

	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float factor);

	void DrawClipped(const std::string& filename,
					 double xposition,
					 double yposition,
					 float xstart,
					 float xend,
					 float ystart,
					 float yend);

	void DrawRect(double xposition, double yposition, double width, double height);

	template<class Vect>
	void drawRect(Vect pos, Vect size) {
		DrawRect(pos.x, pos.y, size.x, size.y);
	}

	void DrawLine(double xstart, double ystart, double xend, double yend);

	void DrawEllipse(double xmid, double ymid, double width, double height);

	void DrawPoint(double x, double y);

	void Rotate(double degree);

	void Translate(double x, double y);

	void Scale(double factor);

	void Scale(double xfactor, double yfactor);

	void PushMatrix();

	void PopMatrix();

	void Reset();

	void Load(const std::string& filename);

	void Unload(const std::string& filename);

	void UnloadAll();

	int GetWidth(const std::string& filename);

	int GetHeight(const std::string& filename);

	double Time();

	void BeginPolygon();

	void Vertex(double xposition,
				double yposition);

	void EndPolygon();

	void SetRelativeMouseMode(bool relative);

	bool GetRelativeMouseMode();

	void SetMouseVisible(bool visible);

	bool IsMouseVisible();

	int GetMouseX();

	int GetMouseY();

	namespace mouse
	{
		enum Button
		{
			Left, Middle, Right
		};
	}

	double GetMouseWheel();

	bool MouseDown(mouse::Button button = mouse::Left);

	bool MousePressed(mouse::Button button = mouse::Left);

	void SetMouse(int xposition, int yposition);

	bool DrawButton(const std::string& sprite,
	                double xposition,
	                double yposition,
	                const std::string& mouseover);

	void SetTitle(const std::string& title);

	void SetBackgroundColor(unsigned char red,
	                        unsigned char green,
	                        unsigned char blue);

	void SetColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha = 255);

	void SetFontColor(unsigned char red,
	                  unsigned char green,
	                  unsigned char blue,
	                  unsigned char alpha = 255);

	void SetSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
	                    unsigned char alpha = 255);

	void setSpriteAlpha(unsigned char alpha);

	void Print(const std::string& text,
	           int xposition,
	           int yposition);

	int GetFontSize();

	void SetFontSize(int size);

	std::string GetFont();

	void SetFont(const std::string& filename);

	void SetFontByName(const std::string& name);

	void Sleep(int milliseconds);

	double FPS();

	void ErrorMessage(const std::string& text);

	bool GetFullscreen();

	void ReadPixel(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);

	int GetWindowWidth();

	int GetWindowHeight();

	void SetAntiAliasing(bool enabled);

	bool GetAntiAliasing();

	void SetVerticalSync(bool enabled);

	bool GetVerticalSync();

	double GetTextWidth(const std::string& text);

	int GetLineHeight();

	namespace key
	{
		enum KeyType
		{
			Left,
			Up,
			Right,
			Down,
			PageUp,
			PageDown,
			Home,
			End,
			BackSpace,
			Tab,
			Clear,
			Return,
			Pause,
			Escape,
			Delete,
			ControlL,
			ControlR,
			CapsLock,
			AltL,
			AltR,
			SuperL,
			SuperR,
			Space,
			ShiftL,
			ShiftR,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			WizUp,
			WizDown,
			WizLeft,
			WizRight,
			WizA,
			WizB,
			WizX,
			WizY,
			WizL,
			WizR,
			WizMenu,
			WizSelect,
			WizVolUp,
			WizVolDown,
			Any
		};
	}

	bool KeyDown(key::KeyType key);

	bool KeyDown(char key);

	bool KeyDown(const std::string& key);

	bool KeyPressed(key::KeyType key);

	bool KeyPressed(char key);

	bool KeyPressed(const std::string& key);

	void Play(const std::string& filename);

	void Stop(const std::string& filename);

	bool IsPlaying(const std::string& filename);

	bool IsOpenALInstalled();

	void SetPlaybackSpeed(float speed);

	void SetVolume(float volume);

	void SetIcon(const std::string& filename);

	int GetDesktopWidth();

	int GetDesktopHeight();

	class FrameBufferImpl;
	class FrameBuffer {
	public:
		FrameBuffer(int width, int height);
		~FrameBuffer();
		void beginDraw();
		void endDraw();
		void draw(int x, int y) const;
		void clear();
	private:
		FrameBuffer(const FrameBuffer&);
		FrameBuffer& operator=(const FrameBuffer&);
		FrameBufferImpl* pImpl;
	};

	class Work {
	public:
		virtual void step() = 0;
		virtual void draw() const = 0;
		virtual void onQuitEvent();
		virtual void onLoad();
		virtual ~Work();
	};

	boost::shared_ptr<Work> GetWork();

	void SetWork(boost::shared_ptr<Work> work);

	void SetWork(Work*);

	void MainLoop();

	void setPrefix(const std::string& path);

	std::string getPrefix();

	void setConfigPath(const std::string& path);

	std::string getConfigPath();
}

#endif // __JNGL_HPP__
