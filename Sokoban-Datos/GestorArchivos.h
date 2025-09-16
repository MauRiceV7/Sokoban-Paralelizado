#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include <fstream>

class GestorArchivos {
public:
    GestorArchivos(const std::string& nombreArchivo = "");
    bool abrir();
    std::string leer();
    std::string leerGuardada();
    void limpiar();
    void copiarArchivo(const std::string& origen, const std::string& destino);
    void escribir(const char& contenido);
    char leerPrimerCaracter();
    void cerrar();
    static bool existeArchivo(const std::string&);

private:
    std::string nombreArchivo;
    std::fstream archivo;
};

#endif // GESTORARCHIVOS_H