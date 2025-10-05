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
void keyboard(unsigned char, int, int);
void draw_grid(float, float, float, float, int, int, float _z = -0.1f);
void update_light_position();

// Sin, Para, Ripple, Wave, Gaussian, Fenc (default)
std::string function = "";
int shading_mode = 1; // flat (default)

float light_pos_x = 5.0f;
float light_pos_y = -5.0f;
float light_pos_z = 5.0f;
float light_move_step = 1.5f;

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

void init() {
	// Enable depth testing for proper 3D rendering (hiding objects behind others)
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// Set a background color (dark blue)
	glClearColor(0.1f, 0.1f, 0.3f, 1.0f);

	// --- LIGHTING SETUP ---
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Define the light properties
	float light_pos[] = { light_pos_x, light_pos_y, light_pos_z, 1.0f }; // Positional light
	float white_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambient_light[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);

	// Enable color tracking, so glColor3f affects the material properties
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// --- GLEW INIT ---
	// Initialize GLEW *after* a GLUT window is created
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		exit(1);
	}
}

int main(int argc, char** argv)
{
	// Initialize GLUT library
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// Specify the initial window size
	glutInitWindowSize(800, 600);
	
	// Create a window with given title
	glutCreateWindow("Visualization");

	init();

	// Specify function to draw scene
	glutDisplayFunc(draw);

	// Specify function to set up viewing
	glutReshapeFunc(viewing);

	// Bind the new keyboard function
	glutKeyboardFunc(keyboard);

	// Start the event loop
	glutMainLoop();
	return 0;
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (function == "Sin")
	{
		draw_quad(-1.0f, 1.0f, -1.0f, 1.0f, 100, 100);
		draw_grid(-1.0f, 1.0f, -1.0f, 1.0f, 100, 100, -1.0f);
	}
	else if (function == "Wave")
	{
		draw_quad(-2.0f * M_PI, 2.0f * M_PI, -2.0f * M_PI, 2.0f * M_PI, 50, 50);
		draw_grid(-2.0f * M_PI, 2.0f * M_PI, -2.0f * M_PI, 2.0f * M_PI, 50, 50, -2.0f);
	}
	else if (function == "Para")
	{
		draw_quad(-2.0f, 2.0f, -2.0f, 2.0f, 30, 30);
		draw_grid(-2.0f, 2.0f, -2.0f, 2.0f, 30, 30);
	}
	else if (function == "Ripple")
	{

		draw_quad(-3.0f, 3.0f, -3.0f, 3.0f, 100, 100);
	}
	else if (function == "Fenc")
	{

		draw_quad(-1.0f, 1.0f, -1.0f, 1.0f, 30, 30);
		draw_grid(-1.0f, 1.0f, -1.0f, 1.0f, 30, 30, -0.5f);
	}
	else
	{
		draw_quad(-1.0f, 1.0f, -1.0f, 1.0f, 30, 30);
		draw_grid(-1.0f, 1.0f, -1.0f, 1.0f, 30, 30);
	}
	glutSwapBuffers();
	glutPostRedisplay();
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
	else if (function == "Fenc")
	{
		return 0.75f / std::exp(std::pow(x * 5, 2) * std::pow(y * 5, 2));
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
		float nx = -std::cos(x);
		float ny = std::sin(y);
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
		nx = -2.0f * x * std::cos(10.0f * (x * x + y * y));
		ny = -2.0f * y * std::cos(10.0f * (x * x + y * y));
		nz = 1.0f;
	}
	else if (function == "Fenc")
	{
		nx = 937.5f * x * y * y * std::exp(-625.0f * x * x * y * y);
		ny = 937.5f * x * x * y * std::exp(-625.0f * x * x * y * y);
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
		z_min = -4.0f; z_max = 4.0f;
	}
	else if (function == "Para")
	{
		z_min = 0.0f; z_max = 8.0f;
	}
	else if (function == "Ripple")
	{
		z_min = -0.1f; z_max = 0.1f;
	}
	else if (function == "Fenc")
	{
		z_min = 0.0f; z_max = 0.7f;
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
		//gluLookAt(3.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		//gluLookAt(3.0f, 4.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		gluLookAt(0.0f, 4.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Wave")
	{
		gluLookAt(25, 15, 5, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Para")
	{
		gluLookAt(5.0f, 12.0f, 10.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Ripple")
	{
		gluLookAt(5.0f, 7.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (function == "Fenc")
	{
		gluLookAt(3.0f, 3.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		gluLookAt(3.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = float(W) / H;
	gluPerspective(40.0f, aspect, 0.1f, 100.0f);
	glViewport(0, 0, W, H);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 's': // Toggle Shading Mode
		shading_mode = 1 - shading_mode; // Toggles between 0 and 1
		if (shading_mode == 1) {
			// Smooth Shading: Interpolates color/normal across the polygon.
			glShadeModel(GL_SMOOTH);
		}
		else {
			// Flat Shading: Uses the color/normal of the last vertex for the entire polygon.
			glShadeModel(GL_FLAT);
		}
		glutPostRedisplay(); // Request redraw to apply change
		break;
	case 'X': light_pos_x += light_move_step; update_light_position(); break;
	case 'x': light_pos_x -= light_move_step; update_light_position(); break;
	case 'Y': light_pos_y += light_move_step; update_light_position(); break;
	case 'y': light_pos_y -= light_move_step; update_light_position(); break;
	case 'Z': light_pos_z += light_move_step; update_light_position(); break;
	case 'z': light_pos_z -= light_move_step; update_light_position(); break;
	case 27: // ESC key to exit
		exit(0);
		break;
	}
}

void draw_grid(float x_min, float x_max, float y_min, float y_max, int n_x, int n_y, float _z) {
	float dx = (x_max - x_min) / (n_x - 1);
	float dy = (y_max - y_min) / (n_y - 1);
	const float grid_z = _z; // Z-coordinate for the grid plane

	// Disable lighting and depth masking so the grid is drawn clearly
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE); // Prevents the grid from writing to the depth buffer

	// Set grid color (e.g., black or red border/black interior as shown in the image)
	glColor3f(0.0f, 0.0f, 0.0f); // Black lines for the interior grid

	// Draw all vertical lines
	glBegin(GL_LINES);
	for (int i = 0; i < n_x; ++i) {
		float x = x_min + i * dx;
		// Line starts at (x, y_min, z)
		glVertex3f(x, y_min, grid_z);
		// Line ends at (x, y_max, z)
		glVertex3f(x, y_max, grid_z);
	}
	glEnd();

	// Draw all horizontal lines
	glBegin(GL_LINES);
	for (int j = 0; j < n_y; ++j) {
		float y = y_min + j * dy;
		// Line starts at (x_min, y, z)
		glVertex3f(x_min, y, grid_z);
		// Line ends at (x_max, y, z)
		glVertex3f(x_max, y, grid_z);
	}
	glEnd();

	// Draw the red border using thicker lines (optional, but looks better)
	glColor3f(1.0f, 0.0f, 0.0f); // Red border
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP); // Draws a closed loop for the border
	glVertex3f(x_min, y_min, grid_z);
	glVertex3f(x_max, y_min, grid_z);
	glVertex3f(x_max, y_max, grid_z);
	glVertex3f(x_min, y_max, grid_z);
	glEnd();
	glLineWidth(1.0f); // Reset line width

	// Re-enable states for the surface plot
	glDepthMask(GL_TRUE); // Re-enable depth writing
	glEnable(GL_LIGHTING); // Re-enable lighting
}

void update_light_position() {
	// The fourth component (1.0f) makes this a positional light source
	float light_pos[] = { light_pos_x, light_pos_y, light_pos_z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glutPostRedisplay(); // Request redraw to show the new shading
}