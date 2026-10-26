#include "board.h"

void iniciarTablero(Tablero& t) {
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            t.celdas[f][c] = 0;
        }
    }
}

void dibujarTablero(const Tablero& t, int origenX, int origenY, const Paleta& pal,
    const bool* filasResaltadas, bool brillo) {
    dibujarMarcoTablero(pal, origenX, origenY, COLUMNAS * TAM_CELDA, FILAS * TAM_CELDA);

    for (int f = 0; f < FILAS; f++) {
        bool resaltada = (filasResaltadas != nullptr && filasResaltadas[f] && brillo);
        for (int c = 0; c < COLUMNAS; c++) {
            int x = origenX + c * TAM_CELDA;
            int y = origenY + f * TAM_CELDA;

            if (t.celdas[f][c] != 0) {
                Color color = resaltada ? WHITE : colorBloque(pal, t.celdas[f][c]);
                dibujarBloque(x, y, TAM_CELDA, color);
            }
            else {
                DrawRectangleLines(x, y, TAM_CELDA, TAM_CELDA, pal.cuadricula);
            }
        }
    }
}

bool hayColision(const Tablero& t, const Pieza& p) {
    int forma[4][4];
    obtenerForma(p.tipo, p.rotacion, forma);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (forma[i][j] == 1) {
                int f = p.fila + i;
                int c = p.col + j;

                if (c < 0 || c >= COLUMNAS || f >= FILAS) {
                    return true;  // se sale del tablero
                }
                if (f >= 0 && t.celdas[f][c] != 0) {
                    return true;  // choca con un bloque
                }
            }
        }
    }
    return false;
}

void fijarPieza(Tablero& t, const Pieza& p) {
    int forma[4][4];
    obtenerForma(p.tipo, p.rotacion, forma);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (forma[i][j] == 1) {
                int f = p.fila + i;
                int c = p.col + j;
                if (f >= 0 && f < FILAS && c >= 0 && c < COLUMNAS) {
                    t.celdas[f][c] = p.tipo;
                }
            }
        }
    }
}

int borrarLineas(Tablero& t) {
    int borradas = 0;
    int f = FILAS - 1;   // se revisa de abajo hacia arriba

    while (f >= 0) {
        bool completa = true;
        for (int c = 0; c < COLUMNAS; c++) {
            if (t.celdas[f][c] == 0) {
                completa = false;
            }
        }

        if (completa) {
            borradas++;
            // Bajar todas las filas de arriba una posición
            for (int k = f; k > 0; k--) {
                for (int c = 0; c < COLUMNAS; c++) {
                    t.celdas[k][c] = t.celdas[k - 1][c];
                }
            }
            // La fila de hasta arriba queda vacía
            for (int c = 0; c < COLUMNAS; c++) {
                t.celdas[0][c] = 0;
            }
            // No se baja "f": hay que revisar esa misma fila otra vez
        }
        else {
            f--;
        }
    }
    return borradas;
}

// Marca en "filas" cuáles están completas (sin borrarlas) y devuelve cuántas son
int detectarLineas(const Tablero& t, bool filas[FILAS]) {
    int cantidad = 0;
    for (int f = 0; f < FILAS; f++) {
        bool completa = true;
        for (int c = 0; c < COLUMNAS; c++) {
            if (t.celdas[f][c] == 0) completa = false;
        }
        filas[f] = completa;
        if (completa) cantidad++;
    }
    return cantidad;
}

// Fila donde terminaría la pieza si cayera hasta chocar
int filaFantasma(const Tablero& t, Pieza p) {
    while (!hayColision(t, p)) {
        p.fila += 1;
    }
    return p.fila - 1;
}

// Cuántos bloques hay en las "cantidad" filas de más abajo
int contarBloquesInferiores(const Tablero& t, int cantidad) {
    int total = 0;
    for (int f = FILAS - cantidad; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            if (t.celdas[f][c] != 0) total++;
        }
    }
    return total;
}

// Borra las "cantidad" filas de más abajo y baja todo lo demás
void borrarFilasInferiores(Tablero& t, int cantidad) {
    for (int f = FILAS - 1; f >= cantidad; f--) {
        for (int c = 0; c < COLUMNAS; c++) {
            t.celdas[f][c] = t.celdas[f - cantidad][c];
        }
    }
    for (int f = 0; f < cantidad; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            t.celdas[f][c] = 0;
        }
    }
}