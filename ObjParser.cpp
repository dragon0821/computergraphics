#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string.h>
#include "ObjParser.h"
#include "bmpfuncs.h"


#define M_PI 3.1415926535897
using std::cos;
using std::sin;
using std::sqrt;

// global variable for counting fps
volatile int frame = 0, time, timebase = 0;
volatile float fps;

/* texture mapping set variable */
GLuint textureMonkey;


bool antialiase_on = true;
double radius = 10;
double theta = 45, phi = 45;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };

 //object var
ObjParser* dp;
ObjParser* bear;
ObjParser* cider;
ObjParser* coke;
ObjParser* present1;
ObjParser* present2;
ObjParser* present3;

// user-defined function
void init(void);
void light_default();
void add_menu();
void get_resource(const char* str);
void mouse(int, int, int, int);
void mouseWheel(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);
void keyboard(unsigned char, int, int);
void special_keyboard(int, int, int);
void draw(void);
void resize(int, int);
//...

/* Main method */
int main(int argc, char** argv)
{
	// glut initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(250, 250);
	glutCreateWindow("GLUT Test");	// ���� ���� ������ ����

	// ����� �ʱ�ȭ �Լ�
	init();

	// pop-up �޴� ��� �Լ�
	add_menu();

	// ���ҽ� �ε� �Լ�
	//get_resource("img/obj/bear2.obj");
	bear = new ObjParser("img/obj/bear2.obj");
	dp = new ObjParser("img/obj/displayrack_3.obj");
	cider = new ObjParser("img/obj/Cider.obj");
	coke = new ObjParser("img/obj/Cider.obj");
	present1 = new ObjParser("img/obj/Present.obj");
	present2 = new ObjParser("img/obj/Present.obj");
	present3 = new ObjParser("img/obj/Present.obj");

	/* Create a single window with a keyboard and display callback */
	glutMouseFunc(&mouse);
	glutMouseWheelFunc(&mouseWheel);
	glutMotionFunc(&motion);
	glutPassiveMotionFunc(&passiveMotion);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&special_keyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);

	//glutIdleFunc(&idle);

	/* Run the GLUT event loop */
	glutMainLoop();

	return EXIT_SUCCESS;
}

void light_default() {
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	/* Light0 ���� ���� ���� */
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.9f, 1.0f };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/* Light1 ���� ���� ���� */
	GLfloat ambientLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuseLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light_position1[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	/************* spot position setting *************/
	/*GLfloat spot_direction[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/

	GLfloat specularMaterial[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/************* Material  setting *************/
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);


	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	/* DEPTH TEST ENABLE */
	glFrontFace(GL_CW);	// CW CCW�ٲ㺸�鼭 front face �����غ���!
}

void setTextureMapping(char buf[100]) {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData(buf, &imgWidth, &imgHeight, &channels);

	int texNum = 1;
	glGenTextures(texNum, &textureMonkey);
	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT ���� �ϳ� ����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}

void init()
{
	printf("init func called\n");
	// clear background color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0, 500, 500, 0);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// set antialiasing
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_LINE_SMOOTH);

	light_default();

	/* TEXTURE MAPPING SET */

	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_REPLACE : polygon�� ���� ������ �����ϰ� texture�� ����
}

void special_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) {
		phi -= 5;
		if (phi < 0) phi = 355;
	}
	else if (key == GLUT_KEY_RIGHT) {
		phi += 5;
		if (phi >= 360) phi = 0;
	}
	else if (key == GLUT_KEY_UP) {
		if (theta > 10) theta -= 5;
	}
	else if (key == GLUT_KEY_DOWN) {
		if (theta < 170) theta += 5;
	}

	std::cout << "theta : " << theta << ", phi : " << phi << "\n";
	glutPostRedisplay();
}

/* Keyboard callback function */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		/* Exit on escape key press */
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	case 'a':
	{
		if (antialiase_on) {
			antialiase_on = false;
			glDisable(GL_POLYGON_SMOOTH);
		}
		else
		{
			antialiase_on = true;
			glEnable(GL_POLYGON_SMOOTH);
		}
		break;
	}
	}
	glutPostRedisplay();
}

void draw_axis(void)
{
	glLineWidth(1.5f);
	glBegin(GL_LINES);

	glColor4f(1.f, 0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(4.f, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 4.f, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 4.f);

	glEnd();
	glLineWidth(1);
}

void draw_obj(ObjParser* objParser)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}

void draw_obj_with_texture(ObjParser* objParser)
{
	if (objParser == dp)
	{
		char buf[] = "img/obj/displayrack_3.bmp";
		setTextureMapping(buf);
	}
	if (objParser == bear)
	{
		char buf[] = "img/obj/bear2.bmp";
		setTextureMapping(buf);
	}
	if (objParser == cider)
	{
		char buf[] = "img/obj/Cider.bmp";
		setTextureMapping(buf);
	}
	if (objParser == coke)
	{
		char buf[] = "img/obj/Coke.bmp";
		setTextureMapping(buf);
	}
	if (objParser == present1)
	{
		char buf[] = "img/obj/present1.bmp";
		setTextureMapping(buf);
	}
	if (objParser == present2)
	{
		char buf[] = "img/obj/present2.bmp";
		setTextureMapping(buf);
	}
	if (objParser == present3)
	{
		char buf[] = "img/obj/present3.bmp";
		setTextureMapping(buf);
	}
	glDisable(GL_BLEND);
	// glEnable(GL_TEXTURE_2D);	// texture �� ������ ���� enable
	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}




/* Display callback function */
void draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
	cam[1] = radius * cos(theta * M_PI / 180);
	cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
	glDisable(GL_LIGHT1);
	
	gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	//glPopMatrix();
	
	glColor3f(1.f, 1.f, 1.f);
	glScalef(2, 2, 2);
	draw_obj_with_texture(dp);//(0,0,0)

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -0.17, 0);
	glScalef(0.6, 0.6, 0.6);
	glRotatef(90, 1, 0, 0);
	draw_obj_with_texture(present1);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -1.7, 0);
	glScalef(0.5, 0.5, 0.5);
	draw_obj_with_texture(cider);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -3.2, 0);
	glScalef(0.05, 0.04, 0.05);
	glRotatef(180, 0, 0, 1);
	glRotatef(180, 1, 0, 0);
	draw_obj_with_texture(bear);
	
	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -0.17, -2.85);
	glScalef(0.6, 0.6, 0.6);
	glRotatef(90, 1, 0, 0);
	draw_obj_with_texture(present2);
	
	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -1.7, -2.85);
	glScalef(0.5, 0.5, 0.5);
	draw_obj_with_texture(coke);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, -3.2, -2.85);
	glScalef(0.6, 0.6, 0.6);
	glRotatef(90, 1, 0, 0);
	draw_obj_with_texture(present3);
	//draw_obj_with_texture(present1);


	glutSwapBuffers();
	glFlush();
}

void mouse(int button, int state, int x, int y)
{
	if (state) {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	else {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius > 1) radius -= 0.5;
	}
	else {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius < 100) radius += 0.5;
	}
	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void main_menu(int option)
{
	if (option == 99) exit(0);
	else if (option == 1) {
		radius = 10;
		theta = 45; phi = 45;
	}
	glutPostRedisplay();
}

void sub_menu(int option)
{
	printf("Submenu %d has been selected\n", option);
}

void add_menu()
{
	int mainmenu1 = glutCreateMenu(&main_menu);
	glutAddMenuEntry("Init", 1);
	glutAddMenuEntry("Quit", 99);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void idle()
{
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME); // unit : ms

	if (time - timebase > 1000)
	{
		fps = frame * 1000.f / (time - timebase);
		timebase = time;
		frame = 0;
		//printf("fps : %.0f\n", fps);
	}
}

void resize(int width, int height)
{
	printf("resize func called\n");
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 500, 0, 500);
	gluPerspective(60, (double)width / (double)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void get_resource(const char* str)
{
	//monkey = new ObjParser(str);
}



