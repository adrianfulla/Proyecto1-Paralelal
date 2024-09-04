/**
 * @file parallel.cpp
 * @author Adrian Fulladolsa, Elías Alvarado, Emilio Solano
 * @brief Implementación de un screensaver de figuras geométricas utilizando OpenGL y GLFW (versión paralelizada)
 * @date 04-09-2004
 * 
 * 
 */

#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <omp.h>


// Declaración de variables globales
// Pantalla
const int WIDTH = 640;
const int HEIGHT = 480;

// Parámetros de la cámara
const float FOV = 90.0f;
const float NEAR_PLANE = 0.1f; // Distancias de los planos
const float FAR_PLANE = 10.0f;

// Estructura para una figura
struct Shape {
    int sides; // Número de lados de la figura
    float radius; // Radio de la figura
    float x, y, z; // Posición de la figura
    float vx, vy, vz; // Velocidad de la figura en cada eje
    float rX, rY, rZ; // Rotación de la figura en cada eje
    float rotationSpeed; // Velocidad de rotación de la figura
    float r, g, b; // Color de la figura
    Shape(): rX(0.0f), rY(0.0f), rZ(0.0f), rotationSpeed(0.0f) {} // Constructor por defecto

    // Función para cambiar el color de la figura
    void changeColor() {
        this->r = static_cast<float>(rand()) / RAND_MAX;
        this->g = static_cast<float>(rand()) / RAND_MAX;
        this->b = static_cast<float>(rand()) / RAND_MAX;
    }
};

std::vector<Shape> shapes; // Vector en el que se almacenan las figuras
std::vector<double> frameTimes; // Vector en el que se almacenan los tiempos de los frames


/*
    *** Funciones para dibujar y actualizar las figuras ***
    * generateShapes: Genera figuras aleatorias y las almacena en el vector shapes
    * drawShape: Dibuja una figura en la pantalla
    * applyPerspective: Aplica la perspectiva a una figura
    * updateShape: Actualiza la posición de una figura
*/

// Función para aplicar la perspectiva a una figura
void applyPerspective(float &x, float &y, float z) {
    float fovFactor = 1.0f / tanf(FOV * 0.5f * (M_PI / 180.0f));
    x *= fovFactor / z;
    y *= fovFactor / z;
}

void drawShape(const Shape &shape) {
    glPushMatrix(); // Se agrega una matriz para la figura, con la que se puede rotar y trasladar el objeto
    glRotatef(shape.rX, 1.0f, 0.0f, 0.0f);
    glRotatef(shape.rY, 0.0f, 1.0f, 0.0f);
    glRotatef(shape.rZ, 0.0f, 0.0f, 1.0f);

    glBegin(GL_POLYGON); // Se inicia un dibujo de un polígono
    glColor3f(shape.r, shape.g, shape.b);

    for (int i = 0; i < shape.sides; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(shape.sides);
        float px = shape.x + shape.radius * cos(angle);
        float py = shape.y + shape.radius * sin(angle);
        float pz = shape.z;
        applyPerspective(px, py, pz);
        glVertex2f(px, py);
    }
    glEnd(); // Se finaliza el dibujo del polígono

    glPopMatrix(); // Se saca de la pila la matriz de la figura dibujada
}

// Función para actualizar los componentes de una figura
void updateShape(Shape &shape, float dt) {
    // Movimiento de la figura según su velocidad y un tiempo delta
    shape.x += shape.vx * dt;
    shape.y += shape.vy * dt;
    shape.z += shape.vz * dt;

    // Rotación de la figura según su velocidad de rotación y un tiempo delta
    shape.rX += shape.rotationSpeed * dt;
    shape.rY += shape.rotationSpeed * dt;
    shape.rZ += shape.rotationSpeed * dt;

    // Si una figura llega a un ángulo de rotación múltiplo de 90 grados, se le suma 1 grado para evitar que "desaparezca"
    if (fmod(shape.rX, 90.0f) < 1.0f) {
        shape.rX += 1.0f;
    }
    if (fmod(shape.rY, 90.0f) < 1.0f) {
        shape.rY += 1.0f;
    }
    if (fmod(shape.rZ, 90.0f) < 1.0f) {
        shape.rZ += 1.0f;
    }

    // Manejo de colisiones con los bordes de la pantalla
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

    // Verificación de colisiones entre figuras
    for (auto &other : shapes) {
        if (&shape == &other) continue; // No se verifica la colisión con la misma figura

        float dx = shape.x - other.x;
        float dy = shape.y - other.y;
        float dz = shape.z - other.z;
        float dist = sqrt(dx * dx + dy * dy + dz * dz); // Distancia entre las figuras
        if (dist < shape.radius + other.radius) {
            // Un overlap simple para separar las figuras
            float overlap = 0.5f * (dist - shape.radius - other.radius);

            // Separar las figuras para evitar que se superpongan
            shape.x -= overlap * (dx) / dist;
            shape.y -= overlap * (dy) / dist;
            shape.z -= overlap * (dz) / dist;
            other.x += overlap * (dx) / dist;
            other.y += overlap * (dy) / dist;
            other.z += overlap * (dz) / dist;

            // Intercambiar velocidades
            std::swap(shape.vx, other.vx);
            std::swap(shape.vy, other.vy);
            std::swap(shape.vz, other.vz);

            // Cambiar color o cambiar la velocidad de rotación de la figura con una probabilidad
            float p = static_cast<float>(rand()) / RAND_MAX;
            if (p < 0.1f) {
                shape.rotationSpeed = 70.0f * (static_cast<float>(rand()) / RAND_MAX);
            }
            if (p < 0.05f) {
                shape.changeColor();
            }
        }
    }
}

// Función para generar figuras aleatorias
void generateShapes(int N) {
    shapes.clear(); // Se limpia el vector de figuras
    for (int i = 0; i < N; ++i) {
        Shape shape; // Se crea una figura
        shape.sides = rand() % 3 + 3; // Genera una cantidad de lados entre 3 y 5 que determina el tipo de figura
        shape.radius = 0.05f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.1f)); // Genera un radio que determina el tamaño de la figura
        shape.x = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)); // Genera una posición en x
        shape.y = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f)); // Genera una posición en y
        shape.z = NEAR_PLANE + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (FAR_PLANE - NEAR_PLANE))); // Genera una posición en z
        shape.vx = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f); // Genera una velocidad en x
        shape.vy = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f); // Genera una velocidad en y
        shape.vz = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f); // Genera una velocidad en z
        // Genera un color aleatorio para la figura
        shape.r = static_cast<float>(rand()) / RAND_MAX;
        shape.g = static_cast<float>(rand()) / RAND_MAX; 
        shape.b = static_cast<float>(rand()) / RAND_MAX;
        shapes.push_back(shape);
    }
}

// Función para obtener la hora actual como una cadena
std::string getCurrentTimeAsString() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

// Función para guardar los tiempos de los frames en un archivo CSV
void saveFrameTimesToCSV(const double& time, int shapeCount, std::string filenamee) {
    std::filesystem::create_directory("plogs");

    std::string fileName = "plogs/" + filenamee + "-" + std::to_string(shapeCount) + ".csv";
    std::ofstream outFile(fileName, std::ios_base::app);
    if (outFile.is_open()) {
        outFile << time << "\n";
        outFile.close();
        //std::cout << "Frame times appended to " << fileName << "\n";
    } else {
        std::cerr << "Failed to open file for writing: " << fileName << "\n";
    }
}

int main(int argc, char** argv) {
    std::string file = getCurrentTimeAsString();
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_shapes>\n";
        return -1;
    }

    int N = std::atoi(argv[1]); // Número de figuras
    if (N <= 0) {
        std::cerr << "Number of shapes must be greater than 0.\n";
        return -1;
    }

    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL 3D Screensaver", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Configurar OpenGL
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Habilita vsync
    glDisable(GL_CULL_FACE); // Deshabilita el culling de caras

    // Crea un generador de números aleatorios
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    generateShapes(N);

    // Variables para medir el tiempo de los frames
    double previousTime = glfwGetTime();
    double lastTime = previousTime;
    double startTime, endTime;
    int frameCount = 0;

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            double fps = double(frameCount) / (currentTime - lastTime);

            // Actualizar el título de la ventada con los FPS actuales
            std::ostringstream ss;
            ss << "OpenGL 3D Screensaver - FPS: " << fps;
            glfwSetWindowTitle(window, ss.str().c_str());

            frameCount = 0;
            lastTime = currentTime;
        }

        glClear(GL_COLOR_BUFFER_BIT); // Limpia el buffer de color

        startTime = glfwGetTime(); // Se mide el tiempo de inicio del frame
	

    	for (auto &shape : shapes) {
        	drawShape(shape);
    	}
	
	#pragma omp parallel for
        for (auto &shape : shapes) {
            updateShape(shape, dt);
        }

        endTime = glfwGetTime(); // Se mide el tiempo de fin del frame

        saveFrameTimesToCSV(endTime - startTime, N, file); // Se almacena el tiempo del frame

        // Intercambia los buffers y procesa los eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Destruir la ventana y terminar GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    //saveFrameTimesToCSV(frameTimes, N);

    return 0;
}
