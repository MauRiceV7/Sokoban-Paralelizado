#include "Tablero.h"

int main() {
    int opc = 0;
    Tablero* TableroL = new Tablero();
    do {
        std::cout << "[1] Jugar nueva partida         \n";
        std::cout << "[2] Cargar una partida guardada \n";
        std::cout << "[3] Salir del programa          \n";
        std::cout << "Su respuesta: "; std::cin >> opc;
        system("cls");

        switch (opc) {
        case 1: {
            TableroL->moverJugador(false);
            system("cls");
        } break;

        case 2: {
			TableroL->moverJugador(true);
            system("cls");
        } break;

        case 3: {
            std::cout << "Gracias por jugar\n\n" << "Estudiantes \n";
            std::cout << "[1] Mauricio Vargas Vicarioli \n" << "[2] Ignacio Ledezma Hidalgo \n\n"; break;
        }

        default: std::cerr << "Esa opcion no existe\n"; break;
        }

    } while (opc == 1 || opc == 2);

    system("pause");
    return 0;
}