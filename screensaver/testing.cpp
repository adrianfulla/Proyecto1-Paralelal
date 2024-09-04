/**
 * @file parallel.cpp
 * @author Adrian Fulladolsa, Elías Alvarado, Emilio Solano
 * @brief Ejecución del archivo parallel.cpp 10 veces con duración de alrededor de 30 segundos y con un valor 				
 constante de N en cada iteración.
 * @date 04-09-2004
 * 
 * 
 */
 
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    for (int i = 0; i < 10; i++) {
        std::cout << "Starting iteration " << i + 1 << std::endl;
        
        // Fork para proceso hijo
        pid_t pid = fork();

        if (pid == 0) {
            execl("./screensaver", "./screensaver", "1000", (char*)nullptr);
            std::cerr << "Failed to start screensaver" << std::endl;
            return 1;
        } else if (pid > 0) {
            sleep(30);

            // Terminar el proceso hijo
            kill(pid, SIGTERM);
            
            // Esperar a que el proceso hijo termine
            waitpid(pid, nullptr, 0);

            std::cout << "Iteration " << i + 1 << " completed" << std::endl;
        } else {
            std::cerr << "Fork failed" << std::endl;
            return 1;
        }
    }
    return 0;
}

