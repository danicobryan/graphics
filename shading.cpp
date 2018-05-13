/***
 Assignment-3: Shading via Lighting and Colors
 
 Name: Pidlaoan, Danico
 
 Project Summary: This project implements Gouraud shading to heriarchal models.
 The project implements models made from square planes, defining source light
 and camera, and computes the appropriate shading. Project written in C++
 for OpenGL.
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
#include <stdio.h>
using namespace std;

float theta = 0.0;

vector<GLfloat> AMB = {0.45, 0.45, 0.45};
vector<GLfloat> DIFF = {0.15, 0.15, 0.15};
vector<GLfloat> SPEC = {0.05, 0.05, 0.05};
vector<GLfloat> LIGHT = {-3.5, -3.5, 1.5};
vector<GLfloat> CAMERA = {2.0, 3.0, 5.0};



/**************************************************
 *              Object Model Class                *
 *                                                *
 *  Stores the points of the object as a vector   *
 *  along with the colors and normals for each    *
 *  point. Normals are computed from the points.  *
 *                                                *
 *************************************************/
class ObjectModel {
    vector<GLfloat> _points;
    vector<GLfloat> _normals;
    vector<GLfloat> _base_colors;
    vector<GLfloat> _colors;
public:
    ObjectModel() { };
    vector<GLfloat> get_points() { return _points; };
    vector<GLfloat> get_normals() { return _normals; };
    vector<GLfloat> get_base_colors() { return _base_colors; };
    vector<GLfloat> get_colors() { return _colors; };
    void set_points(vector<GLfloat> points) { _points = points; };
    void set_normals(vector<GLfloat> normals) { _normals = normals; };
    void set_base_colors(vector<GLfloat> base_colors) { _base_colors = base_colors; };
    void set_colors(vector<GLfloat> colors) { _colors = colors; };
};


/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  Using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene.*
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
    vector<GLfloat> result;
    
    // Append the 1 in the 4th dimension to generate homoegenous coordinates
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
    
    // Perform matrix multiplication for A B
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
 *           Generating Surface Normals           *
 *                                                *
 *  Generate the surface normals of the objects   *
 *  using the cross product between two vectors   *
 *  that lie on the surface (plane) of interest.  *
 *  Recall that the direction of the normal to a  *
 *  surface follows the Right Hand Rule.          *
 *                                                *
 *************************************************/


GLfloat mag(GLfloat x, GLfloat y, GLfloat z){
    return (GLfloat)sqrt((x * x) + (y * y) + (z * z));
}

vector<GLfloat> add_vectors(vector<GLfloat> a, vector<GLfloat> b){
    vector<GLfloat> result;
    result.push_back(a[0] + b[0]);
    result.push_back(a[1] + b[1]);
    result.push_back(a[2] + b[2]);
    return result;
}

vector<GLfloat> subtract_vectors(vector<GLfloat> a, vector<GLfloat> b){
    vector<GLfloat> result;
    result.push_back(a[0] - b[0]);
    result.push_back(a[1] - b[1]);
    result.push_back(a[2] - b[2]);
    return result;
}

vector<GLfloat> unit_vector(vector<GLfloat> a){
    vector<GLfloat> result;
    
    result.push_back(a[0] / mag(a[0], a[1], a[2]));
    result.push_back(a[1] / mag(a[0], a[1], a[2]));
    result.push_back(a[2] / mag(a[0], a[1], a[2]));
    
    return result;
}

// Performs the cross product between two vectors
vector<GLfloat> cross_product(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C;
    
    C.push_back( ( A[1]*B[2] ) - (A[2] * B[1]) );
    C.push_back( ( A[0]*B[2] ) - (A[2] * B[0]) );
    C.push_back( ( A[0]*B[1] ) - (A[1] * B[0]) );
    
    return C;
}

// Generates the normals to each surface (plane)
vector<GLfloat> generate_normals(vector<GLfloat> points) {
    vector<GLfloat> normals;
    
    // Note: each plane (quad) contains 4 points, choose the points
    // to generate your vectors such that the normals (given by the
    // cross product, point to the correct direction
    
    
    for(int i = 0; i < points.size(); i += 12){
        vector<GLfloat> q0 = {points[i], points[i+1], points[i+2]};
        vector<GLfloat> q1 = {points[i+3], points[i+4], points[i+5]};
        vector<GLfloat> q3 = {points[i+9], points[i+10], points[i+11]};
        
        vector<GLfloat> a = subtract_vectors(q1, q0);
        vector<GLfloat> b = subtract_vectors(q3, q0);
        
        vector<GLfloat> unit = unit_vector(cross_product(a,b));
        
        for(int j = 0; j < 4; j++){
            normals.push_back(unit[0]);
            normals.push_back(unit[1]);
            normals.push_back(unit[2]);
        }
    }
    
    return normals;
}


/**************************************************
 *         Shading via Lighting and Color         *
 *                                                *
 *  Generate the set of colors for each face of   *
 *  the planes that compose the objects in the    *
 *  scene. Based on the light source and surface  *
 *  normals, render the colors of the objects by  *
 *  applying the shading equation.                *
 *                                                *
 *************************************************/

// Initializes the base color of a plane to a single color
vector<GLfloat> init_base_color(GLfloat r, GLfloat g, GLfloat b) {
    vector<GLfloat> base_color = {
        r,   g,   b,
        r,   g,   b,
        r,   g,   b,
        r,   g,   b
    };
    return base_color;
}

// Initializes the base color of a plane by specifying the color of each point
vector<GLfloat> init_base_color(GLfloat r0, GLfloat g0, GLfloat b0, GLfloat r1, GLfloat g1, GLfloat b1,
                                GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3) {
    // This enables OpenGL to use interpolation for (Gouraud) shading the plane
    vector<GLfloat> base_color = {
        r0,   g0,   b0,
        r1,   g1,   b1,
        r2,   g2,   b2,
        r3,   g3,   b3
    };
    return base_color;
}

// Performs the dot product between two vectors
GLfloat dot_product(vector<GLfloat> A, vector<GLfloat> B) {
    
    return (A[0] * B[0]) + (A[1] * B[1]) + (A[2] * B[2]);
}

// Generates the colors of a set of surfaces based on the light source,
// surface normals, and base color of the surface
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera) {
    vector<GLfloat> colors;
    
    // base colors, points, normals
    vector<GLfloat> base_colors = object_model.get_base_colors();
    vector<GLfloat> points = object_model.get_points();
    vector<GLfloat> normals = object_model.get_normals();
    
    // for loop by point (3)
    for(int i = 0; i < points.size(); i += 3){
        vector<GLfloat> point = {points[i], points[i+1], points[i+2]};
        vector<GLfloat> base_color = {base_colors[i], base_colors[i+1], base_colors[i+2]};
        vector<GLfloat> normal = {normals[i], normals[i+1], normals[i+2]};
        
        vector<GLfloat> v = subtract_vectors(light_source, point);
        vector<GLfloat> h = unit_vector(subtract_vectors(light_source,v));
        GLfloat n_dot_l = dot_product(normal, light_source);
        GLfloat n_dot_h = dot_product(normal, h);
        
        GLfloat red = base_color[0] * (AMB[0] + (DIFF[0] * (n_dot_l))) + (SPEC[0] * base_color[0] * (n_dot_h));
        colors.push_back(red);
        
        GLfloat green = base_color[1] * (AMB[1] + (DIFF[1] * (n_dot_l))) + (SPEC[1] * base_color[1] * (n_dot_h));
        colors.push_back(green);
        
        GLfloat blue = base_color[2] * (AMB[2] + (DIFF[2] * (n_dot_l))) + (SPEC[2] * base_color[2] * (n_dot_h));
        colors.push_back(blue);
    }
    
    object_model.set_colors(colors);
    return object_model;
}

// Allows for ambience (a vector of 3 values), diffusion (vector of 3 x n values) and specular (vector of 3 x n values)
// as input to the shading equation
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera,
                          vector<GLfloat> amb, vector<GLfloat> diff, vector<GLfloat> spec) {
    vector<GLfloat> colors;
    
    // base colors, points, normals
    vector<GLfloat> base_colors = object_model.get_base_colors();
    vector<GLfloat> points = object_model.get_points();
    vector<GLfloat> normals = object_model.get_normals();
    
    // for loop by point (3)
    for(int i = 0; i < points.size(); i += 3){
        vector<GLfloat> point = {points[i], points[i+1], points[i+2]};
        vector<GLfloat> base_color = {base_colors[i], base_colors[i+1], base_colors[i+2]};
        vector<GLfloat> normal = {normals[i], normals[i+1], normals[i+2]};
        
        vector<GLfloat> v = subtract_vectors(light_source, point);
        vector<GLfloat> h = unit_vector(subtract_vectors(light_source,v));
        GLfloat n_dot_l = dot_product(normal, light_source);
        GLfloat n_dot_h = dot_product(normal, h);
        
        GLfloat red = base_color[0] * (amb[0] + (diff[0] * (n_dot_l))) + (spec[0] * base_color[0] * (n_dot_h));
        colors.push_back(red);
        
        GLfloat green = base_color[1] * (amb[1] + (diff[1] * (n_dot_l))) + (spec[1] * base_color[1] * (n_dot_h));
        colors.push_back(green);
        
        GLfloat blue = base_color[2] * (amb[2] + (diff[2] * (n_dot_l))) + (spec[2] * base_color[2] * (n_dot_h));
        colors.push_back(blue);
    }
    
    object_model.set_colors(colors);
    return object_model;
}



/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  Create a scene by applying transformations to *
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
    glClearColor(0.2, 0.2, 0.2, 0.0);
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

vector<GLfloat> doublevector_to_vector(vector<vector<GLfloat>> object){
    vector<GLfloat> sc;
    for(int i = 0; i < object.size(); i++){
        sc.insert(sc.end(), object[i].begin(), object[i].end());
    }
    return sc;
}

ObjectModel make_bed(){
    ObjectModel bed_object;
    
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
    
    bed_object.set_points(doublevector_to_vector(bed));
    
    
    //**** NORMALS ****//
    vector<GLfloat> bed_points = bed_object.get_points();
    
    vector<GLfloat> normals = generate_normals(bed_points);
    bed_object.set_normals(normals);
    
    
    //**** COLORS ****//
    
    vector<vector<GLfloat>> colors;
    int planes_per_cube = 6;
    
    //Bed Base
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.52, 0.37, 0.26));
    }
    //Mattress
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.5, 0.5, 0.5));
    }
    //Pillow
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(1.000, 1.000, 0.941));
    }
    //Headboard
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.52, 0.37, 0.26));
    }
    
    bed_object.set_base_colors(doublevector_to_vector(colors));
    
    return apply_shading(bed_object, LIGHT, CAMERA, AMB, DIFF, SPEC);
    
}

ObjectModel make_rug(){
    ObjectModel rug_object;
    
    rug_object.set_points(to_cartesian_coord(mat_mult(scaling_matrix(2.0, 0.01, 1.5), to_homogenous_coord(build_cube()))));
    
    vector<vector<GLfloat>> colors;
    int planes_per_cube = 6;
    
    vector<GLfloat> normals = generate_normals(rug_object.get_points());
    rug_object.set_normals(normals);
    
    //Rug Color
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.19, 0.6, 0.8));
    }
    
    rug_object.set_base_colors(doublevector_to_vector(colors));

    return apply_shading(rug_object, LIGHT, CAMERA, AMB, DIFF, SPEC);
}

ObjectModel make_drawer(){
    ObjectModel drawer_object;
    
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
    
    drawer_object.set_points(doublevector_to_vector(drawer));
    
    
    //**** NORMALS ****//
    vector<GLfloat> normals = generate_normals(drawer_object.get_points());
    drawer_object.set_normals(normals);
    
    
    //**** COLORS ****//
    
    vector<vector<GLfloat>> colors;
    int planes_per_cube = 6;
    
    //Body
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.52, 0.37, 0.26));
    }
    
    //Legs
    for(int i = 0; i < planes_per_cube * 4; i++){
        colors.push_back(init_base_color(0.0, 0.0, 0.0));
    }
    
    drawer_object.set_base_colors(doublevector_to_vector(colors));
    
    return apply_shading(drawer_object, LIGHT, CAMERA, AMB, DIFF, SPEC);
    
}

ObjectModel make_table(){
    ObjectModel table_object;
    
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
    
    table_object.set_points(doublevector_to_vector(table));
    
    
    //**** NORMALS ****//
    vector<GLfloat> normals = generate_normals(table_object.get_points());
    table_object.set_normals(normals);
    
    
    //**** COLORS ****//
    
    vector<vector<GLfloat>> colors;
    int planes_per_cube = 6;
    
    //Body
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.52, 0.37, 0.26));
    }
    
    //Legs
    for(int i = 0; i < planes_per_cube * 4; i++){
        colors.push_back(init_base_color(0.0, 0.0, 0.0));
    }
    
    table_object.set_base_colors(doublevector_to_vector(colors));
    
    table_object = apply_shading(table_object, LIGHT, CAMERA, AMB, DIFF, SPEC);
    
    return table_object;
    
}


ObjectModel make_lamp(){
    ObjectModel lamp_object;
    
    //TABLE
    vector<vector<GLfloat>> lamp;
    
    lamp.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.0, 1.5, 0.0), (mat_mult(scaling_matrix(0.5, 0.3, 0.3), to_homogenous_coord(build_cube()))) )));

    
    //leg front-left
    lamp.push_back( to_cartesian_coord(mat_mult(translation_matrix(0.0, 1.2, 0.0), (mat_mult(scaling_matrix(0.1, 0.5, 0.1), to_homogenous_coord(build_cube()))) ))  );
    
    
    for(int element = 0; element < lamp.size(); element++){
        lamp[element] = to_cartesian_coord(mat_mult(translation_matrix(1.2, 0.5, -2.0), to_homogenous_coord(lamp[element])));
    }
    
    
    lamp_object.set_points(doublevector_to_vector(lamp));
    
    vector<GLfloat> lamp_points = lamp_object.get_points();
    
    LIGHT = {lamp_points[0] - (GLfloat)1.3, -1 * lamp_points[1], -1 * lamp_points[2]};
    
    
    //**** NORMALS ****//
    vector<GLfloat> normals = generate_normals(lamp_object.get_points());
    lamp_object.set_normals(normals);
    
    
    //**** COLORS ****//
    
    vector<vector<GLfloat>> colors;
    int planes_per_cube = 6;

    //Bulb
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.933, 0.910, 0.667));
    }
    //Legs
    for(int i = 0; i < planes_per_cube; i++){
        colors.push_back(init_base_color(0.0, 0.0, 0.0));
    }
    
    lamp_object.set_base_colors(doublevector_to_vector(colors));
    
    lamp_object = apply_shading(lamp_object, LIGHT, CAMERA, AMB, DIFF, SPEC);
    
    return lamp_object;
    
}


// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    vector<GLfloat> vertices_of_scene;
    
    ObjectModel bed = make_bed();
    vector<GLfloat> bed_vertices = bed.get_points();
    vertices_of_scene.insert(vertices_of_scene.end(), bed_vertices.begin(), bed_vertices.end());
    
    ObjectModel rug = make_rug();
    vector<GLfloat> rug_vertices = rug.get_points();
    vertices_of_scene.insert(vertices_of_scene.end(), rug_vertices.begin(), rug_vertices.end());
    
    ObjectModel drawer = make_drawer();
    vector<GLfloat> drawer_vertices = drawer.get_points();
    vertices_of_scene.insert(vertices_of_scene.end(), drawer_vertices.begin(), drawer_vertices.end());
    
    ObjectModel table = make_table();
    vector<GLfloat> table_vertices = table.get_points();
    vertices_of_scene.insert(vertices_of_scene.end(), table_vertices.begin(), table_vertices.end());
    
    ObjectModel lamp = make_lamp();
    vector<GLfloat> lamp_vertices = lamp.get_points();
    vertices_of_scene.insert(vertices_of_scene.end(), lamp_vertices.begin(), lamp_vertices.end());
    
    return vector2array(vertices_of_scene);
}

// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> colors_of_scene;
    
    ObjectModel bed = make_bed();
    vector<GLfloat> bed_colors = bed.get_colors();
    colors_of_scene.insert(colors_of_scene.end(), bed_colors.begin(), bed_colors.end());
    
    ObjectModel rug = make_rug();
    vector<GLfloat> rug_colors = rug.get_colors();
    colors_of_scene.insert(colors_of_scene.end(), rug_colors.begin(), rug_colors.end());
    
    ObjectModel drawer = make_drawer();
    vector<GLfloat> drawer_colors = drawer.get_colors();
    colors_of_scene.insert(colors_of_scene.end(), drawer_colors.begin(), drawer_colors.end());
    
    ObjectModel table = make_drawer();
    vector<GLfloat> table_colors = table.get_colors();
    colors_of_scene.insert(colors_of_scene.end(), table_colors.begin(), table_colors.end());
    
    ObjectModel lamp = make_lamp();
    vector<GLfloat> lamp_colors = lamp.get_colors();
    colors_of_scene.insert(colors_of_scene.end(), lamp_colors.begin(), lamp_colors.end());
    
    return vector2array(colors_of_scene);
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
    //glDrawArrays(GL_QUADS, 0, 4*6*(elements));
    glDrawArrays(GL_QUADS, 0, 4*6*(17));
    
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
    glutCreateWindow("Assignment 3");
    
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


