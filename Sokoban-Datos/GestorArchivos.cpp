#include "GestorArchivos.h"
#include <iostream>
#include <omp.h>

GestorArchivos::GestorArchivos(const std::string& nombreArchivo) : nombreArchivo(nombreArchivo), mostrarTiemposConPausa(true) {}

bool GestorArchivos::abrir() {
    archivo.open(nombreArchivo);
    return archivo.is_open();
}

void GestorArchivos::copiarArchivo(const std::string& actual, const std::string& destino) {
    std::ifstream archivoOrigen(actual);
    if (!archivoOrigen.is_open()) {
        return;
    }

    std::ofstream archivoDestino(destino);
    if (!archivoDestino.is_open()) {
        return;
    }

    archivoDestino << archivoOrigen.rdbuf();

    archivoOrigen.close();
    archivoDestino.close();
}
//######### Metodo con medicion de tiempo #########
std::string GestorArchivos::leer() {
    std::string contenido;
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return contenido;
    }

    //Esto obtiene el tamano del archivo de inicio a fin
    archivo.seekg(0, std::ios::end);
    size_t tamano = archivo.tellg();
    archivo.seekg(0);
    if (tamano == 0) {
        std::cout << "Archivo vacio: " << nombreArchivo << std::endl;
        return "";
    }

    //Con esto se determina el numero de hilos a utilizar
    int numHilos = 1; //Si no usa hilos es secuencial (?)
    if (tamano >= 2048) numHilos = 2; //si el archivo es mayor a 2KB: 2 hilos
    if (tamano >= 4096) numHilos = 4; //mayor a 4KB usa 4 hilos
    if (tamano >= 8192) numHilos = omp_get_max_threads(); //Con un archivo mayor a 8KB --> mas grande que 150x150 usa todos los hilos
    std::cout << "Numero de hilos usados para leer archivo (" << nombreArchivo << ", " << tamano << " bytes): " << numHilos << std::endl;

    size_t chunkSize = tamano / numHilos;
    if (chunkSize < 512) chunkSize = 512; //este condicional evita el "overhead"
    std::vector<std::string> chunks(numHilos);

    auto startTime = std::chrono::high_resolution_clock::now();

#pragma omp parallel for num_threads(numHilos) //uso de openMP para el paralelismo
    for (int i = 0; i < numHilos; ++i) {
        std::ifstream fs(nombreArchivo); //cada hilo es su propio stream (fila)
        if (!fs) continue;

        size_t start = i * chunkSize;
        size_t end = (i == numHilos - 1) ? tamano : (i + 1) * chunkSize;

        //busca los saltos de linea para saber donde esta la division de los bloques
        if (start > 0) {
            fs.seekg(start);
            char ch;
            while (fs.get(ch) && fs.tellg() <= end) {
                if (ch == '\n') break;
            }
            start = fs.tellg();
        }
        else {
            fs.seekg(0);
        }

        std::string local;
        std::string linea;
        bool primerCaracter = (i == 0); //solo el primer hilo maneja este apartado del primercaracter
        while (fs.tellg() < end && fs.tellg() != -1 && std::getline(fs, linea)) {
            if (primerCaracter) {
                primerCaracter = false;
                if (!linea.empty()) {
                    linea = linea.substr(1); //para leer el archivo ignora el numero de nivel
                }
            }
            local += linea + '\n';
        }
        chunks[i] = local;
    }

    //concatenacion de resultados (por cada hilo)
    for (int i = 0; i < numHilos; ++i) {
        contenido += chunks[i];
    }

    // Medir y mostrar tiempo
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "Tiempo en leer archivo (paralelizado, " << numHilos << " hilos): "
        << duracion.count() << " microsegundos (" << (duracion.count() / 1000.0) << " ms)" << std::endl;
    if (mostrarTiemposConPausa) {
        std::cout << "Presione cualquier tecla para continuar..." << std::endl;
        _getch();
    }

    return contenido;
}

std::string GestorArchivos::leerGuardada() {
    std::string contenido;
    bool primerCaracter = true;
    if (archivo.is_open()) {
        std::string linea;
        while (std::getline(archivo, linea)) {
            contenido += linea + '\n';
        }
    }
    return contenido;
}

void GestorArchivos::limpiar() {
    if (archivo.is_open()) {
        archivo.close();
    }
    archivo.open(nombreArchivo, std::ios::out | std::ios::trunc);
}

bool GestorArchivos::existeArchivo(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo);
    return archivo.good(); //si existe algun .txt con ese nombre y se puede abrir, entonces es true
}

void GestorArchivos::escribir(const char& contenido) {
    if (archivo.is_open()) {
        archivo << contenido;
    }
}

char GestorArchivos::leerPrimerCaracter() {
    char primerCaracter = '\0';

    if (!archivo.is_open()) {
        std::cerr << "El archivo no fue abierto para lectura." << std::endl;
        return primerCaracter;
    }

    archivo.seekg(0);

    if (archivo.peek() != EOF) {
        archivo.get(primerCaracter);
    }

    if (archivo.fail()) {
        std::cerr << "Error de lectura del primer caracter." << std::endl;
        primerCaracter = '\0';
    }

    return primerCaracter;
}

void GestorArchivos::cerrar() {
    if (archivo.is_open()) {
        archivo.close();
    }
}
