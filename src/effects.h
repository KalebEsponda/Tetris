#pragma once
#include "raylib.h"

void emitirParticula(float x, float y, Color color);
void actualizarParticulas(float dt);
void dibujarParticulas();
void limpiarParticulas();
Vector2 desplazamientoTemblor(float magnitud);