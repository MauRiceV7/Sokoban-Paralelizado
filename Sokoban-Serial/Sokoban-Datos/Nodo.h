#pragma once
#include "Utilities.h"
#include <stack>
#include <vector>
#include <iostream>
#include <sstream>
#include <thread> //para el this_thread
#include <chrono>
class Nodo {
private:
    Nodo* izquierda;
    Nodo* derecha;
    Nodo* arriba;
    Nodo* abajo;

    char simbolo;

public:
    Nodo(char simbolo) : simbolo{ simbolo }, arriba{ nullptr }, derecha{ nullptr }, izquierda{ nullptr }, abajo{ nullptr } {}

    void setIzquierda(Nodo*);
    void setDerecha(Nodo*);
    void setArriba(Nodo*);
    void setAbajo(Nodo*);
    void setSimbolo(char);

    Nodo* getIzquierda();
    Nodo* getDerecha();
    Nodo* getArriba();
    Nodo* getAbajo();
    char getSimbolo();

    ~Nodo();
};






