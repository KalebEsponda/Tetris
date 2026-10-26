#pragma once
#include "raylib.h"

const int TOTAL_PISTAS = 3;   // 0 = fácil, 1 = normal, 2 = difícil

struct Sonidos {
    Sound mover;
    Sound rotar;
    Sound caer;
    Sound linea;
    Sound tetris;
    Sound gameover;
    Sound nivel;
    Sound habilidad;
    Sound error;
    Music musicas[TOTAL_PISTAS];
    int pistaActual;
    bool silencio;
};

void cargarSonidos(Sonidos& s);
void liberarSonidos(Sonidos& s);
void reproducir(const Sound& sonido);
void actualizarMusica(Sonidos& s);
void iniciarMusica(Sonidos& s, int pista);
void detenerMusica(Sonidos& s);
void pausarMusica(Sonidos& s);
void reanudarMusica(Sonidos& s);
void alternarSilencio(Sonidos& s);