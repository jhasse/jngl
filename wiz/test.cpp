#include <jngl.hpp>
#include <iostream>
#include <iomanip>
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <wizGLES.h>

#define f2x(x) ((int)((x) * 65536))

void Bench(void (*draw)(), const std::string& name)
{
	std::cout << name << " ..." << std::flush;
	double startSeconds = jngl::Time();
	for(int i = 0; i < 100; ++i)
	{
		draw();
		jngl::SwapBuffers();
	}
	const double s = jngl::Time() - startSeconds;
	const double fps = (100 / s);
	std::cout << " " << std::setprecision(4) << s << "s" << " (" << fps << " FPS; " << fps * 1536 << " triangles per second)" << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint id;

void JNGL()
{
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			jngl::DrawRect(x * 10, y * 10, 9, 9);
		}
	}
}

void JNGLTextured()
{
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			jngl::Draw("9x9.png", x * 10, y * 10);
		}
	}
}

void VertexArrayFloat()
{
	GLfloat rect[] = { 0, 0, 9, 0, 9, 9, 0, 9 };
	glVertexPointer(2, GL_FLOAT, 0, rect);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			glPushMatrix();
			glTranslatef(x * 10, y * 10, 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glPopMatrix();
		}
	}
}

void VertexArrayFixed()
{
	GLfixed rect[] = { 0, 0, f2x(9.0f), 0, f2x(9.0f), f2x(9.0f), 0, f2x(9.0f) };
	glVertexPointer(2, GL_FIXED, 0, rect);
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			glPushMatrix();
			glTranslatex(f2x(x * 10), f2x(y * 10), 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glPopMatrix();
		}
	}
}

void VBOFloat()
{
	GLuint buffer;
	GLfloat vertexes[] = { 0, 0, 0, 9, 9, 9, 9, 0 };
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			glPushMatrix();
			glTranslatef(x * 10, y * 10, 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glPopMatrix();
		}
	}
}

void VBOFixed()
{
	GLuint buffer;
	GLfixed vertexes[] = { 0, 0, 0, 9, 9, 9, 9, 0 };
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfixed), vertexes, GL_STATIC_DRAW);
	glVertexPointer(2, GL_FIXED, 0, 0);
	for(int x = 0; x < 32; ++x)
	{
		for(int y = 0; y < 24; ++y)
		{
			glPushMatrix();
			glTranslatef(x * 10, y * 10, 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glPopMatrix();
		}
	}
}

void Nothing()
{
}

int main()
{
	jngl::ShowWindow("Benchmark", 320, 240);
	id = jngl::Load("9x9.png");
	while(jngl::Time() < 5)
	{
		jngl::Print("This programm draws 1536 triangles\nper frame.\nSee console output for benchmark\nresults.", 10, 10);
		jngl::SwapBuffers();
	}
	Bench(Nothing, "Drawing nothing");
	Bench(VertexArrayFloat, "Vertex Arrays float");
	Bench(VertexArrayFixed, "Vertex Arrays fixed");
	Bench(VBOFloat, "VBO float");
	Bench(VBOFixed, "VBO fixed");
	Bench(JNGL, "JNGL");
	Bench(JNGLTextured, "JNGL Textured");
}
