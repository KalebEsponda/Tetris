#include "piece.h"
#include "board.h"

// 1 = hay cuadro, 0 = vacío
static const int FORMAS[TOTAL_TIPOS][4][4] = {
    // Clásicas
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},  // 1: I
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},  // 2: O
    {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},  // 3: T
    {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},  // 4: S
    {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},  // 5: Z
    {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},  // 6: J
    {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}},  // 7: L
    // Pequeñas (modo fácil)
    {{0,0,0,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}},  // 8: punto
    {{0,0,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},  // 9: dominó
    {{0,0,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},  // 10: L pequeña
    // Grandes (modo difícil)
    {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,1,0,0}},  // 11: cruz
    {{0,0,0,0},{1,0,1,0},{1,1,1,0},{0,0,0,0}},  // 12: U
    {{0,0,0,0},{1,1,0,0},{1,1,0,0},{1,0,0,0}}   // 13: P
};

void nuevaPieza(Pieza& p, const ConfigJuego& cfg) {
    int indice = GetRandomValue(0, cfg.cantidadTipos - 1);
    p.tipo = cfg.tiposPermitidos[indice];
    p.rotacion = 0;
    p.fila = 0;
    p.col = 3;
}

void obtenerForma(int tipo, int rotacion, int forma[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            forma[i][j] = FORMAS[tipo - 1][i][j];

    for (int k = 0; k < rotacion; k++) {
        int temp[4][4];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                temp[i][j] = forma[3 - j][i];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                forma[i][j] = temp[i][j];
    }
}

void dibujarPieza(const Pieza& p, int origenX, int origenY, const Paleta& pal) {
    int forma[4][4];
    obtenerForma(p.tipo, p.rotacion, forma);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (forma[i][j] == 1) {
                int x = origenX + (p.col + j) * TAM_CELDA;
                int y = origenY + (p.fila + i) * TAM_CELDA;
                dibujarBloque(x, y, TAM_CELDA, colorBloque(pal, p.tipo));
            }
        }
    }
}

// Sombra de la pieza en el lugar donde va a aterrizar
void dibujarFantasma(const Pieza& p, int filaFinal, int origenX, int origenY, const Paleta& pal) {
    int forma[4][4];
    obtenerForma(p.tipo, p.rotacion, forma);
    Color color = colorBloque(pal, p.tipo);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (forma[i][j] == 1) {
                int x = origenX + (p.col + j) * TAM_CELDA;
                int y = origenY + (filaFinal + i) * TAM_CELDA;
                DrawRectangle(x + 2, y + 2, TAM_CELDA - 4, TAM_CELDA - 4, Fade(color, 0.18f));
                DrawRectangleLines(x + 2, y + 2, TAM_CELDA - 4, TAM_CELDA - 4, Fade(color, 0.7f));
            }
        }
    }
}