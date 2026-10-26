#pragma once
#include "raylib.h"
#include "config.h"
#include "style.h"

const int TOTAL_TIPOS = 13;

struct Pieza {
    int tipo;      // 1 a 13
    int rotacion;  // 0 a 3
    int fila;      // posición del cuadro 4x4 en el tablero
    int col;
};

void nuevaPieza(Pieza& p, const ConfigJuego& cfg);
void obtenerForma(int tipo, int rotacion, int forma[4][4]);
void dibujarPieza(const Pieza& p, int origenX, int origenY, const Paleta& pal);
void dibujarFantasma(const Pieza& p, int filaFinal, int origenX, int origenY, const Paleta& pal);