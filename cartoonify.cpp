//
//  main.cpp
//  OpenGL Test
//
//  Created by Danico Pidlaoan on 1/17/18.
//  Copyright © 2018 Danico Pidlaoan. All rights reserved.
//

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <vector>

using namespace std;


void setup() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

class Vertex {
    GLfloat x, y;
    public:
    Vertex(GLfloat, GLfloat);
    GLfloat get_y() { return y; };
    GLfloat get_x() { return x; };
};

Vertex::Vertex(GLfloat X, GLfloat Y) {
    x = X;
    y = Y;
}


vector<Vertex> get_midpoints(vector<Vertex> control_points) {
    vector<Vertex> midpts;
    for (int p = 0; p < control_points.size()-1; p++) {
        Vertex v_0 = control_points[p];
        Vertex v_1 = control_points[p+1];
        GLfloat x1 = (v_0.get_x()+v_1.get_x())/2;
        GLfloat y1 = (v_0.get_y()+v_1.get_y())/2;
        midpts.push_back(Vertex(x1, y1));
    }
    return midpts;
}

vector<Vertex> generate_points(vector<Vertex> control_points) {
    vector<Vertex> points;
    vector<Vertex> midpoints = get_midpoints(control_points);
    vector<Vertex> mid_of_midpoints = get_midpoints(midpoints);
    points.push_back(control_points[0]);
    points.push_back(midpoints[0]);
    for (int p = 0; p < mid_of_midpoints.size(); p++) {
        points.push_back(mid_of_midpoints[p]);
        points.push_back(midpoints[p+1]);
    }
    points.push_back(control_points[control_points.size()-1]);
    return points;
}

vector< vector<Vertex> > init_sketch() {
    vector< vector<Vertex> > sketch_points;
    
    vector<Vertex> face;
    face.push_back(Vertex(-0.486f, 0.145f));
    face.push_back(Vertex(-0.314f, -1.25f));
    face.push_back(Vertex(0.47f, -0.923f));
    face.push_back(Vertex(0.573f, 0.08f));
    face.push_back(Vertex(0.896f, 1.244f));
    face.push_back(Vertex(-0.795f, 1.24f));
    face.push_back(Vertex(-0.486f, 0.145f));
    sketch_points.push_back(face);
    
    vector<Vertex> hair;
    hair.push_back(Vertex(-0.486f, 0.145f));
    hair.push_back(Vertex(-0.492f, 0.255f));
    hair.push_back(Vertex(-0.488f, 0.375f));
    hair.push_back(Vertex(-0.372f, 0.381f));
    hair.push_back(Vertex(-0.438f, 0.719f));
    hair.push_back(Vertex(-0.395f, 0.72f));
    hair.push_back(Vertex(0.334f, 0.668f));
    hair.push_back(Vertex(0.52f, 0.515f));
    hair.push_back(Vertex(0.367f, 0.385f));
    hair.push_back(Vertex(0.519f, 0.302f));
    hair.push_back(Vertex(0.573f, 0.08f));
    sketch_points.push_back(hair);
    
    vector<Vertex> ear_r;
    ear_r.push_back(Vertex(0.557f, 0.097f));
    ear_r.push_back(Vertex(0.657f, 0.392f));
    ear_r.push_back(Vertex(0.806f, 0.126f));
    ear_r.push_back(Vertex(0.623f, -0.15f));
    ear_r.push_back(Vertex(0.515f, -0.35f));
    ear_r.push_back(Vertex(0.53f, -0.037f));
    ear_r.push_back(Vertex(0.557f, 0.095f));
    sketch_points.push_back(ear_r);
    
    vector<Vertex> ear_l;
    ear_l.push_back(Vertex(-0.48f, 0.15f));
    ear_l.push_back(Vertex(-0.71f, 0.376f));
    ear_l.push_back(Vertex(-0.594f, -0.076f));
    ear_l.push_back(Vertex(-0.507f, -0.23f));
    ear_l.push_back(Vertex(-0.594f, -0.076f));
    ear_l.push_back(Vertex(-0.397f, -0.298f));
    ear_l.push_back(Vertex(-0.455f, -0.013f));
    ear_l.push_back(Vertex(-0.48f, 0.15f));
    sketch_points.push_back(ear_l);
    
    
    //RIGHT EYEBROW
    vector<Vertex> brow_r;
    brow_r.push_back(Vertex(0.118f, 0.23f));
    brow_r.push_back(Vertex(0.404f, 0.318f));
    brow_r.push_back(Vertex(0.425f, 0.21f));
    brow_r.push_back(Vertex(0.48f, 0.13f));
    sketch_points.push_back(brow_r);
    vector<Vertex> brow_r2;
    brow_r2.push_back(Vertex(0.142f, 0.192f));
    brow_r2.push_back(Vertex(0.31f, 0.208f));
    brow_r2.push_back(Vertex(0.397f, 0.176f));
    brow_r2.push_back(Vertex(0.48f, 0.13f));
    sketch_points.push_back(brow_r2);
    vector<Vertex> brow_r3;
    brow_r3.push_back(Vertex(0.118f, 0.23f));
    brow_r3.push_back(Vertex(0.142f, 0.190f));
    brow_r3.push_back(Vertex(0.142f, 0.192f));
    sketch_points.push_back(brow_r3);
    
    // LEFT EYEBROW
    vector<Vertex> brow_l;
    brow_l.push_back(Vertex(-0.008f, 0.23f));
    brow_l.push_back(Vertex(-0.294f, 0.318f));
    brow_l.push_back(Vertex(-0.315f, 0.21f));
    brow_l.push_back(Vertex(-0.37f, 0.13f));
    sketch_points.push_back(brow_l);
    vector<Vertex> brow_l2;
    brow_l2.push_back(Vertex(-0.032f, 0.192f));
    brow_l2.push_back(Vertex(-0.20f, 0.208f));
    brow_l2.push_back(Vertex(-0.287f, 0.176f));
    brow_l2.push_back(Vertex(-0.37f, 0.13f));
    sketch_points.push_back(brow_l2);
    vector<Vertex> brow_l3;
    brow_l3.push_back(Vertex(-0.008f, 0.23f));
    brow_l3.push_back(Vertex(-0.032f, 0.190f));
    brow_l3.push_back(Vertex(-0.032f, 0.192f));
    sketch_points.push_back(brow_l3);
    
    
    //LEFT EYELID
    vector<Vertex> eyelid_l;
    eyelid_l.push_back(Vertex(-0.342f, 0.082f));
    eyelid_l.push_back(Vertex(-0.22f, 0.197f));
    eyelid_l.push_back(Vertex(-0.053f, 0.184f));
    eyelid_l.push_back(Vertex(-0.07f, 0.07f));
    sketch_points.push_back(eyelid_l);
    vector<Vertex> eyelid_l2;
    eyelid_l2.push_back(Vertex(-0.342f, 0.082f));
    eyelid_l2.push_back(Vertex(-0.281f, 0.09f));
    eyelid_l2.push_back(Vertex(-0.116f, 0.087f));
    eyelid_l2.push_back(Vertex(-0.07f, 0.07f));
    sketch_points.push_back(eyelid_l2);
    
    //RIGHT EYELID
    vector<Vertex> eyelid_r;
    eyelid_r.push_back(Vertex(0.442f, 0.082f));
    eyelid_r.push_back(Vertex(0.32f, 0.197f));
    eyelid_r.push_back(Vertex(0.153f, 0.184f));
    eyelid_r.push_back(Vertex(0.17f, 0.07f));
    sketch_points.push_back(eyelid_r);
    vector<Vertex> eyelid_r2;
    eyelid_r2.push_back(Vertex(0.442f, 0.082f));
    eyelid_r2.push_back(Vertex(0.381f, 0.09f));
    eyelid_r2.push_back(Vertex(0.216f, 0.087f));
    eyelid_r2.push_back(Vertex(0.17f, 0.07f));
    sketch_points.push_back(eyelid_r2);
    
    
    //LEFT EYE
    vector<Vertex> eye_l;
    eye_l.push_back(Vertex(-0.237f, 0.145f));
    eye_l.push_back(Vertex(-0.24f, 0.027f));
    eye_l.push_back(Vertex(-0.063f, 0.147f));
    eye_l.push_back(Vertex(-0.216f, 0.183f));
    eye_l.push_back(Vertex(-0.237f, 0.145f));
    sketch_points.push_back(eye_l);
    
    //RIGHT EYE
    vector<Vertex> eye_r;
    eye_r.push_back(Vertex(0.337f, 0.145f));
    eye_r.push_back(Vertex(0.34f, 0.027f));
    eye_r.push_back(Vertex(0.163f, 0.147f));
    eye_r.push_back(Vertex(0.316f, 0.183f));
    eye_r.push_back(Vertex(0.337f, 0.145f));
    sketch_points.push_back(eye_r);
    
    //NOSE
    vector<Vertex> nose;
    nose.push_back(Vertex(-0.03, 0.174f));
    nose.push_back(Vertex(-0.04f, 0.02f));
    nose.push_back(Vertex(-0.048f, -0.097f));
    nose.push_back(Vertex(-0.14f, -0.163f));
    nose.push_back(Vertex(-0.174f, -0.24f));
    nose.push_back(Vertex(-0.06f, -0.268f));
    nose.push_back(Vertex(-0.02f, -0.255f));
    nose.push_back(Vertex(0.16f, -0.268f));
    nose.push_back(Vertex(0.274f, -0.24f));
    nose.push_back(Vertex(0.24f, -0.163f));
    nose.push_back(Vertex(0.148f, -0.097f));
    nose.push_back(Vertex(0.14f, 0.02f));
    nose.push_back(Vertex(0.13, 0.174f));

    sketch_points.push_back(nose);
    
    
    // LIPS
    vector<Vertex> lip_bottom;
    lip_bottom.push_back(Vertex(-0.19f, -0.412f));
    lip_bottom.push_back(Vertex(-0.032f, -0.544f));
    lip_bottom.push_back(Vertex(0.105f, -0.517f));
    lip_bottom.push_back(Vertex(0.25f, -0.402f));
    sketch_points.push_back(lip_bottom);
    vector<Vertex> lip_bottom2;
    lip_bottom2.push_back(Vertex(-0.19f, -0.412f));
    lip_bottom2.push_back(Vertex(-0.018f, -0.425f));
    lip_bottom2.push_back(Vertex(0.25f, -0.402f));
    sketch_points.push_back(lip_bottom2);
    vector<Vertex> lip_top_l;
    lip_top_l.push_back(Vertex(-0.19f, -0.412f));
    lip_top_l.push_back(Vertex(-0.082f, -0.36f));
    lip_top_l.push_back(Vertex(-0.015f, -0.351f));
    lip_top_l.push_back(Vertex(0.022f, -0.379f));
    sketch_points.push_back(lip_top_l);
    vector<Vertex> lip_top_r;
    lip_top_r.push_back(Vertex(0.022f, -0.379f));
    lip_top_r.push_back(Vertex(0.115f, -0.351f));
    lip_top_r.push_back(Vertex(0.182f, -0.36f));
    lip_top_r.push_back(Vertex(0.25f, -0.402f));
    sketch_points.push_back(lip_top_r);
    
    return sketch_points;
}

void draw_curve(vector<Vertex> bezier_points, int n_iter) {
    for(int i = 0; i < n_iter; i++) {
        bezier_points = generate_points(bezier_points);
    }
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < bezier_points.size(); i++) {
        glVertex2f(bezier_points[i].get_x(), bezier_points[i].get_y());
    }
    glEnd();
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < bezier_points.size() - 1; i++) {
        glVertex2f(bezier_points[i].get_x(), bezier_points[i].get_y());
        glVertex2f(bezier_points[i + 1].get_x(), bezier_points[i + 1].get_y());
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    
    vector< vector<Vertex> > initial_points = init_sketch();
    
    for (int i = 0; i < initial_points.size(); i++) {
        vector<Vertex> points_i = initial_points[i];
        vector<Vertex> points_to_draw = generate_points(points_i);
        draw_curve(points_to_draw, 10);
    }
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
