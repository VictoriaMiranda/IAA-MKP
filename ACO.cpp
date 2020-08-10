#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>  
#include <array>
#include <iterator>
#include <cstdlib>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace std;

int seed;
string name;
// n° objetos, restricciones, optimo, numero de hormigas, iteraciones
int n, m, opt, K, N, Beta; 
int stop = 0; // variable que limita el numero de evaluaciones
int F; // limite de funciones de evaluacion
float rho = 0.02; // tasa de evaporacion
float tau_max = 6.0;
float tau_min = 0.01;
vector<int> ganancias, capacidades;
vector<vector<int>> pesos;
vector<float> etas;

int evals = 0;

// funcion que lee la instancia entregada
void readFile(string filename) {
    ifstream infile;
    infile.open(filename);
    infile >> n >> m >> opt;
    int dato;

    for (int j = 0; j < n; j++) {
        infile >> dato;
        ganancias.push_back(dato);
    }
    for (int i = 0; i < m; i++) {
        vector<int> fila;
        for (int j = 0; j < n; j ++) {
            infile >> dato;
            fila.push_back(dato);
        }
        pesos.push_back(fila);
    }
    for (int i = 0; i < m; i++) {
        infile >> dato;
        capacidades.push_back(dato);
    }
    infile.close();
}

// funcion que verifica si solucion es factible
bool factible(vector<int> solucion_actual) {
    for (int i = 0; i < m; i++) {
        int suma = 0;
        for (int j = 0; j < n; j++) {
            suma += solucion_actual[j]*pesos[i][j];
        }
        if (suma > capacidades[i]) return false;
    }
    return true;
}

// funcion de evaluacion
int evaluation(vector<int> solucion_actual) {
    int suma = 0;
    evals += 1;
    if (evals == F) stop = 1;
    for (int j = 0; j < n; j++) {
        suma += solucion_actual[j]*ganancias[j];
    }
    return suma;
}

// dada una solucion parcial, entrega los objetos que aun pueden ingresar a la mochila
vector<int> Candidates(vector<int> actual) {
    vector<int> temp;
    vector<int> candidates(n,0);
    for(int j = 0; j < n; j++) {
        temp = actual;
        if (actual[j] == 0) {
            temp[j] = 1;
            if (factible(temp) == true) {
                candidates[j] = 1;
            }
        }
    }
    return candidates;
}

// suma las feromonas que existen en los arcos que unen al objeto i con cada objeto que se encuentra en la solucion que se esta construyendo
float suma_feromonas(int i, vector<int> s_k, vector<vector<float>> feromonas) {
    float suma = 0.0;
    for(int j = 0; j < n; j++) {
        if (s_k[j] == 1) {
            suma += feromonas[j][i];
        }
    }
    return (suma*etas[i]);
}

//funcion que calcula el denominador de la ecuacion de probabilidad de transicion
float p_denominador(vector<int> candidatos, vector<int> s_k, vector<vector<float>> feromonas) {
    float suma = 0.0;
    for(int c = 0; c < n; c++) {
        if (candidatos[c] == 1) {
            suma += suma_feromonas(c,s_k,feromonas);
        }
    }
    return suma;
}

// funcion que calcula el valor de eta del objeto j
float eta(int j, int Beta) {
    int p = ganancias[j];
    float recursos = 0.0;
    for(int i = 0; i < m; i++) {
        recursos += pesos[i][j];
    }
    return pow((p/recursos),Beta);
}

void Capture_Params(int argc, char **argv) {
    name = argv[1];
    K = atoi(argv[2]);
    F = atoi(argv[3]);
    Beta = atoi(argv[4]);
    seed = stoi(argv[5]);
}

// entero aleatorio entre 0 y number-1
int random_int(int number) {
    return (rand() % number);
}

// punto flotante aleatorio que va entre 0 y 1
float random_float() {
    return static_cast <float> (rand())/static_cast <float> (RAND_MAX);
}

// funcion que realiza Busqueda Local (tipo Hill Climbing, con operador random)
vector<vector<int>> LocalSearch(vector<int> solucion, int iteration_best) {
    int r,newBest;
    int half = n/2;
    vector<int> why;
    int number = 0;
    vector<int> s = solucion;
    vector<int> temp;

    while (number < 250) {
        temp = s; 
        r = random_int(half);
        temp[r] = pow(s[r] - 1,2);
        r = random_int(half) + half;
        temp[r] = pow(s[r] - 1,2);

        if (factible(temp) == true) {
            number += 1;
            newBest = evaluation(temp);
            if (stop == 1) {
                number = 250;
            }
            if (newBest > iteration_best) {
                s = temp;
                iteration_best = newBest;
            }
        }
        
    }

    why.push_back(iteration_best);
    return {s,why};
}

int main(int argc, char **argv) {
    Capture_Params(argc,argv);
    srand(seed);
    readFile(name);
    std::cout << "Instancia " << name << endl;
    std::cout << "n° objetos: " << n << ", n° restricciones: " << m << ", valor del optimo: " << opt << endl;

    for(int j = 0; j < n; j++) {
        etas.push_back(eta(j,Beta));
    }

    vector<int> best_global_ant(n,0);
    int best_global_benefit = 0;
    int r;
    float p1, p2, random, acum;
    vector<int> Candidatos(n,1);
    vector<vector<float>> feromonas(n,vector<float>(n,tau_max));
    
    int it = 0;

    auto start = std::chrono::system_clock::now();

    while (stop == 0) {
        vector<int> best_iteration_ant(n,0);
        int best_iteration_benefit = 0;
                
        for (int k = 0; k < K; k++) {   
            vector<int> s_actual(n,0);
            vector<float> taus_0(n,0);
            r = random_int(n);   
            s_actual[r] = 1;
            float denominador_k;
  
            Candidatos = Candidates(s_actual);
            while(count(Candidatos.begin(),Candidatos.end(),1) > 0) {
                random = random_float();
                acum = 0;

                denominador_k = p_denominador(Candidatos,s_actual,feromonas);
                for (int j = 0; j < n; j++) {
                    if (Candidatos[j] == 1) {
                        
                        taus_0[j] = taus_0[j] + feromonas[j][r];
                        p2 = (taus_0[j]*etas[j])/denominador_k;
                        
                        acum += p2;
                    }
                    if (acum > random) {
                        s_actual[j] = 1;
                        r = j; 
                        j = n;
                    }
                    
                }
 
                Candidatos = Candidates(s_actual);
            }
            
            int value = evaluation(s_actual);

            if (value > best_iteration_benefit) {
                best_iteration_ant = s_actual;
                best_iteration_benefit = value;
            }
            if (stop == 1) {
                k = K;
            }
        }

        // busqueda local
        if (stop == 0) {
            vector<vector<int>> resultado = LocalSearch(best_iteration_ant,best_iteration_benefit);
            best_iteration_ant = resultado[0];
            best_iteration_benefit = resultado[1][0];
        }

        if (best_iteration_benefit > best_global_benefit) {
            best_global_ant = best_iteration_ant;
            best_global_benefit = best_iteration_benefit;
            std::cout << "nuevo beneficio: " << best_global_benefit << endl;
        }


        // evaporacion de feromonas
        for (int i = 0; i < n-1; i++) {
            for (int j = i+1; j < n; j++) {
                feromonas[i][j] *= (1.0-rho);
                if (feromonas[i][j] < tau_min) {
                    feromonas[i][j] = tau_min;
                }
                feromonas[j][i] = feromonas[i][j];
            }
        }
            
        // intensificacion
        for (int j = 0; j < n-1; j++) {
            if (best_iteration_ant[j] == 1) {
                for (int k = j+1; k < n; k++) {
                    if (best_iteration_ant[k] == 1) {
                        feromonas[j][k] += 1.0/(1+best_global_benefit-best_iteration_benefit); 
                        if (feromonas[j][k] > tau_max) {
                            feromonas[j][k] = tau_max;
                        }
                        feromonas[k][j] = feromonas[j][k];
                    }
                }
            }
        }

        if (best_iteration_benefit == opt) break;
        it += 1;
    }
    auto end = std::chrono::system_clock::now();

    std::cout << "Mejor hormiga: " << endl;
    for (int j = 0; j < n; j++) {
        std::cout << best_global_ant[j];
    }
    std::cout << endl << "tiempo de ejecucion: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " [s]";
    std::cout << endl << "con beneficio: " << best_global_benefit << endl;
    std::cout << "Gap: " << 100.0*(opt - best_global_benefit)/opt << endl;
    std::cout << "Evaluaciones: " << evals << endl;
}