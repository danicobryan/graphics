//
//  main.cpp
//  Cartoonify
//
//  Created by Danico Pidlaoan on 1/24/18.
//  Copyright © 2018 Danico Pidlaoan. All rights reserved.
//

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

using namespace std;

void setup() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set our color to black (R, G, B)
    glColor3f(0.0f, 0.0f, 0.0f);
    // Draw 4 points with coordinates (x, y) counter-clockwise
    // (0.5, 0.5), (-0.5, 0.5), (-0.5f, -0.5f) (0.5f, -0.5f)
    // Note if we want clockwise then add: glFrontFace(GL_CW)
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
    // Draw 2 lines from
    // (0.5, 0.5) to (-0.5, 0.5) and (-0.5f, -0.5f) to (0.5f, -0.5f)
    glBegin(GL_LINES);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
    glutSwapBuffers();
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800,600);
    glutCreateWindow("Points and Lines");
    setup();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
