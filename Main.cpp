#include <GL/glew.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>

void draw();
void draw_quad(float, float, float, float, int, int);
float f(float, float);
float* n(float, float);
void get_z_range(float&, float&);
void set_color_by_height(float, float, float);
void viewing(int, int);

// Sin, Para, Wave, Ripple, Gaussian (default)
std::string function = "Para";

class Quad
{
public:Quad()
{
	glBegin(GL_QUADS);
}
	  void addPoint(float x, float y, float z)
	  {
		  glVertex3f(x, y, z);
	  }

	  void addNormal(float* n)
	  {
		  glNormal3f(n[0], n[1], n[2]);
	  }

	  void draw()
	  {
		  glEnd();
	  }
};

int main(int argc, char** argv)
{
	// Initialize GLUT library
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// Specify the initial window size
	glutInitWindowSize(800, 600);

	// Create a window with given title
	glutCreateWindow("Visualization");

	// Set a background color (dark blue)
	glClearColor(0.1f, 0.1f, 0.3f, 1.0f);

	// Specify function to draw scene
	glutDisplayFunc(draw);

	// Specify function to set up viewing
	glutReshapeFunc(viewing);

	// Start the event loop
	glutMainLoop();
	return 0;
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (function == "Sin")
	{
		draw_quad(-1.0f, 1.0f, -1.0f, 1.0f, 300, 300);
	}
	else if (function == "Wave")
	{
		draw_quad(-2.0f * M_PI, 2.0f * M_PI, -2.0f * M_PI, 2.0f * M_PI, 100, 100);
	}
	else if (function == "Para")
	{

		draw_quad(-2.0f, 2.0f, -2.0f, 2.0f, 100, 100);
	}
	else if (function == "Ripple")
	{

		draw_quad(-3, 3, -3, 3, 100, 100);
	}
	else
	{
		draw_quad(-1.0f, 1.0f, -1.0f, 1.0f, 30, 30);
	}
	glutSwapBuffers();
}

void draw_quad(float x_min, float x_max, float y_min, float y_max, int n_x, int n_y) {
	float dx = (x_max - x_min) / (n_x - 1);
	float dy = (y_max - y_min) / (n_y - 1);

	float z, z_min, z_max;
	get_z_range(z_min, z_max);

	for (float x = x_min; x <= x_max - dx; x += dx) {
		for (float y = y_min; y <= y_max - dy; y += dy) {
			Quad q;

			z = f(x, y);
			set_color_by_height(z, z_min, z_max);
			q.addNormal(n(x, y));
			q.addPoint(x, y, z);

			z = f(x + dx, y);
			set_color_by_height(z, z_min, z_max);
			q.addNormal(n(x + dx, y));
			q.addPoint(x + dx, y, z);

			z = f(x + dx, y + dy);
			set_color_by_height(z, z_min, z_max);
			q.addNormal(n(x + dx, y + dy));
			q.addPoint(x + dx, y + dy, z);

			z = f(x, y + dy);
			set_color_by_height(z, z_min, z_max);
			q.addNormal(n(x, y + dy));
			q.addPoint(x, y + dy, z);

			q.draw();
		}
	}
}

float f(float x, float y)
{
	if (function == "Sin")
	{
		return std::sin(1.0f / (x * x + y * y));
	}
	else if (function == "Wave")
	{
		return std::sin(x) + std::cos(y);
	}
	else if (function == "Para")
	{
		return x * x + y * y;
	}
	else if (function == "Ripple")
	{
		return std::sin(10.0f * (x * x + y * y)) / 10.0f;
	}
	else
	{
		return std::exp(-(x * x + y * y));
	}
}

float* n(float x, float y)
{
	float* normal = new float[3];
	float nx = 0, ny = 0, nz = 0;

	if (function == "Sin")
	{
		float nx = 2.0f * x * std::cos(1.0f / (x * x + y * y)) / std::pow(x * x + y * y, 2.0f);
		float ny = 2.0f * y * std::cos(1.0f / (x * x + y * y)) / std::pow(x * x + y * y, 2.0f);
		float nz = 1.0f;
	}
	else if (function == "Wave")
	{
		float nx = std::cos(x);
		float ny = -std::sin(y);
		float nz = 1.0f;
	}
	else if (function == "Para")
	{
		nx = -2.0f * x;
		ny = -2.0f * y;
		nz = 1.0f;
	}
	else if (function == "Ripple")
	{
		nx = 2.0f * x * std::cos(10 * (x * x + y * y));
		ny = 2.0f * y * std::cos(10 * (x * x + y * y));
		nz = 1.0f;
	}
	else
	{
		nx = 2.0f * x * f(x, y);;
		ny = 2.0f * y * f(x, y);;
		nz = 1.0f;
	}

	float length = std::sqrt(nx * nx + ny * ny + nz * nz);
	if (length > 0)
	{
		normal[0] = nx / length;
		normal[1] = ny / length;
		normal[2] = nz / length;
	}
	else
	{
		normal[0] = 0.0f;
		normal[1] = 0.0f;
		normal[2] = 0.0f;
	}

	return normal;
}

void get_z_range(float& z_min, float& z_max) {
	if (function == "Sin")
	{
		z_min = -1.5f; z_max = 1.5f;
	}
	else if (function == "Wave")
	{
		z_min = -6.0f; z_max = 8.0f;
	}
	else if (function == "Para")
	{
		z_min = 0.0f; z_max = 8.0f;
	}
	else if (function == "Ripple")
	{
		z_min = -0.2f; z_max = 0.2f;
	}
	else
	{
		z_min = 0.0f; z_max = 1.3f;
	}
}

void set_color_by_height(float z, float z_min, float z_max) {

	float t = (z - z_min) / (z_max - z_min);

	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	float R = t;
	float G = 0.0f;
	float B = 1.0f - t;

	glColor3f(R, G, B);
}

void viewing(int W, int H)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (function == "Sin")
	{
		gluLookAt(3.0f, 5.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Wave")
	{
		gluLookAt(0, -30, 35, 0, 0, 0, 0, 0, 1);
	}
	else if (function == "Para")
	{
		gluLookAt(5.0f, 12.0f, 10.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Ripple")
	{
		gluLookAt(5, -15, 10, 0, 0, 0, 0, 0, 1);
	}
	else
	{
		gluLookAt(3.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = float(W) / H;

	if (function == "Sin")
	{
		gluPerspective(40.0f, aspect, 0.1f, 100.0f);
	}
	else if (function == "Para")
	{
		gluPerspective(40.0f, aspect, 0.1f, 100.0f);
	}
	else
	{
		gluPerspective(40.0f, aspect, 0.1f, 100.0f);
	}

	glViewport(0, 0, W, H);
}