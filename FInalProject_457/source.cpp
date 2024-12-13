#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
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

bool loadOBJ(const char* path) {
    FILE* file = fopen(path, "r");
    if (fopen_s(&file, path, "r") != 0 || !file) {
        std::cerr << "Cannot open OBJ file: " << path << "\n";
        return false;
    }

    std::vector<Vertex> tempVertices;
    std::vector<Vertex> tempNormals;
    std::vector<std::vector<int>> faces;

    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;

        if (strcmp(lineHeader, "v") == 0) { // Vertex position
            Vertex vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tempVertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vn") == 0) { // Normal vector
            Vertex normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tempNormals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) { // Face
            std::vector<int> faceVertices;
            int vIndex[3], vnIndex[3];

            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vIndex[0], &vnIndex[0], &vIndex[1], &vnIndex[1], &vIndex[2], &vnIndex[2]);
            if (matches == 6) {
                faceVertices.push_back(vIndex[0] - 1);
                faceVertices.push_back(vIndex[1] - 1);
                faceVertices.push_back(vIndex[2] - 1);
                faces.push_back(faceVertices);
            }
        }
    }
    fclose(file);

    // Copy the vertices into the main array
    for (const auto& v : tempVertices) {
        vertices.push_back(v);
    }

    // Handle face rendering (example)
    for (const auto& face : faces) {
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; ++i) {
            const Vertex& vertex = vertices[face[i]];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
        glEnd();
    }

    return true;
}

void drawOBJ() {
    glBegin(GL_POINTS); // Assume the OBJ contains triangles
    for (size_t i = 0; i < vertices.size(); i++) {
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    }
    glEnd();
}

void drawCustomSpaceModel() {
    // Set the color for stars
    glColor3f(1.0f, 1.0f, 1.0f); // Set color for stars (white)

    // Number of stars
    const int numStars = 20;
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
void renderText(const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Save current projection matrix
    glLoadIdentity(); // Reset projection matrix
    gluOrtho2D(0, WIDTH, 0, HEIGHT); // Set orthographic projection (screen coordinates)

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Save current modelview matrix
    glLoadIdentity(); // Reset modelview matrix

    glDisable(GL_LIGHTING); // Disable lighting so text is visible
    glColor3f(1.0f, 1.0f, 1.0f); // White text color

    // Set the position in screen coordinates (top-left)
    glRasterPos2i(10, HEIGHT - 20); // (x, y) in pixels

    // Render each character
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    glEnable(GL_LIGHTING); // Re-enable lighting

    // Restore matrices
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
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handleKeyboard);
    glutTimerFunc(0, timer, 0);
    if (!loadOBJ("astro.obj")) {
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
    renderText("Press L to turn on/off spotlight from the moon");
    // Render the flying astro.obj model
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Translate to the center (X, Y, Z = 0)
    glScalef(1.0f, 1.0f, 1.0f); // Scale up the model significantly
    glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for astronaut
    drawOBJ();
    glPopMatrix();

    // Set up the camera
    gluLookAt(0.0f, 0.0f, 8.5f,  // Move camera closer to the center (reduce Z value)
        0.0f, 0.0f, 0.0f,   // Look at the center
        0.0f, 1.0f, 0.0f);  // Up direction


    // Apply Earth's rotation
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    // Bind Earth texture and draw
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    glEnable(GL_TEXTURE_2D);
    drawSphere(1.0f, 50, 50);
    glDisable(GL_TEXTURE_2D);

    // Draw stars
    drawCustomSpaceModel();

    // Draw moon with texture and parametric curve
    drawMoonWithLight();

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
    if (key == 'l' || key == 'L') {
        moonLightOn = !moonLightOn;
        if (moonLightOn) {
            glEnable(GL_LIGHT1);
        }
        else {
            glDisable(GL_LIGHT1);
        }
    }
    else if (key == 13) { // Enter key has ASCII value 13
        moveAstronaut = !moveAstronaut; // Toggle movement on Enter key press
    }
}

void drawMoonWithLight() {
    float moonX = 3.0f * cos(moonAngle); //parametric curve equations
    float moonZ = 3.0f * sin(moonAngle);//parametric curve equations

    // Set up moonlight
    if (moonLightOn) {
        GLfloat moonLightPosition[] = { moonX, 0.0f, moonZ, 1.0f };
        GLfloat moonLightDiffuse[] = { 155.5f, 155.5f, 155.2f, 155.0f };

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, moonLightPosition);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, moonLightDiffuse);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0f); // Spotlight cutoff angle (smaller angle = more focused light)
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f); // Controls intensity distribution (higher = more concentrated)
        GLfloat spotDirection[] = { -moonX, 0.0f, -moonZ }; // Direction of the spotlight (towards the center)
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);  // Constant factor
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);   // Linear attenuation
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05f); // Quadratic attenuation (slightly fades with distance)


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
