#include "Tablero.h"
char nivel;

Tablero::Tablero() : filas(0), columnas(0), inicio(nullptr), actual(nullptr) {
    cuentaPuntos = 0;
    columnaJugador = 0;
    filaJugador = 0;
    nivelJuego = GestorArchivos();
}

void Tablero::inicializarTablero(int filas, int columnas) {
    this->filas = filas;
    this->columnas = columnas;
    Nodo* fila = nullptr; // Almacena inicio de fila
    Nodo* filaSuperior = nullptr; // Almacena fila superior
    Nodo* actualSuperior = nullptr; // Camina fila superior

    //Marca el tiempo inicial
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (i == 0 && j == 0) { // Si estoy en el inicio
                inicio = new Nodo(' ');
                actual = inicio;
                fila = inicio;
                filaSuperior = inicio;
            }
            else { // Si ya me movi de posicion, por ejemplo - X - - - -
                Nodo* nuevoS = new Nodo(' ');
                actual->setDerecha(nuevoS); // Actual apunta a inicio en la primera iteracion
                nuevoS->setIzquierda(actual); // El getIzquierda de nuevo es actual, que apunta a inicio
                actual = nuevoS;

                // Conexion de los nodos vecinos (arriba, abajo, izquierda y derecha)
                if (i > 0 && j == 0) {
                    Nodo* nuevo = new Nodo(' ');
                    fila->setAbajo(nuevo);
                    nuevo->setArriba(fila);
                    filaSuperior = fila;
                    fila = nuevo;
                    actual = fila;
                }

                if (i > 0 && j > 0) {
                    actualSuperior = filaSuperior->getDerecha();
                    actual->setArriba(actualSuperior);
                    actualSuperior->setAbajo(actual);
                    actualSuperior = actualSuperior->getDerecha();
                }
                // Ahora, actual vale lo que vale nuevo, que es el siguiente de inicio
            }
        }
    }
    //Marca el tiempo final
    auto endTime = std::chrono::high_resolution_clock::now();
    //Calcula e imprime la duración
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    long long tiempoDurado = duracion.count();
    std::cout << "Tiempo durado en inicializar el tablero: " << duracion.count() << " millisegundos" << std::endl;

}

void Tablero::insertarSimbolo(int fila, int columna, char simbolo) {
    // Fila y columna dentro de los limites del tablero
    if (fila >= 0 && fila < filas && columna >= 0 && columna < columnas) {
        Nodo* nodo = encontrarNodo(fila, columna);
        nodo->setSimbolo(simbolo);
    }
    else {
        //Si las filas o las columnas no estan dentro de los limites
        std::cout << "La fila o columna especificada esta fuera de los limites del tablero." << std::endl;
    }
}
//######### Metodo con medicion de tiempo #########
void Tablero::imprimirTablero() {
    Nodo* filaInicio = inicio;
    GestorArchivos guardarPartida = GestorArchivos("PartidaGuardada.txt");

    guardarPartida.abrir();
    if (guardarPartida.leer() != "")
        guardarPartida.limpiar();

    guardarPartida.escribir(nivel);

    //Marca el tiempo inicial
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < filas; i++) {
        actual = filaInicio;
        
        for (int j = 0; j < columnas; j++) {
            std::cout << actual->getSimbolo() << " ";
            guardarPartida.escribir(actual->getSimbolo());
            actual = actual->getDerecha();
        }
        std::cout << std::endl;
        guardarPartida.escribir('\n');
        filaInicio = filaInicio->getAbajo();
    }
    //Marca el tiempo final
    auto endTime = std::chrono::high_resolution_clock::now();
    //Calcula e imprime la duración
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    long long tiempoDurado = duracion.count();
    std::cout << "Tiempo durado en imprimir nivel: " << duracion.count() << " millisegundos" << std::endl;

    guardarPartida.cerrar();
}
void Tablero::imprimirTableroParalelizado() {
    Nodo* nodoInicial = inicio;
    std::vector<std::list<char>> results(filas * columnas);
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::string direccionProcesar;

    // Esto lo estoy usando para indicar la dirección que se va a procesar si de arriba->abajo o izquierda->derecha (LISTO!!)
    if (filas > columnas) {
        direccionProcesar = "abajo";
    }
    else {
        direccionProcesar = "derecha";
    }

    // Marca el tiempo inicial
    auto startTime = std::chrono::high_resolution_clock::now();

    // Map: Se lanzan los Threads
    // TODO: falta averiguar como le pasamos el nodo que necesitamos pasarle a cada thread ya que no 
    //       estoy seguro como funciona este metodo de threads.emplace_back porque creo que itera sobre algo por si mismo.
    threads.emplace_back([&, i]() {
        traverseRow(matrix[i], results[i], mtx);
        });

    // Join threads
    for (auto& t : threads) {
        t.join();
    }
    // Reduce: Imprimir resultados
    for (size_t i = 0; i < results.size(); ++i) {
        for (char simbolo : results[i]) {
            std::cout << simbolo << " ";
        }
        std::cout << "\n";
    }

    //Marca el tiempo final
    auto endTime = std::chrono::high_resolution_clock::now();
    //Calcula e imprime la duración
    auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    long long tiempoDurado = duracion.count();
    std::cout << "Tiempo durado en imprimir nivel: " << duracion.count() << " millisegundos" << std::endl;
}

void Tablero::procesarLinea(Nodo*& inicial, std::string direccion, std::list<char>& resultado, std::mutex& mtx) {
    std::list<char> local;
    Nodo* actual = inicial;

    if (direccion != "derecha" && direccion != "abajo") {
        std::cout << "Direccion no procesable";
    }

    while (actual) {
        local.push_back(actual->getSimbolo());
        if (direccion == "derecha") {
            actual = actual->getDerecha();
        }
        else if (direccion == "abajo") {
            actual = actual->getAbajo();
        }
    }

    std::lock_guard<std::mutex> lock(mtx);
    resultado.splice(resultado.end(), local);
}


void Tablero::movimiento(int& siguienteColumna, int& siguienteFila, char tecla) {
    if (tecla == 'a' || tecla == 'A') {
        siguienteColumna--;
    }
    else if (tecla == 'd' || tecla == 'D') {
        siguienteColumna++;
    }
    else if (tecla == 'w' || tecla == 'W') {
        siguienteFila--;
    }
    else if (tecla == 's' || tecla == 'S') {
        siguienteFila++;
    }
}

Nodo* Tablero::encontrarNodo(int fila, int columna) {
    Nodo* nodo = inicio;
    for (int i = 0; i < fila; i++) {
        nodo = nodo->getAbajo();
    }
    for (int j = 0; j < columna; j++) {
        nodo = nodo->getDerecha();
    }
    return nodo;
}

void Tablero::cambiarSimbolos(Nodo*& nodoActual, Nodo*& nodoMovimiento, Nodo*& nodoSiguiente,
    char simboloActual, char simboloMovimiento, char simboloSiguiente, int fila, int columna, int& fJug, int& cJug) {
    nodoActual->setSimbolo(simboloActual);
    nodoMovimiento->setSimbolo(simboloMovimiento);
    nodoSiguiente->setSimbolo(simboloSiguiente);
    fJug = fila;
    cJug = columna;
}

void Tablero::guardarNivel(std::string nombreArchivo) {
    int fila = 0;
    int columna = 0;
    cuentaPuntos = 0;

    nivelJuego = GestorArchivos(nombreArchivo);
    nivelJuego.abrir();
    std::string contenido = nivelJuego.leer();

    for (char caracter : contenido) {
        if (caracter == '\n') {
            fila++;
        }
    }

    for (char caracter : contenido) {
        if (caracter == '\n') {
            break;
        }
        columna++;
    }

    inicializarTablero(fila, columna);

    fila = 0;
    columna = 0;
    for (char caracter : contenido) {
        if (caracter == '\n') {
            fila++;
            columna = 0;
        }
        else {
            if (caracter == '@') {
                filaJugador = fila;
                columnaJugador = columna;
            }
            else if (caracter == '.') {
                cuentaPuntos++;
            }
            insertarSimbolo(fila, columna, caracter);
            columna++;
        }
    }

    nivelJuego.cerrar();
}

void Tablero::jugar(char tecla) {
    bool esPunto = false;

    //Nueva posicion del jugador
    int nuevaFila = filaJugador;
    int nuevaColumna = columnaJugador;

    movimiento(nuevaColumna, nuevaFila, tecla);
    Nodo* nodo = encontrarNodo(nuevaFila, nuevaColumna); // El nodo al que me voy a mover

    //Verificacion adicional en caso de que el programa falle y el jugador trate de salirse de los limites
    if (nuevaFila >= 0 && nuevaFila < filas && nuevaColumna >= 0 && columnaJugador < columnas) {
        if (nodo->getSimbolo() != '#') { //Es el nodo al que me voy a mover distinto de una pared
            Nodo* nodoActual = encontrarNodo(filaJugador, columnaJugador); // El nodo actual donde me encuentro antes del movimiento

            // Verificar si la nueva posicion esta ocupada por una caja $
            if (nodo->getSimbolo() == '$' || nodo->getSimbolo() == '!') {
                // Calculo de la posicion siguiente en la direccion del movimiento
                int siguienteFila = nuevaFila;
                int siguienteColumna = nuevaColumna;

                movimiento(siguienteColumna, siguienteFila, tecla);

                // Verificar si la posicion siguiente del que me voy a mover esta vacia
                Nodo* siguienteNodo = encontrarNodo(siguienteFila, siguienteColumna); // El nodo siguiente al que me estoy moviendo (el siguiente de una caja por ejemplo)

                if (nodoActual->getSimbolo() == '%') { //If que hace que, si la posicion actual del jugador es '%', cambia el simbolo a '.'
                    if (nodo->getSimbolo() == '!') { //Si el nodo al que me voy a mover es una caja en pos. final
                        if (siguienteNodo->getSimbolo() == '#') {
                            // No hace nada, nada mas bloquea que no pase eso
                        }
                        else if (siguienteNodo->getSimbolo() == '!') { // Si a donde me voy a mover es una caja en pos final y el siguiente tambien es una caja en pos final
                            // No haga nada, no se mueva
                        }
                        else {
                            if (siguienteNodo->getSimbolo() == ' ') {
                                cambiarSimbolos(nodoActual, nodo, siguienteNodo, '.', '%', '$', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                                if (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop();
                            }
                            else {
                                cambiarSimbolos(nodoActual, nodo, siguienteNodo, '.', '%', '!', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                            }
                        }
                    }
                    else if (nodo->getSimbolo() == '$' && siguienteNodo->getSimbolo() == '.') {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, '.', '@', '!', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                        contadorCajasEnPos.push(1);
                    }
                    else {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, '.', '@', '$', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                    }
                }
                else if (siguienteNodo->getSimbolo() == ' ') {
                    // Mover al jugador y la caja si la posicion siguiente de la caja es vacia
                    if (nodo->getSimbolo() == '!') {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, ' ', '%', '$', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                        if (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop();
                    }
                    else {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, ' ', '@', '$', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                    }
                }
                else if (siguienteNodo->getSimbolo() == '.') {
                    if (nodo->getSimbolo() == '!') {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, ' ', '%', '!', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                        if (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop();
                    }
                    else {
                        cambiarSimbolos(nodoActual, nodo, siguienteNodo, ' ', '@', '!', nuevaFila, nuevaColumna, filaJugador, columnaJugador);
                    }
                    contadorCajasEnPos.push(1);
                }
            }
            else {
                // Si la nueva posicion no esta ocupada por una caja, se mueve al jugador

                if (nodo->getSimbolo() == '.') {
                    esPunto = true; //el nodo al que me voy a mover es un punto (o sea, pos. final para almacenar una caja)
                }
                else {
                    esPunto = false;
                }
                if (nodoActual->getSimbolo() == '%') {
                    if (nodo->getSimbolo() == '.') {
                        esPunto = true;
                        nodoActual->setSimbolo('.');
                        nodo->setSimbolo('%');
                    }
                    else {
                        nodoActual->setSimbolo('.');
                        nodo->setSimbolo('@');
                        esPunto = false;
                    }
                }
                else if (esPunto) {
                    // Si la posicion actual era '@', pero la nueva posicion es un punto
                    nodoActual->setSimbolo(' ');
                    nodo->setSimbolo('%');
                }
                else if (!esPunto && nodoActual->getSimbolo() != '%') {
                    nodoActual->setSimbolo(' ');
                    nodo->setSimbolo('@');
                }

                filaJugador = nuevaFila;
                columnaJugador = nuevaColumna;

                // Verifica si el nodo anterior tenia un '.' y lo restaura
                Nodo* nodoAnterior = encontrarNodo(filaJugador, columnaJugador);
                if (nodoAnterior->getSimbolo() == '.') {
                    nodoAnterior->setSimbolo('%'); // Si el nodo anterior tenia '.', restaura a '%'
                }
            }
        }
    }
}

void Tablero::moverJugador(bool jugarPartidaGuardada) {
    nivel = '1';
    char tecla;
    int nivelPartidaGuardada = 1; //se empieza en el nivel 1
    bool revisarPartidaGuardada = false;
    GestorArchivos partidaGuardada = GestorArchivos("PartidaGuardada.txt");
    partidaGuardada.abrir();
    nivelPartidaGuardada = partidaGuardada.leerPrimerCaracter() - '0';

    if (partidaGuardada.leerGuardada() != " " && jugarPartidaGuardada) {
        revisarPartidaGuardada = true;
    }
    else {
        nivelPartidaGuardada = 1;
    }
    partidaGuardada.cerrar();

    for (int i = nivelPartidaGuardada; i <= 4; i++) {
        movimientosRealizados.clear();
        while (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop();
        std::string nombreArchivo = "Nivel" + std::to_string(i) + ".txt";
        if (revisarPartidaGuardada) {
            nivel++;
            guardarNivel("PartidaGuardada.txt");
            
        }
        else {
            guardarNivel(nombreArchivo); //si no hay partida guardada
        }

        do {
            system("cls");
            imprimirTablero();

            if (contadorCajasEnPos.size() == cuentaPuntos) { //si la cantidad de puntos del nivel es igual a la cantidad de cajas en pos. final
                std::cout << "Felicidades, ha ganado!" << "\n";
                nivelJuego.copiarArchivo("Nivel" + std::to_string(i + 1) + ".txt", "PartidaGuardada.txt");
                break;
            }

            std::cout << "Cantidad de Cajas en Posicion Final: " << contadorCajasEnPos.size() << " de " << cuentaPuntos << std::endl;
            std::cout << "Use las teclas W, A, S, D para mover al jugador @" << std::endl;
            std::cout << "Presione Z para reiniciar el nivel." << std::endl;
            std::cout << "Presione cualquier letra distinta de W, A, S, D para salir del nivel." << std::endl;

            tecla = _getch();

            if (tecla == 'Z' || tecla == 'z') {
                guardarNivel(nombreArchivo);
                while (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop();
            }

            if (tecla != 'W' && tecla != 'A' && tecla != 'S' && tecla != 'D' && tecla != 'Z' && tecla != 'w' && tecla != 'a' && tecla != 's' && tecla != 'd' && tecla != 'z') {
                std::cout << "\nSaliendo del nivel...\n"; Sleep(500);
                return;
            }

            if (tecla == 'W' || tecla == 'A' || tecla == 'S' || tecla == 'D' || tecla == 'w' || tecla == 'a' || tecla == 's' || tecla == 'd')
                movimientosRealizados.push_back(tecla);

            jugar(tecla);

        } while (true);

        while (!contadorCajasEnPos.empty()) contadorCajasEnPos.pop(); //El contador se resetea cada vez que termina un nivel

        char respuesta = ' ';

        if (!revisarPartidaGuardada) {
            std::cout << "Desea revisar la repeticion (S/N): ";
            std::cin >> respuesta;
            if (respuesta == 'S' || respuesta == 's') {
                guardarNivel(nombreArchivo);

                system("cls");
                imprimirTablero();

                for (char movimiento : movimientosRealizados) {
                    jugar(static_cast<char>(toupper(movimiento)));

                    Sleep(200);

                    system("cls");
                    imprimirTablero();
                }

                std::cout << "Movimientos Realizados:\n";
                for (char movimiento : movimientosRealizados) {
                    std::cout << static_cast<char>(toupper(movimiento)) << " "; //para mostrar los movimientos realizados por el usuario, un detalle visual
                }
                std::cout << "\n\n";

                system("pause");
            }
        }
        revisarPartidaGuardada = false;

        if (i == 4) { // Si se agrega o se quita un nivel, el numero debe cambiarse
            std::cout << "\nHa completado todos los niveles, felicidades!\n";
            Sleep(300);
            std::cout << "Volviendo al menu principal\n\n";
            system("pause");
        }
    }
}