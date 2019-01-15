#include "Angel.h"
#include <conio.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
//vectors, points, texture, etc.
point4* points;
point4* vertices;
vec3* normalvectors;
vec3* faces;
vec3* normals;
vec2* textures;
vec2* texture_coordinates;
GLubyte* image;
GLuint texture[1];
//Angles for rotations
enum{	Xaxis = 0, Yaxis = 1,Zaxis = 2,NumAxes = 3};
int Axis = Xaxis;
GLfloat Theta[NumAxes] = { 10.0, 10.0, 10.0 };
GLfloat initialTheta[NumAxes] = { 10.0, 10.0, 10.0 };
//
int  NumFaces, NumPoints;
float width, height = 0.0;
int projection = 1, displaytype=1, lightMovement = 1, reflection = 1, shading = 1;
GLfloat scaleFactor = 0.9;
//viewer_pos and light positions. (initial light position is needed to find the original light position after switching from moving lights to fix lights)
vec3 viewer_pos(0.0, 0.0, 0.0);
point4 light1_position(-1.0, 0.0, 1.0, 1.0);		// Point light source
point4 light1_initial_position = light1_position;  //for switching btw fix and moving lights
point4 light2_position(1.0, 0.0, 1.0, 0.0);		// Directional light source
point4 light2_initial_position = light2_position;//for switching btw fix and moving lights
bool pointSource, directionalSource = false;

// Parse the Off x file
void parseOffx(const char* filename) {
	ifstream fin(filename);
	//disregard the first line
	char buf[40];
	fin.getline(buf,40, '\n');
	fin.getline(buf, 40, '\n');
	char* token = strtok(buf, " \t");
	int NumVertices = strtof(token, NULL);
	NumFaces = strtof(strtok(NULL, " \t"), NULL);
	textures = (vec2*)malloc(NumVertices * sizeof(vec2));
	texture_coordinates = (vec2*)malloc(NumFaces * 3 * sizeof(vec2));
	faces = (vec3*)malloc(NumFaces * sizeof(vec3));
	normalvectors = (vec3*)malloc(NumVertices * sizeof(vec3));
	normals = (vec3*)malloc(NumFaces * 3 * sizeof(vec3));
	points = (point4*)malloc(NumFaces * 3 * sizeof(point4));
	vertices = (point4*)malloc(NumVertices * sizeof(point4));
	int vertexcount = 0;
	for (int i = 0; i < NumVertices; i++) {  //vertices
		char buf[40];
		fin.getline(buf, 40, '\n');
		char* token = strtok(buf, " \t");
		float n1 = strtof(token, NULL);
		float n2 = strtof(strtok(NULL, " \t"), NULL);
		float n3 = strtof(strtok(NULL, " \t"), NULL);
		vertices[vertexcount++] = point4(n1, n2, n3, 1.0);
}
	int facescount = 0;
	for (int i = 0; i < NumFaces; i++) { //faces starts with 3
		char buf[40];
		fin.getline(buf, 40, '\n');
		char* token = strtok(buf, " \t");
		float n1 = strtof(strtok(NULL, " \t"), NULL);
		float n2 = strtof(strtok(NULL, " \t"), NULL);
		float n3 = strtof(strtok(NULL, " \t"), NULL);
		faces[facescount++] = vec3(n1, n2, n3);
	}
	int  texturecount = 0;
	for (int i = 0; i < NumVertices; i++) {//textures start with vt
		char buf[40];
		fin.getline(buf, 40, '\n');
		char* token = strtok(buf, " \t");
		float n1 = strtof(strtok(NULL, " \t"), NULL);
		float n2 = strtof(strtok(NULL, " \t"), NULL);
		textures[texturecount++] = vec2(n1, 1 - n2);
	}
	int  normalcount = 0;
	for (int i = 0; i < NumVertices; i++) {//normalvectors start with vn
		char buf[40];
		fin.getline(buf, 40, '\n');
		char* token = strtok(buf, " \t");
		float n1 = strtof(strtok(NULL, " \t"), NULL);
		float n2 = strtof(strtok(NULL, " \t"), NULL);
		float n3 = strtof(strtok(NULL, " \t"), NULL);
		normalvectors[normalcount++] = vec3(n1, n2, n3);
	}
	
}
//I used the method listed on the course website for paring the ppm file.
void parsePPM(const char* filename) {
	FILE *fd;
	int n, m, k;
	char c, b[100];
	int red, green, blue;
	fd = fopen(filename, "r");
	fscanf(fd, "%[^\n] ", b);
	if (b[0] != 'P' || b[1] != '3') {
		printf("%s is not a PPM file!\n", b);
		return;
	}
	fscanf(fd, "%c", &c);
	while (c == '#') {
		fscanf(fd, "%[^\n] ", b);
		fscanf(fd, "%c", &c);
	}
	ungetc(c, fd);
	fscanf(fd, "%d %d %d", &n, &m, &k);
	image = (GLubyte*)malloc(n*m * 3 * sizeof(GLubyte));
	for (unsigned int i = n * m; i > 0; i--) {
		fscanf(fd, "%d %d %d", &red, &green, &blue);
		image[3 * n * m - 3 * i] = red;
		image[3 * n * m - 3 * i + 1] = green;
		image[3 * n * m - 3 * i + 2] = blue;
	}
	//get the textures and save them to texture to use in the fragment shader.
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, n, m, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}
//smallest polygon triangle is the building unit.
void triangle(int a, int b, int c) {
	normals[NumPoints] = normalvectors[a]; texture_coordinates[NumPoints] = textures[a];  points[NumPoints++] = vertices[a];
	normals[NumPoints] = normalvectors[b]; texture_coordinates[NumPoints] = textures[b];  points[NumPoints++] = vertices[b];
	normals[NumPoints] = normalvectors[c]; texture_coordinates[NumPoints] = textures[c];  points[NumPoints++] = vertices[c];
}
void picture() {
	parseOffx("shapeX.txt");
	parsePPM("texture.ppm");
	for (int i = 0; i<NumFaces; i++) {
		triangle(faces[i].x, faces[i].y, faces[i].z);
	}
}
//----------------------------------------------------------------------------
//to be transffered to the shader
GLuint AmbientProduct, DiffuseProduct,ModelView, Projection, PointSource, DirectionalSource, DisplayType, Shininess,  Light1Position, Light2Position,SpecularProduct, LightMovement, Shading, Reflection;
void
init()
{	picture();
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR); //try here different alternatives
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //try here different alternatives
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, NumFaces * 3 * (sizeof(vec3) + sizeof(point4)+  sizeof(vec2)), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, NumFaces * 3 * sizeof(point4), points);
	glBufferSubData(GL_ARRAY_BUFFER, NumFaces * 3 * sizeof(point4), NumFaces * 3 * sizeof(vec3), normals);
	glBufferSubData(GL_ARRAY_BUFFER,NumFaces * 3 * (sizeof(vec3) +  sizeof(point4)), NumFaces * 3 * sizeof(vec2), texture_coordinates);
    //shaders
	GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,	BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,	BUFFER_OFFSET(NumFaces * 3 * sizeof(point4)));

	GLuint vTextureCoordinates = glGetAttribLocation(program, "vTextureCoordinates");
	glEnableVertexAttribArray(vTextureCoordinates);
	glVertexAttribPointer(vTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(NumFaces * 3 * (sizeof(point4)+ sizeof(vec3)))  );

	// Initialize shader lighting parameters
	point4 light_position(-1.0, 0.0, 0.0, 1.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0); // L_a
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0); // L_d
	color4 light_specular(1.0, 1.0, 1.0, 1.0); // L_s

	color4 material_ambient(1.0, 0.0, 1.0, 1.0); // k_a
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0); // k_d
	color4 material_specular(1.0, 0.8, 0.0, 1.0); // k_s

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;
	
	//shader communication
	Light1Position = glGetUniformLocation(program, "Light1Position");
	Light2Position = glGetUniformLocation(program, "Light2Position");
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	PointSource = glGetUniformLocation(program, "pointSource");
	DirectionalSource = glGetUniformLocation(program, "directionalSource");
	DisplayType = glGetUniformLocation(program, "displayType");
	Shininess = glGetUniformLocation(program, "Shininess");
	Shading = glGetUniformLocation(program, "shading");
	Reflection = glGetUniformLocation(program, "reflection");

	glUniform4fv(Light1Position, 1, light1_position);
	glUniform4fv(Light2Position, 1, light2_position);
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform1i(PointSource, pointSource);
	glUniform1i(DirectionalSource, directionalSource);
	glUniform1i(DisplayType, displaytype);
	glUniform1f(Shininess, 4.0);
	glUniform1i(Shading, shading);
	glUniform1i(Reflection, reflection);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
void
display(void)
{	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	//  Generate tha model view matrix
	mat4 model_view = (Scale(scaleFactor, scaleFactor, scaleFactor) * Translate( - viewer_pos) *RotateX(Theta[0]) *RotateY(Theta[1]) * RotateZ(Theta[2]));
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	if (lightMovement == 2) {
		light1_position = model_view * light1_initial_position;
		light2_position = model_view * light1_initial_position;
		glUniform4fv(Light1Position, 1, light1_position);
		glUniform4fv(Light2Position, 1, light2_position);
	}
	if(displaytype==1){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		Theta[Xaxis] -= 2.0;
		break;
	case GLUT_KEY_DOWN:
		Theta[Xaxis] += 2.0;
		break;
	case GLUT_KEY_LEFT:
		Theta[Yaxis] -= 2.0;
		break;
	case GLUT_KEY_RIGHT:
		Theta[Yaxis] += 2.0;
		break;
	default:
		break;
	}
}

void
idle(void)
{	if (Theta[0] > 360.0) {
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

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'i':
		for(int i=0;i<3;i++){ 
			Theta[i] = initialTheta[i];
		}
		break;
	case 'z':
		if (projection == 1) scaleFactor *= 1.1;
		else viewer_pos -= vec3(0.0, 0.0, 0.1);
		break;
	case 'Z':
		if (projection == 1) scaleFactor *= 0.9;
		else viewer_pos += vec3(0.0, 0.0, 0.1);
		break;
	case 'h':printf("Welcome!\n Press i to initialize position. \n Press the Z and z to zoom in and out. \n Press t and T to rotate and the Z axis\n Use the arrow keys to rotate around the X and Y axes.\n Right click to see the options menu. You can change the reflection, shading, texture, background color, display type and many more!\nDefault Settings: \nDisplay: Wireframe \nLights: Both Lights off \nShading: Gouraud \nReflection: Phong  \nLight Position:Fixed, Material:Plastic, \nShininess:4  \nProjection: Orthographic \nBackground: White");
	///////////////help screen
		break;
	case 't':
		Theta[Zaxis] -= 2.0;
		break;
	case 'T':
		Theta[Zaxis] += 2.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
//----------------------------------------------------------------------------

void reshape(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;
	mat4 projectionmatrix;
	switch (projection) {
	case 1:
		viewer_pos = vec3(0.0, 0.0, 0.0);
		if (w <= h)
			projectionmatrix = Ortho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
		else
			projectionmatrix = Ortho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
		break;
	case 2:
		viewer_pos = vec3(0.0, 0.0, 2.6);
		projectionmatrix = Perspective(45.0, aspect, 0.5, 5.0);
		break;
	default:
		break;
	}
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projectionmatrix);
}
//Menu items options
static int big_menu, shading_menu,  reflection_menu, light_turn_menu, light_position_menu, material_menu, projection_menu, display_menu, background_color_menu;

void menu(int selection) {
	switch (selection) {
	case 1:
		shading = 1;
		glUniform1i(Shading, shading);
		break;
	case 2:
		shading = 2;
		glUniform1i(Shading, shading);
		break;
	case 3:
		reflection = 1;
		glUniform1i(Reflection, reflection);
		break;
	case 4:
		reflection = 2;
		glUniform1i(Reflection, reflection);
		break;
	case 5:
		pointSource = !pointSource;
		glUniform1i(PointSource, pointSource);
		break;
	case 6:
		directionalSource = !directionalSource;
		glUniform1i(DirectionalSource, directionalSource);
		break;
	case 7:
		lightMovement = 1;
		glUniform4fv(Light1Position, 1, light1_initial_position);
		glUniform4fv(Light2Position, 1, light2_initial_position);
		break;
	case 8:
		lightMovement = 2;
		break;
	case 9:
		glUniform1f(Shininess, 4.0);
		break;
	case 10:
		glUniform1f(Shininess, 4000.0);
		break;
	case 11:
		projection = 1;
		reshape(width, height);
		break;
	case 12:
		projection = 2;
		reshape(width, height);
		break;
	case 13:
		displaytype = 1;
		glUniform1i(DisplayType, displaytype);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 14:
		displaytype = 2;
		glUniform1i(DisplayType, displaytype);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 15:
		displaytype = 3;
		glUniform1i(DisplayType, displaytype);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 16:
		glClearColor(1.0, 1.0, 1.0, 1.0);
		break;
	case 17:
		glClearColor(0.0, 0.0, 0.0, 1.0);
		break;
	case 18:
		glClearColor(0.5, 0.5, 0.5, 1.0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
//menu gui
void create_menu(void){
	shading_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Gouraud", 1);
	glutAddMenuEntry("Phong", 2);
	reflection_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Phong", 3);
	glutAddMenuEntry("Modified Phong", 4);
	light_turn_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Point", 5);
	glutAddMenuEntry("Directional", 6);
	light_position_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Fixed", 7);
	glutAddMenuEntry("Moving", 8);
	material_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Plastic", 9);
	glutAddMenuEntry("Metallic", 10);
	projection_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Ortographic", 11);
	glutAddMenuEntry("Perspective", 12);
	display_menu = glutCreateMenu(menu);
	glutAddMenuEntry("Wireframe", 13);
	glutAddMenuEntry("Shading", 14);
	glutAddMenuEntry("Texture", 15);
	background_color_menu = glutCreateMenu(menu);
	glutAddMenuEntry("White", 16);
	glutAddMenuEntry("Black", 17);
	glutAddMenuEntry("Gray", 18);
	big_menu = glutCreateMenu(menu);
	glutAddSubMenu("Shading", shading_menu);
	glutAddSubMenu("Reflection", reflection_menu);
	glutAddSubMenu("Light turn on/off", light_turn_menu);
	glutAddSubMenu("Light position", light_position_menu);
	glutAddSubMenu("Material", material_menu);
	glutAddSubMenu("Projection", projection_menu);
	glutAddSubMenu("Display", display_menu);
	glutAddSubMenu("Background color", background_color_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
int main(int argc, char **argv)
{	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Shading and Texturing");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	create_menu();
	glutMainLoop();
	return 0;
}