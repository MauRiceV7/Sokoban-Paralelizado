#include "Nodo.h"

// Getters
Nodo* Nodo::getIzquierda() { return izquierda; }
Nodo* Nodo::getDerecha() { return derecha; }
Nodo* Nodo::getArriba() { return arriba; }
Nodo* Nodo::getAbajo() { return abajo; }
char Nodo::getSimbolo() { return simbolo; }

// Setters
void Nodo::setIzquierda(Nodo* nuevoIzquierda) { izquierda = nuevoIzquierda; }
void Nodo::setDerecha(Nodo* nuevoDerecha) { derecha = nuevoDerecha; }
void Nodo::setArriba(Nodo* nuevoArriba) { arriba = nuevoArriba; }
void Nodo::setAbajo(Nodo* nuevoAbajo) { abajo = nuevoAbajo; }
void Nodo::setSimbolo(char nuevoSimbolo) { simbolo = nuevoSimbolo; }

Nodo::~Nodo() {
    if (izquierda != nullptr) delete izquierda;
    if (derecha != nullptr) delete derecha;
    if (arriba != nullptr) delete arriba;
    if (abajo != nullptr) delete abajo;
}