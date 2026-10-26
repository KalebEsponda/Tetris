#pragma once
#include "raylib.h"

struct Paleta {
    const char* nombre;
    Color fondo;         // fondo de la ventana
    Color fondoTablero;  // fondo del área de juego
    Color marco;         // marcos y títulos
    Color cuadricula;    // líneas del tablero vacío
    Color texto;         // números y texto principal
    Color bloques[7];    // colores de las piezas
};

const Paleta& obtenerPaleta(int indice);
int totalPaletas();
Color colorBloque(const Paleta& pal, int tipo);

void dibujarBloque(int x, int y, int tam, Color color);
void dibujarMarcoTablero(const Paleta& pal, int x, int y, int ancho, int alto);
void dibujarPanel(const Font& fuente, const Paleta& pal, int x, int y,
    int ancho, int alto, const char* titulo, const char* valor);
void dibujarTextoCentrado(const Font& fuente, const char* texto, int centroX,
    int y, float tam, Color color);