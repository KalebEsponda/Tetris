#pragma once
#include "skills.h"

const int TOTAL_NIVELES = 8;

// Datos de cada nivel: se leen de esta tabla, así que para cambiar la dificultad de un
// nivel solo hay que editar números aquí, sin tocar la lógica del juego
struct Nivel {
    int lineasParaSubir;      // líneas que hay que borrar para pasar al siguiente (0 = último nivel)
    float factorVelocidad;    // se multiplica por la caída de la dificultad (menor = más rápido)
    int paleta;               // paleta de colores de este nivel
    int habilidades;          // habilidades que se pueden usar en este nivel (se combinan con |)
};

// nivel va de 1 a TOTAL_NIVELES
inline const Nivel& datosNivel(int nivel) {
    static const Nivel NIVELES[TOTAL_NIVELES] = {
        {  5, 1.00f, 0, 0 },                                            // 1: sin habilidades
        {  6, 0.90f, 1, HAB_CAMBIAR },                                  // 2
        {  7, 0.80f, 2, HAB_CAMBIAR },                                  // 3
        {  8, 0.72f, 3, HAB_CAMBIAR | HAB_CONGELAR },                   // 4
        {  9, 0.64f, 4, HAB_CAMBIAR | HAB_CONGELAR },                   // 5
        { 10, 0.57f, 5, HAB_TODAS },                                    // 6
        { 12, 0.50f, 6, HAB_TODAS },                                    // 7
        {  0, 0.45f, 7, HAB_TODAS }                                     // 8 (último)
    };
    int i = nivel - 1;
    if (i < 0) i = 0;
    if (i > TOTAL_NIVELES - 1) i = TOTAL_NIVELES - 1;
    return NIVELES[i];
}