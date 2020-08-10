# IAA-MKP
En este repositorio se encuentra una implementación para resolver MKP usando un algoritmo greedy, un Tabu Search, y un Ant Colony Optimization. 

En la carpeta Greedy se encuentra el código del primer informe y su makefile. Al ejecutar el makefile tener cuidado de tener las carpetas de las instancias en el mismo directorio que el archivo cpp.

Luego en la carpeta TabuSearch está la implementación de TS de la segunda entrega. Los parámetros que toma el programa son: ./TS name t N seed. Donde name: ruta del archivo, t: largo de lista tabu, N: número de iteraciones, y seed: semilla que usará el programa.

Finalmente, el archivo ACO.cpp contiene la implementacion de Ant Colony System de la tercera entrega. Los parámetros del programa son ./aco name K F beta seed. Donde name: K: es el número de hormigas, F: el número de evaluaciones a la función de evaluación. Los demás parámetros se explicaron en el párrafo anterior.

Para ejecutar el código ir a la carpeta donde se encuentra el proyecto y escribir make por consola. Luego ingresar make run.

Las instancias se encuentran en la carpeta gk y pb.
