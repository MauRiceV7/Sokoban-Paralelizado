#include "GestorArchivos.h"
#include <iostream>

GestorArchivos::GestorArchivos(const std::string& nombreArchivo) : nombreArchivo(nombreArchivo) {}

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
    bool primerCaracter = true;
    if (archivo.is_open()) {
        std::string linea;
        //Marca el tiempo inicial
        auto startTime = std::chrono::high_resolution_clock::now();
        while (std::getline(archivo, linea)) {
            if (primerCaracter) {
                primerCaracter = false;
                if (!linea.empty()) {
                    linea = linea.substr(1);
                }
            }
            contenido += linea + '\n';
        }
        //Marca el tiempo final
        auto endTime = std::chrono::high_resolution_clock::now();
        //Calcula e imprime la duración
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        long long tiempoDurado = duracion.count();
        std::cout << "Tiempo durado en leer archivo: " << duracion.count() << " microsegundos" << std::endl;
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
