/***
 Assignment-2: Geometric Modeling of a Scene
 
 Name: Wong, Alex (Please write your name in Last Name, First Name format)
 
 Collaborators: Doe, John; Doe, Jane
 ** Note: although the assignment should be completed individually
 you may speak with classmates on high level algorithmic concepts. Please
 list their names in this section
 
 Project Summary: A short paragraph (3-4 sentences) describing the work you
 did for the project.
 ***/



#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <vector>
#include <iostream>
#include <iterator>
using namespace std;

float theta = 0.0;
long elements = 0;

/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene *
 *                                                *
 *************************************************/

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
    vector<GLfloat> vertices = {
        +0.5,   +0.5,   +0.0,
        -0.5,   +0.5,   +0.0,
        -0.5,   -0.5,   +0.0,
        +0.5,   -0.5,   +0.0
    };
    return vertices;
}

// Converts a vector to an array
GLfloat* vector2array(vector<GLfloat> vec) {
    GLfloat* arr = new GLfloat[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
    return arr;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_homogenous_coord(vector<GLfloat> cartesian_coords) {
    
    // Append the 1 in the 4th dimension to generate homoegenous coordinates
    
    vector<GLfloat> result;
    
    for(int i = 0; i < cartesian_coords.size(); i+=3){
        result.push_back(cartesian_coords[i]);
        result.push_back(cartesian_coords[i+1]);
        result.push_back(cartesian_coords[i+2]);
        result.push_back(+1.0);
    }
    
    return result;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_cartesian_coord(vector<GLfloat> homogenous_coords) {
    
    // Remove the 1 in the 4th dimension to generate Cartesian coordinates
    vector<GLfloat> result;
    for(int i = 0; i < homogenous_coords.size(); i++){
        if((i + 1) % 4 != 0){
            result.push_back(homogenous_coords[i]);
        }
    }
    
    return result;
}

// Definition of a translation matrix
vector<GLfloat> translation_matrix (float dx, float dy, float dz) {
    vector<GLfloat> translate_mat = {
        +1.0, +0.0, +0.0, dx,
        +0.0, +1.0, +0.0, dy,
        +0.0, +0.0, +1.0, dz,
        +0.0, +0.0, +0.0, +1.0
    };
    
    return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix (float sx, float sy, float sz) {
    vector<GLfloat> scale_mat = {
        sx, +0.0, +0.0, +0.0,
        +0.0, sy, +0.0, +0.0,
        +0.0, +0.0, sz, +0.0,
        +0.0, +0.0, +0.0, +1.0
    };
    
    return scale_mat;
}

// Definition of a rotation matrix along the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    vector<GLfloat> rotate_mat_x = {
        +1.0, +0.0, +0.0, +0.0,
        +0.0, cos(theta), -sin(theta), +0.0,
        +0.0, sin(theta), cos(theta), +0.0,
        +0.0, +0.0, +0.0, +1.0
    };
    
    return rotate_mat_x;
}


// Definition of a rotation matrix along the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    vector<GLfloat> rotate_mat_y = {
        cos(theta), +0.0, sin(theta), +0.0,
        +0.0, +1.0, +0.0, +0.0,
        -sin(theta), +0.0, cos(theta), +0.0,
        +0.0, +0.0, +0.0, +1.0
    };
    
    return rotate_mat_y;
}


// Definition of a rotation matrix along the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    vector<GLfloat> rotate_mat_z = {
        cos(theta), -sin(theta), +0.0, +0.0,
        sin(theta), cos(theta), +0.0, +0.0,
        +0.0, +0.0, +1.0, +0.0,
        +0.0, +0.0, +0.0, +1.0
    };
    
    return rotate_mat_z;
}

// Perform matrix multiplication for A B
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    
    for(int i = 0; i < B.size(); i += 4){
        GLfloat newX = (A[0] * B[i]) + (A[1] * B[i+1]) + (A[2] * B[i+2]) + (A[3] * B[i+3]);
        GLfloat newY = (A[4] * B[i]) + (A[5] * B[i+1]) + (A[6] * B[i+2]) + (A[7] * B[i+3]);
        GLfloat newZ = (A[8] * B[i]) + (A[9] * B[i+1]) + (A[10] * B[i+2]) + (A[11] * B[i+3]);
        GLfloat one = (A[12] * B[i]) + (A[13] * B[i+1]) + (A[14] * B[i+2]) + (A[15] * B[i+3]);
        
        result.push_back(newX);
        result.push_back(newY);
        result.push_back(newZ);
        result.push_back(one);
    }
    
    return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
    vector<GLfloat> result;
    
    // Creates a unit cube by transforming a set of planes
    vector<GLfloat> front = to_cartesian_coord(mat_mult(translation_matrix(+0.0, +0.0, +0.5), to_homogenous_coord(init_plane())));
    
    vector<GLfloat> right = to_cartesian_coord(mat_mult(translation_matrix(+0.5, +0.0, +0.0), mat_mult(rotation_matrix_y(M_PI / +2.0), to_homogenous_coord(init_plane()))));
    
    vector<GLfloat> left = to_cartesian_coord(mat_mult(translation_matrix(-0.5, +0.0, +0.0), mat_mult(rotation_matrix_y(M_PI / -2.0), to_homogenous_coord(init_plane()))));
    
    vector<GLfloat> back = to_cartesian_coord(mat_mult(translation_matrix(+0.0, +0.0, -0.5), mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(init_plane()))));
    
    vector<GLfloat> top = to_cartesian_coord(mat_mult(translation_matrix(+0.0, +0.5, +0.0), mat_mult(rotation_matrix_x(M_PI / +2.0), to_homogenous_coord(init_plane()))));
    vector<GLfloat> bottom = to_cartesian_coord(mat_mult(translation_matrix(+0.0, -0.5, +0.0), mat_mult(rotation_matrix_x(M_PI / -2.0), to_homogenous_coord(init_plane()))));
    
    
    result = front;
    result.insert(result.end(), right.begin(), right.end());
    
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), back.begin(), back.end());
    result.insert(result.end(), top.begin(), top.end());
    result.insert(result.end(), bottom.begin(), bottom.end());

    
    return result;
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by using setting *
 *  the projection viewing matrices               *
 *                                                *
 *************************************************/

void setup() {
    // Enable the vertex array functionality
    glEnableClientState(GL_VERTEX_ARRAY);
    // Enable the color array functionality (so we can specify a color for each vertex)
    glEnableClientState(GL_COLOR_ARRAY);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Set up some default base color
    glColor3f(0.5, 0.5, 0.5);
    // Set up white background
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void init_camera() {
    // Camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a 50 degree field of view, 1:1 aspect ratio, near and far planes at 3 and 7
    gluPerspective(50.0, 1.0, 2.0, 10.0);
    // Position camera at (2, 3, 5), attention at (0, 0, 0), up at (0, 1, 0)
    gluLookAt(2.0, 6.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0);
}

vector<GLfloat> object_to_scene(vector<vector<GLfloat>> object, vector<GLfloat> sc){
    for(int i = 0; i < object.size(); i++){
        sc.insert(sc.end(), object[i].begin(), object[i].end());
    }
    return sc;
}

// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    //Scene
    vector<GLfloat> scene;
    
    // BED
    vector<vector<GLfloat>> bed;
    //Bed Base
    bed.push_back(to_cartesian_coord(mat_mult(scaling_matrix(1.0, -0.5, 2.0), to_homogenous_coord(build_cube()))));
    //Mattress
    bed.push_back(to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, 0.0), (mat_mult(scaling_matrix(1.0, -0.5, 2.0), to_homogenous_coord(build_cube()))))));
    // Pillow
    bed.push_back(to_cartesian_coord(mat_mult(translation_matrix(0.0, 1.0, -1.0), (mat_mult(scaling_matrix(0.6, -0.5, 0.2), to_homogenous_coord(build_cube()))))));
    //Headboard
    bed.push_back(to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.75, -1.1), (mat_mult(scaling_matrix(1.0, 2.0, 0.2), to_homogenous_coord(build_cube()))))));
    
    for(int element = 0; element < bed.size(); element++){
        bed[element] = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.25, -1.0), to_homogenous_coord(bed[element])));
    }
    
    // RUG
    vector<vector<GLfloat>> rug;
    //Bed Base
    rug.push_back(to_cartesian_coord(mat_mult(scaling_matrix(2.0, 0.01, 1.5), to_homogenous_coord(build_cube()))));
    
    
    //DRAWER
    vector<vector<GLfloat>> drawer;
    //Body
    drawer.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, 0.0), (mat_mult(scaling_matrix(2.0, 1.0, 1.0), to_homogenous_coord(build_cube()))) )));
    //Leg front-left
    drawer.push_back( to_cartesian_coord(mat_mult(translation_matrix(-0.9, -0.25, 0.4), (mat_mult(scaling_matrix(0.2, 0.5, 0.2), to_homogenous_coord(build_cube()))) ))  );
    //Leg front-right
    drawer.push_back( to_cartesian_coord(mat_mult(translation_matrix(+0.9, -0.25, 0.4), (mat_mult(scaling_matrix(0.2, 0.5, 0.2), to_homogenous_coord(build_cube()))) ))  );
    //Leg back-left
    drawer.push_back( to_cartesian_coord(mat_mult(translation_matrix(-0.9, -0.25, -0.4), (mat_mult(scaling_matrix(0.2, 0.5, 0.2), to_homogenous_coord(build_cube()))) ))  );
    drawer.push_back( to_cartesian_coord(mat_mult(translation_matrix(+0.9, -0.25, -0.4), (mat_mult(scaling_matrix(0.2, 0.5, 0.2), to_homogenous_coord(build_cube()))) ))  );
    for(int element = 0; element < drawer.size(); element++){
        drawer[element] = to_cartesian_coord(mat_mult(translation_matrix(-2.0, 0.5, -1.0), (mat_mult(rotation_matrix_y(M_PI / +2.0), to_homogenous_coord(drawer[element]))) ));
    }
    
    //TABLE
    vector<vector<GLfloat>> table;
    table.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, 0.0), (mat_mult(scaling_matrix(1.0, 1.0, 0.7), to_homogenous_coord(build_cube()))) )));
    //leg front-left
    table.push_back( to_cartesian_coord(mat_mult(translation_matrix(-0.45, -0.25, 0.3), (mat_mult(scaling_matrix(0.1, 0.5, 0.1), to_homogenous_coord(build_cube()))) ))  );
    table.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.45, -0.25, 0.3), (mat_mult(scaling_matrix(0.1, 0.5, 0.1), to_homogenous_coord(build_cube()))) ))  );
    table.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.45, -0.25, -0.3), (mat_mult(scaling_matrix(0.1, 0.5, 0.1), to_homogenous_coord(build_cube()))) ))  );
    table.push_back( to_cartesian_coord(mat_mult(translation_matrix(-0.45, -0.25, -0.3), (mat_mult(scaling_matrix(0.1, 0.5, 0.1), to_homogenous_coord(build_cube()))) ))  );
    for(int element = 0; element < table.size(); element++){
        table[element] = to_cartesian_coord(mat_mult(translation_matrix(1.2, 0.5, -2.0), to_homogenous_coord(table[element])));
    }
    
    
    scene = object_to_scene(bed, scene);
    scene = object_to_scene(rug, scene);
    scene = object_to_scene(drawer, scene);
    scene = object_to_scene(table, scene);
    
    elements = bed.size() + rug.size() + drawer.size() + table.size();
    
    return vector2array(scene);
}

// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> colors;
    
    //Color Matices
    
    //Grey
    vector<GLfloat> grey = {
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
    };
    
    //Brown
    vector<GLfloat> brown {
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
        0.52, 0.37, 0.26,
    };
    
    //Blue
    vector<GLfloat> blue {
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
        0.19, 0.6, 0.8,
    };
    
    //Black
    vector<GLfloat> black {
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        
    };
    
    //Snow
    vector<GLfloat> snow = {
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
        1.000, 1.000, 0.941,
    };
    
    // Base
    colors.insert(colors.end(), brown.begin(), brown.end());
    
    //Mattress
    colors.insert(colors.end(), grey.begin(), grey.end());
    
    //Pillow
    colors.insert(colors.end(), snow.begin(), snow.end());
    
    //Headboard
    colors.insert(colors.end(), brown.begin(), brown.end());
    
    //Mat
    colors.insert(colors.end(), blue.begin(), blue.end());
    
    //Drawer Body
    colors.insert(colors.end(), brown.begin(), brown.end());
    
    //Drawer Legs
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    
    
    //Table Body
    colors.insert(colors.end(), brown.begin(), brown.end());
    
    //Drawer Legs
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    colors.insert(colors.end(), black.begin(), black.end());
    
    return vector2array(colors);
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // World model parameters
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //glRotatef(theta, 1.0, 0.0, 0.0);
    glRotatef(theta, 0.0, 1.0, 0.0);
    
    GLfloat* vertices = init_scene();
    
    GLfloat* colors = init_color();
    
    glVertexPointer(3,          // 3 components (x, y, z)
                    GL_FLOAT,   // Vertex type is GL_FLOAT
                    0,          // Start position in referenced memory
                    vertices);  // Pointer to memory location to read from
    
    //pass the color pointer
    glColorPointer(3,           // 3 components (r, g, b)
                   GL_FLOAT,    // Vertex type is GL_FLOAT
                   0,           // Start position in referenced memory
                   colors);     // Pointer to memory location to read from
    
    // Draw quad point planes: each 4 vertices
    glDrawArrays(GL_QUADS, 0, 4*6*(elements));
    
    glFlush();            //Finish rendering
    glutSwapBuffers();
    
    delete vertices;
    delete colors;
}

void idle_func() {
    theta = theta+0.3;
    display_func();
}

int main (int argc, char **argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Create a window with rendering context and everything else we need
    glutCreateWindow("Assignment 2");
    
    setup();
    init_camera();
    
    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();
    
    // Remember to call "delete" on your dynmically allocated arrays
    // such that you don't suffer from memory leaks. e.g.
    // delete arr;
    
    
    return 0;
}


