#pragma once
#include "raylib.h"
#include "piece.h"
#include "style.h"

const int COLUMNAS = 10;
const int FILAS = 20;
const int TAM_CELDA = 30;

struct Tablero {
    int celdas[FILAS][COLUMNAS];
};

void iniciarTablero(Tablero& t);
void dibujarTablero(const Tablero& t, int origenX, int origenY, const Paleta& pal,
    const bool* filasResaltadas = nullptr, bool brillo = false);
bool hayColision(const Tablero& t, const Pieza& p);
void fijarPieza(Tablero& t, const Pieza& p);
int borrarLineas(Tablero& t);
int detectarLineas(const Tablero& t, bool filas[FILAS]);
int filaFantasma(const Tablero& t, Pieza p);
int contarBloquesInferiores(const Tablero& t, int cantidad);
void borrarFilasInferiores(Tablero& t, int cantidad);