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

	void hideWindow();

	bool running();

	void updateInput();

	void swapBuffers();

	void quit();

	void continue();

	void draw(const std::string& filename,
	          double xposition,
	          double yposition);

	template<class Vect>
	void draw(const std::string& filename, Vect pos) {
		draw(filename, pos.x, pos.y);
	}

	void drawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float xfactor,
	                float yfactor);

	void drawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float factor);

	void drawClipped(const std::string& filename,
					 double xposition,
					 double yposition,
					 float xstart,
					 float xend,
					 float ystart,
					 float yend);

	void drawRect(double xposition, double yposition, double width, double height);

	template<class Vect>
	void drawRect(Vect pos, Vect size) {
		DrawRect(pos.x, pos.y, size.x, size.y);
	}

	void drawLine(double xstart, double ystart, double xend, double yend);

	void drawEllipse(double xmid, double ymid, double width, double height);

	void drawPoint(double x, double y);

	void rotate(double degree);

	void translate(double x, double y);

	void scale(double factor);

	void scale(double xfactor, double yfactor);

	void pushMatrix();

	void popMatrix();

	void reset();

	void load(const std::string& filename);

	void unload(const std::string& filename);

	void unloadAll();

	int getWidth(const std::string& filename);

	int getHeight(const std::string& filename);

	double Time();

	void beginPolygon();

	void vertex(double xposition,
				double yposition);

	void endPolygon();

	void setRelativeMouseMode(bool relative);

	bool getRelativeMouseMode();

	void setMouseVisible(bool visible);

	bool isMouseVisible();

	int getMouseX();

	int getMouseY();

	namespace mouse
	{
		enum Button
		{
			Left, Middle, Right
		};
	}

	double getMouseWheel();

	bool mouseDown(mouse::Button button = mouse::Left);

	bool mousePressed(mouse::Button button = mouse::Left);

	void setMouse(int xposition, int yposition);

	bool drawButton(const std::string& sprite,
	                double xposition,
	                double yposition,
	                const std::string& mouseover);

	void setTitle(const std::string& title);

	void setBackgroundColor(unsigned char red,
	                        unsigned char green,
	                        unsigned char blue);

	void setColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha = 255);

	void setFontColor(unsigned char red,
	                  unsigned char green,
	                  unsigned char blue,
	                  unsigned char alpha = 255);

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
	                    unsigned char alpha = 255);

	void setSpriteAlpha(unsigned char alpha);

	void print(const std::string& text,
	           int xposition,
	           int yposition);

	int getFontSize();

	void setFontSize(int size);

	std::string getFont();

	void setFont(const std::string& filename);

	void setFontByName(const std::string& name);

	void sleep(int milliseconds);

	double FPS();

	void errorMessage(const std::string& text);

	bool getFullscreen();

	void readPixel(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);

	int getWindowWidth();

	int getWindowHeight();

	void setAntiAliasing(bool enabled);

	bool getAntiAliasing();

	void setVerticalSync(bool enabled);

	bool getVerticalSync();

	double getTextWidth(const std::string& text);

	int getLineHeight();

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
