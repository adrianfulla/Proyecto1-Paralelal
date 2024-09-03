#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

const int WIDTH = 640;
const int HEIGHT = 480;

const float FOV = 90.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 10.0f;

struct Shape {
    int sides;
    float radius;
    float x, y, z;
    float vx, vy, vz;
    float r, g, b;
};

std::vector<Shape> shapes;
std::vector<double> frameTimes;

void applyPerspective(float &x, float &y, float z) {
    float fovFactor = 1.0f / tanf(FOV * 0.5f * (M_PI / 180.0f));
    x *= fovFactor / z;
    y *= fovFactor / z;
}

void drawShape(const Shape &shape) {
    glBegin(GL_POLYGON);
    glColor3f(shape.r, shape.g, shape.b);

    for (int i = 0; i < shape.sides; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(shape.sides);
        float px = shape.x + shape.radius * cos(angle);
        float py = shape.y + shape.radius * sin(angle);
        float pz = shape.z;
        applyPerspective(px, py, pz);
        glVertex2f(px, py);
    }
    glEnd();
}

void updateShape(Shape &shape, float dt) {
    shape.x += shape.vx * dt;
    shape.y += shape.vy * dt;
    shape.z += shape.vz * dt;

    // Border Collision Handling in 3D
    float projectedRadiusX = shape.radius / shape.z;
    float projectedRadiusY = shape.radius / shape.z;

    if (shape.x - projectedRadiusX < -1.0f) {
        shape.x = -1.0f + projectedRadiusX;
        shape.vx *= -1;
    }
    if (shape.x + projectedRadiusX > 1.0f) {
        shape.x = 1.0f - projectedRadiusX;
        shape.vx *= -1;
    }
    if (shape.y - projectedRadiusY < -1.0f) {
        shape.y = -1.0f + projectedRadiusY;
        shape.vy *= -1;
    }
    if (shape.y + projectedRadiusY > 1.0f) {
        shape.y = 1.0f - projectedRadiusY;
        shape.vy *= -1;
    }
    if (shape.z - shape.radius < NEAR_PLANE) {
        shape.z = NEAR_PLANE + shape.radius;
        shape.vz *= -1;
    }
    if (shape.z + shape.radius > FAR_PLANE) {
        shape.z = FAR_PLANE - shape.radius;
        shape.vz *= -1;
    }

    // Check for collisions with other shapes in 3D space
    for (auto &other : shapes) {
        if (&shape == &other) continue;

        float dx = shape.x - other.x;
        float dy = shape.y - other.y;
        float dz = shape.z - other.z;
        float dist = sqrt(dx * dx + dy * dy + dz * dz);
        if (dist < shape.radius + other.radius) {
            // Simple elastic collision response in 3D
            float overlap = 0.5f * (dist - shape.radius - other.radius);

            // Separate the shapes so they don't overlap
            shape.x -= overlap * (dx) / dist;
            shape.y -= overlap * (dy) / dist;
            shape.z -= overlap * (dz) / dist;
            other.x += overlap * (dx) / dist;
            other.y += overlap * (dy) / dist;
            other.z += overlap * (dz) / dist;

            // Swap velocities in 3D
            std::swap(shape.vx, other.vx);
            std::swap(shape.vy, other.vy);
            std::swap(shape.vz, other.vz);
        }
    }
}

void generateShapes(int N) {
    shapes.clear();
    for (int i = 0; i < N; ++i) {
        Shape shape;
        shape.sides = rand() % 3 + 3; // Generate between 3 and 5 sides
        shape.radius = 0.05f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.1f));
        shape.x = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        shape.y = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        shape.z = NEAR_PLANE + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (FAR_PLANE - NEAR_PLANE)));
        shape.vx = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        shape.vy = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        shape.vz = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        shape.r = static_cast<float>(rand()) / RAND_MAX;
        shape.g = static_cast<float>(rand()) / RAND_MAX;
        shape.b = static_cast<float>(rand()) / RAND_MAX;
        shapes.push_back(shape);
    }
}

std::string getCurrentTimeAsString() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

void saveFrameTimesToCSV(const std::vector<double>& frameTimes, int shapeCount) {
    std::string fileName = getCurrentTimeAsString() + "-" + std::to_string(shapeCount) + ".csv";
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        outFile << "Frame Time (s)\n";
        for (const auto& time : frameTimes) {
            outFile << time << "\n";
        }
        outFile.close();
        std::cout << "Frame times saved to " << fileName << "\n";
    } else {
        std::cerr << "Failed to open file for writing: " << fileName << "\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_shapes>\n";
        return -1;
    }

    int N = std::atoi(argv[1]);
    if (N <= 0) {
        std::cerr << "Number of shapes must be greater than 0.\n";
        return -1;
    }

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL 3D Screensaver", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    generateShapes(N);

    double previousTime = glfwGetTime();
    double lastTime = previousTime;
    int frameCount = 0;

    double startTime, endTime;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            double fps = double(frameCount) / (currentTime - lastTime);

            std::ostringstream ss;
            ss << "OpenGL 3D Screensaver - FPS: " << fps;
            glfwSetWindowTitle(window, ss.str().c_str());

            frameCount = 0;
            lastTime = currentTime;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        startTime = glfwGetTime();
        for (auto &shape : shapes) {
            updateShape(shape, dt);
            drawShape(shape);
        }
        endTime = glfwGetTime();

        frameTimes.push_back(endTime - startTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    saveFrameTimesToCSV(frameTimes, N);
    return 0;
}
