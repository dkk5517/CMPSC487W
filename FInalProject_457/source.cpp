#define _CRT_SECURE_NO_WARNINGS
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "obj_loader.h"

// Create an instance of the OBJ loader
objl::Loader loader;
struct Vertex { float x, y, z; };

std::vector<Vertex> vertices; // Store vertex positions

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

GLuint earthTexture; // Earth texture ID
GLuint moonTexture;  // Moon texture ID

float angle = 0.0f;        // Rotation angle for Earth
float moonAngle = 0.0f;    // Angle for moon's parametric curve motion
bool moonLightOn = true;   // Toggle for moonlight
float modelPosX = -3.0f; // Start position of the model (X-axis)
float modelSpeed = 0.01f; // Movement speed
bool moveAstronaut = false; // Flag to control astronaut movement
float cameraZoom = 8.5f; // Camera zoom (initial value)
float cameraAngleX = 0.0f; // Left/right rotation
float cameraX = 0.0f; // X-position of the camera
float cameraY = 0.0f; // X-position of the camera
float cameraZ = 8.5f; // Default Z-position of the camera
float cameraSpeed = 0.5f; // Speed of camera translation

// Object position
float objectPositionY = 0.0f; // Initial position along the Y-axis
float objectPositionX = 0.0f; // Initial position along the X-axis
float rotationAngleX = 0.0f; // Initial rotation angle around X-axis
float rotationAngleY = 0.0f; // Initial rotation angle around Y-axis
float rotationAngleZ = 0.0f; // Initial rotation angle around Z-axis
// Function prototypes
void display();
void reshape(int w, int h);
void timer(int value);
void drawSphere(float radius, int slices, int stacks);
GLuint loadBMP(const char* filePath);
void initLighting();
void handleKeyboard(unsigned char key, int x, int y);
void drawMoonWithLight();
void drawParametricMoon();
void handleSpecialKeyboard(int key, int x, int y);

// Load the OBJ file and its associated MTL file
bool loadModel(const std::string& filename) {
    return loader.LoadFile(filename);
}

void drawOBJ() {
    glTranslatef(0.0f, 2.8f, 0.0f);
    for (const auto& mesh : loader.LoadedMeshes) {
        glBegin(GL_TRIANGLES);
        for (const auto& index : mesh.Indices) {
            const auto& vertex = mesh.Vertices[index];
            glNormal3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
            glColor3f(1.0f, 1.0f, 1.0f); // Set object color to white
            glVertex3f(vertex.Position.X * 0.5f, vertex.Position.Y * 0.5f, vertex.Position.Z * 0.5f); // Scale down the object
        }
        glEnd();
    }


}


void drawCustomSpaceModel() {
    // Set the color for stars
    glColor3f(1.0f, 1.0f, 1.0f); // Set color for stars (white)

    // Number of stars
    const int numStars = 15;
    for (int i = 0; i < numStars; ++i) {
        // Randomize star position
        float x = (rand() % 200 - 100) / 10.0f;
        float y = (rand() % 200 - 100) / 10.0f;
        float z = (rand() % 200 - 100) / 10.0f;

        // Draw a small sphere (star)
        glPushMatrix();
        glTranslatef(x, y, z);
        drawSphere(0.05f, 2, 2);
        glPopMatrix();
    }
}
void renderText(const char* text, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT); // Set orthographic projection (screen coordinates)

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING); // Disable lighting to make text visible
    glColor3f(1.0f, 1.0f, 1.0f); // White text

    glRasterPos2i(x, y); // Set the position for text
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Earth and Moon Demo");

    // Initialize lighting and textures
    glEnable(GL_DEPTH_TEST);
    initLighting();
    earthTexture = loadBMP("earth.bmp");
    moonTexture = loadBMP("moon.bmp"); // Load moon texture

    // Register callbacks
    glutDisplayFunc(display);
    glutIdleFunc(display); // Automatically call display function to create continuous movement
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handleKeyboard);
    glutSpecialFunc(handleSpecialKeyboard);

    glutTimerFunc(0, timer, 0);
    if (!loadModel("astro.obj")) {
        std::cerr << "Failed to load astro.obj\n";
        return -1;
    }

    glutMainLoop();


    return 0;
}

void display() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Draw the instruction text
    renderText("Instructions:", 10, HEIGHT - 20);
    renderText("Press '+' to zoom in", 10, HEIGHT - 40);
    renderText("Press '-' to zoom out", 10, HEIGHT - 60);
    renderText("Press Left/Right (A/D) to rotate camera", 5, HEIGHT - 80);
    renderText("Forward/Back arrows to move camera up/down", 5, HEIGHT - 100);
    renderText("Press 'L' to toggle moonlight", 10, HEIGHT - 120);
    renderText("Astrounat on Top of the World", 10, HEIGHT - 150);

    // Render the flying astro.obj model
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Translate to the center (X, Y, Z = 0)
    glScalef(1.0f, 1.0f, 1.0f); // Scale up the model significantly
    glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for astronaut
   
    glPopMatrix();
    float cameraX = sin(cameraAngleX * M_PI / 180.0f) * cameraZoom;
    float cameraZ = cos(cameraAngleX * M_PI / 180.0f) * cameraZoom;


    gluLookAt(
        cameraX, 0.0f, cameraZ,  // Camera position (X, Y, Z)
        cameraX, cameraY, 0.0f,     // Look-at point moves with cameraX
        0.0f, 1.0f, 0.0f         // Up direction
    );




    // Apply Earth's rotation
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    // Bind Earth texture and draw
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    glEnable(GL_TEXTURE_2D);
    drawSphere(2.0f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    // Draw stars
    drawCustomSpaceModel();

    // Draw moon with texture and parametric curve
    drawMoonWithLight();
    drawOBJ();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    angle += 0.5f;
    moonAngle += 0.02f; // Update moon's movement along parametric curve
    if (angle > 360.0f) angle -= 360.0f;
    // Move astronaut only if the flag is true
    if (moveAstronaut) {
        modelPosX += modelSpeed; // Move the astronaut to the right
        if (modelPosX > 3.0f) modelPosX = -3.0f; // Reset position after flying out
    }

    glutPostRedisplay();


    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void drawSphere(float radius, int slices, int stacks) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, radius, slices, stacks);
    gluDeleteQuadric(quad);
}

GLuint loadBMP(const char* filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to load BMP file: " << filePath << "\n";
        exit(1);
    }

    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    if (header[0] != 'B' || header[1] != 'M') {
        std::cerr << "Invalid BMP file\n";
        exit(1);
    }

    int dataOffset = *(int*)&header[10];
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = *(int*)&header[34];

    if (imageSize == 0) imageSize = width * height * 3;

    unsigned char* data = new unsigned char[imageSize];
    file.seekg(dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(data), imageSize);
    file.close();

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}




void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 0.5f }; // Dim ambient light
    GLfloat lightDiffuse[] = { 0.1f, 0.1f, 0.1f, 0.5f }; // Dim diffuse light


    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void handleKeyboard(unsigned char key, int x, int y) {
    if (key == 'l' || key == 'L') { // Existing toggle for moonlight
        moonLightOn = !moonLightOn;
        if (moonLightOn) glEnable(GL_LIGHT1);
        else glDisable(GL_LIGHT1);
    }
    else if (key == 13) { // Enter key
        moveAstronaut = !moveAstronaut;
    }
    else if (key == '+') { // Zoom In
        cameraZoom -= 0.5f;
        if (cameraZoom < 2.0f) cameraZoom = 2.0f; // Prevent over-zoom
    }
    else if (key == '-') { // Zoom Out
        cameraZoom += 0.5f;
        if (cameraZoom > 20.0f) cameraZoom = 20.0f; // Prevent over-zoom
    }
    else if (key == 'a' || key == 'A') { // Rotate Camera Left
        cameraAngleX -= 5.0f;
    }
    else if (key == 'd' || key == 'D') { // Rotate Camera Right
        cameraAngleX += 5.0f;
    }
}

// Handle special keys like arrow keys
void handleSpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT: // Move camera left
        cameraX -= cameraSpeed;
        break;
    case GLUT_KEY_RIGHT: // Move camera right
        cameraX += cameraSpeed;
        break;
    case GLUT_KEY_DOWN: // Move camera left
        cameraY -= cameraSpeed;
        break;
    case GLUT_KEY_UP: // Move camera right
        cameraY += cameraSpeed;
        break;
    }
    glutPostRedisplay(); // Request screen redraw
}
void drawMoonWithLight() {
    float moonX = 3.0f * cos(moonAngle);
    float moonZ = 3.0f * sin(moonAngle);

    // Set up moonlight
  // Set up moonlight
    if (moonLightOn) {
        GLfloat moonLightPosition[] = { moonX, 0.0f, moonZ, 1.0f };
        GLfloat moonLightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bright white light
        GLfloat spotDirection[] = { -moonX, 0.0f, -moonZ }; // Spotlight direction (towards center)

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, moonLightPosition);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, moonLightDiffuse);

        // Adjust spotlight parameters for larger, rounder light
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f); // Larger spotlight angle (more spread out)
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f); // Softer intensity distribution
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);

        // Adjust attenuation for broader range
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.005f);
    }

    // Draw moon with texture
    glPushMatrix();
    glTranslatef(moonX, 0.0f, moonZ);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    glEnable(GL_TEXTURE_2D);
    drawSphere(0.5f, 50, 50);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
