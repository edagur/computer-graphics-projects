
#include "Angel.h"
#include <conio.h>
#include <stdio.h>

typedef vec4  color4;
typedef vec4  point4;
int currentface = 0;
bool update = false;
const int NumVerticesPerSmallCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
point4 points[NumVerticesPerSmallCube * 27 + 24 * 27];
color4 colors[NumVerticesPerSmallCube * 27 + 24 * 27];
float smallcubelength = 0.3333333333333333333333333333333333;
mat4 ModelViews[27];
// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 10.0, -45.0, 0.0 };
// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
int Index = 0;
int cube;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[64] = {
	//front
	//4th row
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5,  0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5,  0.5, 1.0),
	point4(0.5,  -0.5,  0.5, 1.0),
	//3rd row
	point4(-0.5, -0.5 + smallcubelength,  0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + smallcubelength,  0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + smallcubelength,  0.5, 1.0),
	point4(0.5,  -0.5 + smallcubelength,  0.5, 1.0),
	//2nd row
	point4(-0.5, -0.5 + 2 * smallcubelength,  0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + 2 * smallcubelength,  0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + 2 * smallcubelength,  0.5, 1.0),
	point4(0.5,  -0.5 + 2 * smallcubelength,  0.5, 1.0),
	//1st
	point4(-0.5, 0.5,  0.5, 1.0),
	point4(-0.5 + smallcubelength, 0.5,  0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, 0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	//middle first
	//4th row
	point4(-0.5, -0.5,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + smallcubelength, -0.5,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5,  0.5 - smallcubelength, 1.0),
	point4(0.5,  -0.5,  0.5 - smallcubelength, 1.0),
	//3rd row
	point4(-0.5, -0.5 + smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(0.5,  -0.5 + smallcubelength,  0.5 - smallcubelength, 1.0),
	//2nd row
	point4(-0.5, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength, 1.0),
	point4(0.5,  -0.5 + 2 * smallcubelength,  0.5 - smallcubelength, 1.0),
	//1st
	point4(-0.5, 0.5,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + smallcubelength, 0.5,  0.5 - smallcubelength, 1.0),
	point4(-0.5 + 2 * smallcubelength, 0.5,  0.5 - smallcubelength, 1.0),
	point4(0.5,  0.5,  0.5 - smallcubelength, 1.0),
	//middle second
	//4th row
	point4(-0.5, -0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + smallcubelength, -0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(0.5,  -0.5,  0.5 - smallcubelength * 2, 1.0),
	//3rd row
	point4(-0.5, -0.5 + smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(0.5,  -0.5 + smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	//2nd row
	point4(-0.5, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + 2 * smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	point4(0.5,  -0.5 + 2 * smallcubelength,  0.5 - smallcubelength * 2, 1.0),
	//1st
	point4(-0.5, 0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + smallcubelength, 0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(-0.5 + 2 * smallcubelength, 0.5,  0.5 - smallcubelength * 2, 1.0),
	point4(0.5,  0.5,  0.5 - smallcubelength * 2, 1.0),
	//back
	//4th row
	point4(-0.5, -0.5,  -0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5,  -0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5,  -0.5, 1.0),
	point4(0.5,  -0.5,  -0.5, 1.0),
	//3rd row
	point4(-0.5, -0.5 + smallcubelength,  -0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + smallcubelength,  -0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + smallcubelength,  -0.5, 1.0),
	point4(0.5,  -0.5 + smallcubelength,  -0.5, 1.0),
	//2nd row
	point4(-0.5, -0.5 + 2 * smallcubelength,  -0.5, 1.0),
	point4(-0.5 + smallcubelength, -0.5 + 2 * smallcubelength,  -0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, -0.5 + 2 * smallcubelength,  -0.5, 1.0),
	point4(0.5,  -0.5 + 2 * smallcubelength,  -0.5, 1.0),
	//1st
	point4(-0.5, 0.5,  -0.5, 1.0),
	point4(-0.5 + smallcubelength, 0.5,  -0.5, 1.0),
	point4(-0.5 + 2 * smallcubelength, 0.5,  -0.5, 1.0),
	point4(0.5,  0.5,  -0.5, 1.0),
};

// RGBA olors
color4 vertex_colors[7] = {
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
};

mat3 faces[6] = {

	mat3(0,1,2,3,4,5,6,7,8),			//front face
	mat3(8, 17, 26, 5, 14, 23, 2, 11, 20),		//right face
	mat3(26,25,24, 23, 22, 21, 20,19,18), //back
	mat3(18,9,0,21,12,3,24,15,6),		//left
	mat3(24,15,6,25, 16,7,26,17,8),		//top
	mat3(20,11,2,19, 10, 1,18,9,0)	//bottom

};

// quad generates two triangles for each face and assigns colors to the vertices
void
quad(int a, int b, int c, int d, color4 k)
{
	// Initialize colors

	colors[Index] = k; points[Index] = vertices[a]; Index++;
	colors[Index] = k; points[Index] = vertices[b]; Index++;
	colors[Index] = k; points[Index] = vertices[c]; Index++;
	colors[Index] = k; points[Index] = vertices[a]; Index++;
	colors[Index] = k; points[Index] = vertices[c]; Index++;
	colors[Index] = k; points[Index] = vertices[d]; Index++;

}

// generate 12 triangles: 36 vertices and 36 colors
void
smallCube(int zero, int one, int two, int three, int four, int five, int six, int seven)
{
	//triangles 12 per small cube
	quad(one, zero, three, two, vertex_colors[0]);
	quad(two, three, seven, six, vertex_colors[1]);
	quad(three, zero, four, seven, vertex_colors[2]);
	quad(six, five, one, two, vertex_colors[3]);
	quad(four, five, six, seven, vertex_colors[4]);
	quad(five, four, zero, one, vertex_colors[5]);
	//lines 14 per small cube
	points[Index] = vertices[zero]; Index++;
	points[Index] = vertices[one]; Index++;
	points[Index] = vertices[one]; Index++;
	points[Index] = vertices[two]; Index++;
	points[Index] = vertices[two]; Index++;
	points[Index] = vertices[three]; Index++;
	points[Index] = vertices[three]; Index++;
	points[Index] = vertices[zero]; Index++;
	points[Index] = vertices[four]; Index++;
	points[Index] = vertices[five]; Index++;
	points[Index] = vertices[five]; Index++;
	points[Index] = vertices[six]; Index++;
	points[Index] = vertices[six]; Index++;
	points[Index] = vertices[seven]; Index++;
	points[Index] = vertices[seven]; Index++;
	points[Index] = vertices[four]; Index++;
	points[Index] = vertices[zero]; Index++;
	points[Index] = vertices[four]; Index++;
	points[Index] = vertices[one]; Index++;
	points[Index] = vertices[five]; Index++;
	points[Index] = vertices[two]; Index++;
	points[Index] = vertices[six]; Index++;
	points[Index] = vertices[three]; Index++;
	points[Index] = vertices[seven]; Index++;
}

void bigCube() {
	for (int k = 0; k<3; k++) {
		for (int j = 0; j<3; j++) {
			for (int i = 0; i<3; i++) {
				smallCube(0 + i + 4 * j + 16 * k, 4 + i + 4 * j + 16 * k, 5 + i + 4 * j + 16 * k, 1 + i + 4 * j + 16 * k, 16 + i + 4 * j + 16 * k, 20 + i + 4 * j + 16 * k, 21 + i + 4 * j + 16 * k, 17 + i + 4 * j + 16 * k);
			}
		}
	}
}

//----------------------------------------------------------------------------
//after mouse click
void updateCube() {
	faces[currentface] = mat3(faces[currentface][0][2], faces[currentface][1][2], faces[currentface][2][2], faces[currentface][0][1], faces[currentface][1][1], faces[currentface][2][1], faces[currentface][0][0], faces[currentface][1][0], faces[currentface][2][0]);
	//FOR FUTURE REFERENCE: MAKE SURE THE MODEL VIEW ROTATION AND THE ARRAY ROTATION OF A FACE ARE IN THE SAME DIRECTION. 
	if (currentface == 0 || currentface == 2) {
		int angle = 270;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int currentsmallface = faces[currentface][i][j];
				ModelViews[currentsmallface] = RotateZ(angle) * ModelViews[currentsmallface];
			}
		}
	}

	if (currentface == 1 || currentface == 3) {
		for (int i = 0; i < 3; i++) {
			int angle = 270;
			if (currentface == 1) angle = 90;
			if (currentface == 3) angle = 270;
			for (int j = 0; j < 3; j++) {
				int currentsmallface = faces[currentface][i][j];
				ModelViews[currentsmallface] = RotateX(angle) * ModelViews[currentsmallface];
			}
		}
	}

	if (currentface == 4 || currentface == 5) {
		for (int i = 0; i < 3; i++) {
			int angle = 270;
			if (currentface == 4) angle = 270;
			if (currentface == 5) angle = 90;

			for (int j = 0; j < 3; j++) {
				int currentsmallface = faces[currentface][i][j];
				ModelViews[currentsmallface] = RotateY(angle) * ModelViews[currentsmallface];
			}
		}
	}


	switch (currentface) {
	case 0:
		for (int i = 0; i < 3; i++) faces[1][i][0] = faces[currentface][2 - i][2];
		for (int i = 0; i < 3; i++) faces[3][i][2] = faces[currentface][i][0];
		for (int i = 0; i < 3; i++) faces[4][i][2] = faces[currentface][2][i];
		for (int i = 0; i < 3; i++) faces[5][i][2] = faces[currentface][0][2 - i];
		break;
	case 1:
		for (int i = 0; i < 3; i++) faces[0][i][2] = faces[currentface][2 - i][0];
		for (int i = 0; i < 3; i++) faces[2][i][0] = faces[currentface][i][2];
		for (int i = 0; i < 3; i++) faces[4][2][i] = faces[currentface][0][2 - i];
		for (int i = 0; i < 3; i++) faces[5][0][i] = faces[currentface][2][2 - i];
		break;
	case 2:
		for (int i = 0; i < 3; i++) faces[1][i][2] = faces[currentface][i][0];
		for (int i = 0; i < 3; i++) faces[3][i][0] = faces[currentface][2 - i][2];
		for (int i = 0; i < 3; i++) faces[4][i][0] = faces[currentface][0][2 - i];
		for (int i = 0; i < 3; i++) faces[5][i][0] = faces[currentface][2][i];
		break;
	case 3:
		for (int i = 0; i < 3; i++) faces[0][i][0] = faces[currentface][i][2];
		for (int i = 0; i < 3; i++) faces[2][i][2] = faces[currentface][2 - i][0];
		for (int i = 0; i < 3; i++) faces[4][0][i] = faces[currentface][2][i];
		for (int i = 0; i < 3; i++) faces[5][2][i] = faces[currentface][0][i];
		break;
	case 4:
		for (int i = 0; i < 3; i++) faces[0][2][i] = faces[currentface][i][2];
		for (int i = 0; i < 3; i++) faces[1][0][i] = faces[currentface][2][2 - i];
		for (int i = 0; i < 3; i++) faces[2][0][i] = faces[currentface][2 - i][0];
		for (int i = 0; i < 3; i++) faces[3][2][i] = faces[currentface][0][i];
		break;
	case 5:
		for (int i = 0; i < 3; i++) faces[0][0][i] = faces[currentface][2 - i][2];
		for (int i = 0; i < 3; i++) faces[1][2][i] = faces[currentface][0][2 - i];
		for (int i = 0; i < 3; i++) faces[2][2][i] = faces[currentface][i][0];
		for (int i = 0; i < 3; i++) faces[3][0][i] = faces[currentface][2][i];
		break;
	}
	glutPostRedisplay();
}

void randomize() {
	for (int i = 0; i<5; i++) {
		currentface = rand() % 6;
		updateCube();
	}

}

// OpenGL initialization
void
init()
{
	bigCube();
	randomize();
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	// Set current program object
	glUseProgram(program);

	// Set projection matrix
	mat4  projection;
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);
	glClearStencil(0);
	// Set state variable "clear color" to clear buffer with.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	printf("NOTE: Please click only on the central cube of a face when wishing to rotate the face. \n That is how the cube is set up. Thanks! :) \n Press H for help.");


}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//  Generate tha model-view matrix
	const vec3 displacement(0.0, 0.0, 0.0);
	mat4  model_view = (Scale(1.0, 1.0, 1.0) * Translate(displacement) *
		RotateX(Theta[Xaxis]) *
		RotateY(Theta[Yaxis]) *
		RotateZ(Theta[Zaxis]));  // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	for (int facecount = 0; facecount < 6; facecount++) {
		glStencilFunc(GL_ALWAYS, facecount, -1);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int cube = faces[facecount][i][j];
				glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view*ModelViews[cube]);
				glDrawArrays(GL_TRIANGLES, cube * 36 + cube * 24, 36);
				glDrawArrays(GL_LINES, 36 + cube * (36 + 24), 24);
			}
		}
	}
	glDisable(GL_STENCIL_TEST);
	glutSwapBuffers();

}

//for rotating the whole cube more than 360 degrees
void
idle(void)
{
	if (Theta[0] > 360.0) {
		Theta[0] -= 360.0;
	}
	else if (Theta[0] < -360.0) { Theta[0] += 360.0; }
	if (Theta[1] > 360.0) {
		Theta[1] -= 360.0;
	}
	else if (Theta[1] < -360.0) { Theta[1] += 360.0; }
	if (Theta[2] > 360.0) {
		Theta[2] -= 360.0;
	}
	else if (Theta[2] < -360.0) { Theta[2] += 360.0; }

	glutPostRedisplay();
}

void
keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' | key == 'q')
		exit(0);
	if (key == 'H' | key == 'h') {
		printf(" \n Press right arrow to rotate the object to the right. \n Press lift arrow to rotate the object to the left. \n Press up arrow to rotate the objectup. \n Press down arrow to rotate the object down. \n Left click on the CENTER of a face to rotate it.  \n");
	}
}

//Left Click on the center of a face to rotate it clockwise.
void mouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		y = glutGet(GLUT_WINDOW_HEIGHT) - y;
		glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &currentface);
		update = true;
		updateCube();
	}
	glutPostRedisplay();

}

// Rotate the big cube to view from different angles
void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		Theta[0] -= 3.0;
		break;
	case GLUT_KEY_DOWN:
		Theta[0] += 3.0;
		break;
	case GLUT_KEY_LEFT:
		Theta[1] -= 3.0;
		break;
	case GLUT_KEY_RIGHT:
		Theta[1] += 3.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / GLfloat(h);
	mat4  reshapematrix;
	if (w <= h) {
		reshapematrix = Ortho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
	}
	if (w>h) {
		reshapematrix = Ortho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
	}
	glUniformMatrix4fv(Projection, 1, GL_TRUE, reshapematrix);

}

int
main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(517, 517);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}







