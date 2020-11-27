#include <gl/glut.h>
#include <iostream>
#include <windows.h>
#include <vector>
#include <math.h>
#include "GL/glext.h"
#include "GL/glcorearb.h"
#include "bmpfuncs.h"

#define M_PI 3.141592
#define CURRENT_WIDTH 500
#define CURRENT_HEIGHT 500
#define DIST 10
#define RADIAN_TRANSF M_PI/180

using namespace std;

bool left_state = false;

double disdif = 0.0;
double viewX = 0.0;
double viewY = 0.0;
double viewZ = 0.0;
double theta = 0.0;
double radianpi = 0.0;
double angleUnit = 2 * (M_PI / 180);
double g_nX, g_nY;

GLuint texName[6];
GLuint g_nCubeTex;

void init(void);
void resize(int width, int height);
void camera_eye(void);
void draw(void);
void draw_skybox(float size);
void draw_sphere(float size);
double Determinant(GLfloat** a, int n);
void gluInvertMatrix(GLfloat** a, GLfloat** b);
void CoFactor(GLfloat** a, int num, GLfloat** b);
void Transpose(GLfloat** b, int num);
void div_det(GLfloat** b, int num, double det);
void keyboard(unsigned char key, int x, int y);
void specialkey(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);


int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(CURRENT_WIDTH, CURRENT_HEIGHT);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("MainWidow");
    init();

    glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkey);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    return 0;
}

void init(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);

    GLfloat light_ambient[] = { 0.25f,0.25f,0.25f,1.0f };
    GLfloat light_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glGenTextures(1, &g_nCubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
    int imgWidth[6], imgHeight[6], channels[6];

    uchar* img0 = readImageData("img/environment/pattern.bmp", &imgWidth[0], &imgHeight[0], &channels[0]);
    uchar* img1 = readImageData("img/environment/pattern.bmp", &imgWidth[1], &imgHeight[1], &channels[1]);
    uchar* img2 = readImageData("img/environment/pattern.bmp", &imgWidth[2], &imgHeight[2], &channels[2]);
    uchar* img3 = readImageData("img/environment/wood.bmp", &imgWidth[3], &imgHeight[3], &channels[3]);
    uchar* img4 = readImageData("img/environment/game.bmp", &imgWidth[4], &imgHeight[4], &channels[4]);
    uchar* img5 = readImageData("img/environment/game.bmp", &imgWidth[5], &imgHeight[5], &channels[5]);



    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth[0], imgHeight[0], 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth[1], imgHeight[1], 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth[2], imgHeight[2], 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth[3], imgHeight[3], 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth[4], imgHeight[4], 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth[5], imgHeight[5], 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

    camera_eye();

    glLoadIdentity();
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)width / (float)height, 1, 500);
    glMatrixMode(GL_MODELVIEW);
}

void camera_eye() {
    if (theta > 2 * M_PI) theta -= 2 * M_PI;
    if (theta <= 0) theta += 2 * M_PI;
    if (radianpi > 2 * M_PI) radianpi -= 2 * M_PI;
    if (radianpi <= 0) radianpi += 2 * M_PI;
    viewX = (DIST + disdif) * cos(theta) * cos(radianpi);
    viewY = (DIST + disdif) * sin(theta);
    viewZ = (DIST + disdif) * cos(theta) * sin(radianpi);
}


void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    if ((0 <= theta && theta < M_PI / 2) || (3 * M_PI / 2 < theta && theta <= 2 * M_PI)) {
        gluLookAt(viewX, viewY, viewZ, 0, 0, 0, 0, 1, 0);
    }
    else {
        gluLookAt(viewX, viewY, viewZ, 0, 0, 0, 0, -1, 0);
    }

    draw_skybox(60);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();  //현재의 texture 상태를 저장

    GLfloat camera[16];
    GLfloat inv_camera[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, camera);
    camera[14] = 0;

    GLfloat** camera_4x4, ** inv_camera_4x4;
    camera_4x4 = new GLfloat * [4];
    inv_camera_4x4 = new GLfloat * [4];
    for (int i = 0; i < 4; i++) {
        camera_4x4[i] = new GLfloat[4];
        inv_camera_4x4[i] = new GLfloat[4];
    }

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            camera_4x4[row][col] = camera[row * 4 + col];
        }
    } //역행렬 구하기

    gluInvertMatrix(camera_4x4, inv_camera_4x4);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            inv_camera[row * 4 + col] = inv_camera_4x4[row][col];
        }
    }

    glMultMatrixf(inv_camera);

    glMatrixMode(GL_MODELVIEW);
    draw_sphere(1);
    glMatrixMode(GL_TEXTURE);

    glPopMatrix();  //역행렬 곱해주기 전의 상태로 돌아간다.
    glMatrixMode(GL_MODELVIEW);  //모델 뷰 상태로 되돌려 놓음

    glFlush();
    glutSwapBuffers();
}

void draw_skybox(float size) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);

    glBegin(GL_QUADS);
    glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
    glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
    glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
    glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
    glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-size, -size, size);
    glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
    glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
    glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
    glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
    glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
    glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-size, -size, size);
    glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
    glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord3d(-1.0, -1.0, +1.0); glVertex3f(-size, -size, size);
    glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-size, size, size);
    glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(size, size, size);
    glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(size, -size, size);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-size, -size, -size);
    glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-size, size, -size);
    glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(size, size, -size);
    glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(size, -size, -size);
    glEnd();

    glEnable(GL_LIGHTING);
}

void draw_sphere(float size) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);

    glutSolidSphere(size, 30, 30);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

double Determinant(GLfloat** a, int n) {
    int i, j, j1, j2;
    double det = 0;
    GLfloat** m = NULL;

    if (n == 2) {
        det = a[0][0] * a[1][1] - a[1][0] * a[0][1];
    }
    else {
        det = 0;
        for (j1 = 0; j1 < n; j1++) {
            m = new GLfloat * [n - 1];
            for (i = 0; i < n - 1; i++) {
                m[i] = new GLfloat[n - 1];
            }
            for (i = 1; i < n; i++) {
                j2 = 0;
                for (j = 0; j < n; j++) {
                    if (j == j1)
                        continue;
                    m[i - 1][j2] = a[i][j];
                    j2++;
                }
            }
            det += pow(-1.0, j1 + 2.0) * a[0][j1] * Determinant(m, n - 1);
            for (i = 0; i < n - 1; i++)
                delete[] m[i];
            delete[] m;
        }
    }
    return(det);
}// 4차원 행렬을 구하므로 n=4 를 대입

void gluInvertMatrix(GLfloat** a, GLfloat** b) { //시선행렬의 역행렬을 구해준다.
    double det = Determinant(a, 4);
    CoFactor(a, 4, b);  //여인수(cofactor 구하는 함수(직접구현)
    //    Transpose(b,4);  //전치행렬 구하는 함수(직접 구현)
    div_det(b, 4, det); //역행렬 구하는 함수(직접 구현)
}

void CoFactor(GLfloat** a, int num, GLfloat** b) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            b[i][j] = a[i][j];
        }
    }
    b[0][0] = a[1][1] * a[2][2] - a[1][2] * a[2][1];
    b[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
    b[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
    b[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
    b[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
    b[1][2] = a[1][0] * a[0][2] - a[0][0] * a[1][2];
    b[2][0] = a[1][0] * a[2][1] - a[1][1] * a[2][0];
    b[2][1] = a[0][1] * a[2][0] - a[0][0] * a[2][1];
    b[2][2] = a[0][0] * a[1][1] - a[1][0] * a[0][1];
}

void Transpose(GLfloat** b, int num) {
    GLfloat** temp = new GLfloat * [num];
    for (int i = 0; i < num; i++) {
        temp[i] = new GLfloat[num];
    }

    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            temp[i][j] = b[j][i];
        }
    }
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            b[i][j] = temp[i][j];
        }
    }
}
void div_det(GLfloat** b, int num, double det) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            b[i][j] = b[i][j] / det;
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'I':
        disdif -= 1.0;
        break;
    case 'i':
        disdif -= 1.0;
        break;
    case 'O':
        disdif += 1.0;
        break;
    case 'o':
        disdif += 1.0;
        break;
    default:
        break;
    }
    camera_eye();
    glutPostRedisplay();
}

void specialkey(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        radianpi += angleUnit;
        break;
    case GLUT_KEY_RIGHT:
        radianpi -= angleUnit;
        break;
    case GLUT_KEY_UP:
        theta += angleUnit;
        break;
    case GLUT_KEY_DOWN:
        theta -= angleUnit;
        break;
    default:
        break;
    }
    camera_eye();
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    y = CURRENT_HEIGHT - y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        g_nX = x;
        g_nY = y;
        left_state = true;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        left_state = false;
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    y = CURRENT_HEIGHT - y;
    if (left_state) {
        radianpi += (x - g_nX) * RADIAN_TRANSF;
        theta -= (y - g_nY) * RADIAN_TRANSF;
        g_nX = x;
        g_nY = y;
        camera_eye();
    }
    glutPostRedisplay();
}
