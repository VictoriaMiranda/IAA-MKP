#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>  
#include <array>
#include <iterator>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <algorithm> 

using namespace std;

string name;
int n, m, opt;
vector<int> ganancias, capacidades;
vector<vector<int>> pesos;

bool factible(int indice_actual, vector<int> acumulado) {
    for (int i = 0; i < m; i++) {
        if (acumulado[i]+pesos[i][indice_actual] > capacidades[i]) {
            return false;
        }
    }
    return true;
}

void readFile(string filename) {
    ifstream infile;
    infile.open(filename);
    infile >> n >> m >> opt;
    int dato;
    //cout << "y aqui? " << n << m << opt << endl;
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
        //cout << eficiencia[i] << " ";
    }
    for (int j = 0; j < n; j++) {
        max_element_index = max_element(eficiencia.begin(),eficiencia.end()) - eficiencia.begin();
        eficiencia[max_element_index] = 0;
        //si es factible, agregalo a la mochila 
        if (factible(max_element_index,acumulado) == true) {
            for (int i = 0; i < m; i++) {
                acumulado[i] += pesos[i][max_element_index];
            }
            mochila[max_element_index] = 1;
            FO += ganancias[max_element_index];
        }
    }
    cout << "Beneficio total de greedy con average: " << FO << endl;
    return mochila;
}

vector<int> greedy_basic() {
    vector<int> mochila(n,0);
    int max_element_index;
    vector<int> acumulado(m,0);
    int FO = 0;
    for (int j = 0; j < n; j++) {
        max_element_index = max_element(ganancias.begin(),ganancias.end()) - ganancias.begin();
        if (factible(max_element_index,acumulado) == true) {
            for (int i = 0; i < m; i++) {
                acumulado[i] += pesos[i][max_element_index];
            }
            mochila[max_element_index] = 1;
            FO += ganancias[max_element_index];
        }
        ganancias[max_element_index] = 0;
    }
    cout << endl << "Beneficio total de greedy basico: " << FO << endl;
    return mochila;
}

void Capture_Params(int argc, char **argv) {
    name = argv[1];
}

int main(int argc, char **argv) {
    Capture_Params(argc,argv);
    readFile(name);
    cout << "Instancia " << name << endl;
    cout << "n° objetos: " << n << ", n° restricciones: " << m << ", valor del optimo: " << opt << endl;
    // Empieza Greedy
    vector<int> solucion;
    auto start1 = std::chrono::system_clock::now();
    solucion = greedy_avg();
    auto end1 = std::chrono::system_clock::now();
    cout << "tiempo del greedy mejorado: " << std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count() << " microsegundos" << endl;

    for (int j = 0; j < n; j ++) {
        cout << solucion[j] << " ";
    }
    auto start = std::chrono::system_clock::now();
    solucion = greedy_basic();
    auto end = std::chrono::system_clock::now();

    cout << "tiempo del greedy basico: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microsegundos"<<endl;

    for (int j = 0; j < n; j ++) {
        cout << solucion[j] << " ";
    }
}
