#pragma once
#pragma warning (disable : 4996)
#include <iostream>
#include <vector>
#include <string>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include "GL/glext.h"
#include "GL/glcorearb.h"
#include "bmpfuncs.h"

class environment {
public:
    GLuint texName[6];
    GLuint g_nCubeTex;

	void envset() {
        glGenTextures(1, &g_nCubeTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
        int imgWidth[6], imgHeight[6], channels[6];

        uchar* img0 = readImageData("img/environment/pattern.bmp", &imgWidth[0], &imgHeight[0], &channels[0]);
        uchar* img1 = readImageData("img/environment/grass.bmp", &imgWidth[1], &imgHeight[1], &channels[1]);
        uchar* img2 = readImageData("img/environment/grass.bmp", &imgWidth[2], &imgHeight[2], &channels[2]);
        uchar* img3 = readImageData("img/environment/wood.bmp", &imgWidth[3], &imgHeight[3], &channels[3]);
        uchar* img4 = readImageData("img/environment/grass.bmp", &imgWidth[4], &imgHeight[4], &channels[4]);
        uchar* img5 = readImageData("img/environment/grass.bmp", &imgWidth[5], &imgHeight[5], &channels[5]);



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

        glLoadIdentity();
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

        glDisable(GL_TEXTURE_CUBE_MAP);
        glEnable(GL_LIGHTING);
    }
};
