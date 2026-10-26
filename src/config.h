#pragma once
#include <initializer_list>

const int MAX_TIPOS_PERMITIDOS = 16;
const int TOTAL_DIFICULTADES = 3;   // 0 = fácil, 1 = normal, 2 = difícil

struct ConfigJuego {
    float intervalo;                            // segundos que tarda en bajar una casilla
    int pistaMusica;                            // 0 = fácil, 1 = normal, 2 = difícil
    int tiposPermitidos[MAX_TIPOS_PERMITIDOS];  // qué piezas pueden salir
    int cantidadTipos;
};

inline ConfigJuego crearConfig(float intervalo, int pistaMusica, std::initializer_list<int> tipos) {
    ConfigJuego c = {};
    c.intervalo = intervalo;
    c.pistaMusica = pistaMusica;
    for (int t : tipos) {
        if (c.cantidadTipos < MAX_TIPOS_PERMITIDOS) {
            c.tiposPermitidos[c.cantidadTipos] = t;
            c.cantidadTipos++;
        }
    }
    return c;
}

// Fácil: lento, música tranquila y piezas pequeñas
inline ConfigJuego configFacil() {
    return crearConfig(1.0f, 0, { 1, 2, 3, 6, 7, 8, 9, 10 });
}

// Normal: las 7 piezas clásicas
inline ConfigJuego configNormal() {
    return crearConfig(0.7f, 1, { 1, 2, 3, 4, 5, 6, 7 });
}

// Difícil: rápido, música acelerada y piezas grandes de 5 cuadros
inline ConfigJuego configDificil() {
    return crearConfig(0.4f, 2, { 1, 2, 3, 4, 5, 6, 7, 11, 12, 13 });
}

// Devuelve la configuración según el número de dificultad (0, 1 o 2)
inline ConfigJuego configPorIndice(int indice) {
    if (indice == 0) return configFacil();
    if (indice == 2) return configDificil();
    return configNormal();
}

inline const char* nombreDificultad(int indice) {
    if (indice == 0) return "FACIL";
    if (indice == 2) return "DIFICIL";
    return "NORMAL";
}