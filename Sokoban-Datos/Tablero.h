#pragma once
#include "Nodo.h"
#include "GestorArchivos.h"
#include <list>
#include <mutex>
#include <conio.h>


class Tablero {
public:
    Tablero();
    void inicializarTablero(int, int);
    void imprimirTableroParalelizado();
    void procesarLinea(Nodo*, std::string, std::list<char>&, std::mutex&);
    void insertarSimbolo(int, int, char);
    void moverJugador(bool jugarPG);
    void movimiento(int&, int&, char);
    Nodo* encontrarNodo(int, int);
    void cambiarSimbolos(Nodo*&, Nodo*&, Nodo*&, char, char, char, int, int, int&, int&);
    void guardarNivel(std::string nombreArchivo);
    void jugar(char tecla);

    int filas;
    int columnas;
    int filaJugador;
    int columnaJugador;
    int cuentaPuntos;
    bool mostrarTiemposConPausa;
    std::stack<int> contadorCajasEnPos;
    std::vector<char> movimientosRealizados;
    Nodo* inicio; // siempre apunta al inicio del "grid"
    Nodo* actual; // se mueve por el "grid"
    GestorArchivos nivelJuego;
};