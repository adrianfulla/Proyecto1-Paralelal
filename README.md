# Proyecto 1 - Screensaver
## Universidad del Valle de Guatemala
### CC3069 - Computación paralela y distribuida
### Autores
- Emilio José Solano Orozco, 21212
- Adrian Fulladolsa Palma, 21592
- Elías Alberto Alvarado Raxón, 21808


## Descripción

 En este proyecto se tiene como objetivo crear un screensaver implementado con C++ y utilizando técnicas de paralelización para mejorar el rendimiento del programa. Para lograr esto, inicialmente se desarrolló una versión secuencial del programa en donde se crean N figuras con distintas formas, tamaños, colores y velocidad de movimiento. Luego de crearlas y agregarlas a la pantalla, existe una función la cuál está evaluando constantemente el comportamiento de esta; específicamente, verificando si ha colisionado con otra figura. En dado caso esto ocurra, se actualizan sus valores basados en distintos cálculos físicos para asegurar un comportamiento adecuado a la hora de colisionar con otro objeto. La siguiente fase fue utilizar este mismo código, pero ahora aplicando técnicas de paralelización apoyándonos con OpenMP y sus distintas directivas y cláusulas. Como se mencionó anteriormente, esto fue con el fin de mejorar el rendimiento del programa y tener un mejor resultado de ejecución. Para medir esto, se realizó una recolección de datos con las que se obtuvieron métricas como el speedup y efficiency.


## Prerequisitos
- Sistema Operativo Linux -> https://www.linux.org/pages/download/
- Compilador de C++/g++ -> Generalmente incluido en distros de Linux
- Git -> https://git-scm.com/downloads
- OpenMP -> https://www.openmp.org/specifications/
- OpenGL -> https://www.khronos.org/opengl/wiki/Getting_Started#Downloading_OpenGL
- GLFW -> https://www.glfw.org/download

## Instalación

1. Clonar repositorio 
```bash
git clone https://github.com/adrianfulla/Proyecto1-Paralelal.git
```

2. Entrar al directorio del repositorio
```bash
cd Proyecto1-Paralelal/
```

3. Compilar programa secuencial
```bash
g++ screensaver/sequential.cpp -o screensaver-seq -lglfw -lGL -lm
```

3. Compilar programa paralelo
```bash
g++ screensaver/parallel-3.cpp -o screensaver-par -lglfw -lGL -lm -fopenmp
```

## Ejecución de screensaver
Programa secuencial
 ```bash
./screensaver-seq <Numero de elementos>
```


Programa secuencial
 ```bash
./screensaver-par <Numero de elementos>
```

Reeemplazar `<Numero de elementos>` con la cantidad de objetos que se desean renderizar.

