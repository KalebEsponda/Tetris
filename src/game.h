#pragma once
#include "raylib.h"
#include "board.h"
#include "piece.h"
#include "config.h"
#include "levels.h"
#include "skills.h"
#include "style.h"
#include "audio.h"

const int ANCHO_VENTANA = 600;
const int ALTO_VENTANA = 700;
const int ORIGEN_X = 60;     // esquina del tablero en pantalla
const int ORIGEN_Y = 50;

// Todo lo que cambia durante una partida, junto en un solo paquete
struct Partida {
    Tablero tablero;
    Pieza pieza;
    int dificultad;               // 0 = fácil, 1 = normal, 2 = difícil
    ConfigJuego cfg;              // velocidad, piezas y música de esa dificultad
    int puntaje;
    int lineas;                   // líneas borradas en total
    int nivel;                    // nivel actual (1 a TOTAL_NIVELES)
    int lineasNivel;              // líneas borradas dentro del nivel actual
    float tiempoBanner;           // segundos que le quedan al aviso de "NIVEL X"
    int energia;                  // energía para las habilidades (0 a ENERGIA_MAXIMA)
    float tiempoCongelado;        // segundos que le quedan a CONGELAR (0 = no activo)
    const char* mensaje;          // aviso corto sobre el tablero ("CAMBIAR", "SIN ENERGIA"...)
    bool mensajeError;            // true si el aviso es de error
    float tiempoMensaje;          // segundos que le quedan al aviso
    float temporizador;
    bool juegoTerminado;
    bool animando;                // true mientras parpadean las líneas
    float tiempoAnimacion;
    bool filasLlenas[FILAS];      // qué filas están parpadeando
};

void reiniciar(Partida& p, int dificultad, Sonidos& sonidos);
void subirNivel(Partida& p, Sonidos& sonidos);
void actualizarJuego(Partida& p, const Paleta& pal, Sonidos& sonidos, float& temblor, float dt);
void dibujarJuego(const Partida& p, const Font& fuente, const Paleta& pal, float temblor);
void dibujarAyudaPausa(const Partida& p, const Font& fuente, const Paleta& pal);
void reanudar(Partida& p, Sonidos& sonidos);