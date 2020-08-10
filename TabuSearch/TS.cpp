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
// n° objetos, restricciones, optimo, largo de lista tabu, iteraciones
int n, m, opt, t, N;
vector<int> ganancias, capacidades;
vector<vector<int>> pesos;
//lista tabu
vector<string> tl;
int evals = 0;

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

// solucion inicial aleatoria
vector<int> ramdomInicialization() {
    bool f = false;
    vector<int> v;
    while(f==false) {
        for (int j = 0; j < n; j++) {
            int r = rand() % 2;
            v.push_back(r);
        }
        //verifica factibilidad
        if (factible(v)==true) f = true;
        else v.clear();
    }
    std::cout << "Solucion inicial" << endl;
    for (int j = 0; j < n; j++) {
        std::cout << v[j] << " ";
    }
    std::cout << endl;
    return v;
}

// algortimo greedy usado en informe 1, se puede usar como solucion inicial
vector<int> greedy_avg() {
    vector<double> eficiencia(n,0);
    vector<int> mochila(n,0);
    int max_element_index;
    vector<int> acumulado(m,0);
    int FO = 0;

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            eficiencia[j] += pesos[i][j];
        }
    }
    for (int j = 0; j < n; j++) {
        eficiencia[j] = ganancias[j]/eficiencia[j];
    }
    for (int j = 0; j < n; j++) {
        max_element_index = max_element(eficiencia.begin(),eficiencia.end()) - eficiencia.begin();
        eficiencia[max_element_index] = 0;
        mochila[max_element_index] = 1;
        //si es factible, agregalo a la mochila 
        if (factible(mochila) == true) {
            for (int i = 0; i < m; i++) {
                acumulado[i] += pesos[i][max_element_index];
            }
            mochila[max_element_index] = 1;
            FO += ganancias[max_element_index];
        }
        else mochila[max_element_index] = 0;
    }
    std::cout << "Solucion inicial" << endl;
    for (int j = 0; j < n; j++) {
        std::cout << mochila[j] << " ";
    }
    std::cout << endl;
    return mochila;
}

// funcion de evaluacion
int evaluation(vector<int> solucion_actual) {
    int suma = 0;
    evals += 1;
    for (int j = 0; j < n; j++) {
        suma += solucion_actual[j]*ganancias[j];
    }
    return suma;
}

// funcion que construye el vecindario con 2-bitflips dada una solucion
vector<vector<int>> Neighborhood(vector<int> actual) {
    vector<int> neighbor0;
    vector<int> neighbor;
    int temporal_value;
    int temporal_move1;
    int temporal_move2;
    int temp_best_value = 0;
    vector<vector<int>> result;
    bool flag = false;
    string a;

    for (int j = 0; j < n-1; j++) {
        a = to_string(j);
        neighbor0 = actual;
        neighbor0[j] = pow(actual[j] - 1,2);
        for (int jj = j + 1; jj < n; jj++) {
            
            //if (evals == 100000) break;

            neighbor = neighbor0;
            neighbor[jj] = pow(actual[jj] - 1,2);
            string b = a;
            b.append("-");
            b.append(to_string(jj)); // string con los dos bits que cambiaron
            
            if (count(tl.begin(),tl.end(),b) == 0) {
                if (factible(neighbor) == true) {
                    flag = true;
                    temporal_value = evaluation(neighbor);
                    if (temporal_value > temp_best_value) {
                        temp_best_value = temporal_value;
                        temporal_move1 = j;
                        temporal_move2 = jj;
                    }
                }
            }
        }
    }
    if (flag == false) return {{-1}}; 

    neighbor = actual;
    neighbor[temporal_move1] = pow(actual[temporal_move1] - 1,2);
    neighbor[temporal_move2] = pow(neighbor[temporal_move2] - 1,2);
    result.push_back(neighbor);
    result.push_back({temp_best_value,temporal_move1,temporal_move2});

    return result;
}

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

void Capture_Params(int argc, char **argv) {
    name = argv[1];
    t = atoi(argv[2]);
    N = atoi(argv[3]);
    seed = stoi(argv[4]);
}

int main(int argc, char **argv) {
    Capture_Params(argc,argv);
    srand(seed);
    readFile(name);
    std::cout << "Instancia " << name << endl;
    std::cout << "n° objetos: " << n << ", n° restricciones: " << m << ", valor del optimo: " << opt << endl;

    // Empieza Tabu Search
    vector<int> valores;

    auto start = std::chrono::system_clock::now();
    //solucion inicial aleatoria
    vector<int> actual = ramdomInicialization();

    //solucion con greedy
    //vector<int> actual = greedy_avg();

    //construye vecindario
    vector<int> ultimate_best = actual;
    int ultimate_value = evaluation(ultimate_best);
    int best_value = ultimate_value;
    int move = -1;
    int move_value = -1;
    int it = 0;

    std::cout << "Beneficio actual: " << ultimate_value << endl;

    while(it < N) {
        
        //if (evals >= 100000) break;

        vector<vector<int>> neighborhood = Neighborhood(actual);
        if (neighborhood[0][0] == -1) break; // no existe solucion factible o no prohibida

        actual = neighborhood[0]; // mejor solucion del vecindario
        best_value = neighborhood[1][0]; // beneficio de esta solucion

        //bits que cambiaron se ingresan a lista tabu
        string str(to_string(neighborhood[1][1])); 
        str.append("-");
        str.append(to_string(neighborhood[1][2]));
        if (tl.size() < t) {        
            tl.push_back(str);
        }
        else {                      
            tl.erase(tl.begin());
            tl.push_back(str);
        }
        if (best_value > ultimate_value) {
            ultimate_value = best_value;
            ultimate_best = actual;
        }
        it += 1;
    }
    auto end = std::chrono::system_clock::now();

    std::cout << "Mejor solucion: " << endl;
    for (int j = 0; j < n; j++) {
        std::cout << ultimate_best[j] << " ";
    }
    std::cout << endl << "Beneficio: " << ultimate_value;
    std::cout << endl << "gap: " << (100.0*(opt - ultimate_value))/opt;
    std::cout << endl << "lista tabu" << endl;
    for (int k = 0; k < t; k++) {
        std::cout << tl[k] << " ";
    }
    std::cout << endl << "tiempo de ejecucion: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " [ms]";
    std::cout << endl << "numero de evaluaciones: " << evals << endl;

}