#include "effects.h"

struct Particula {
    float x, y;
    float vx, vy;
    float vida;
    float vidaMax;
    Color color;
    bool activa;
};

static const int MAX_PARTICULAS = 300;
static Particula particulas[MAX_PARTICULAS] = {};

static float aleatorio(float minimo, float maximo) {
    return minimo + (maximo - minimo) * (GetRandomValue(0, 1000) / 1000.0f);
}

void emitirParticula(float x, float y, Color color) {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (!particulas[i].activa) {
            particulas[i].x = x;
            particulas[i].y = y;
            particulas[i].vx = aleatorio(-140.0f, 140.0f);
            particulas[i].vy = aleatorio(-260.0f, -40.0f);
            particulas[i].vidaMax = aleatorio(0.5f, 1.0f);
            particulas[i].vida = particulas[i].vidaMax;
            particulas[i].color = color;
            particulas[i].activa = true;
            return;
        }
    }
}

void actualizarParticulas(float dt) {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (particulas[i].activa) {
            particulas[i].vy += 600.0f * dt;     // gravedad
            particulas[i].x += particulas[i].vx * dt;
            particulas[i].y += particulas[i].vy * dt;
            particulas[i].vida -= dt;
            if (particulas[i].vida <= 0.0f) {
                particulas[i].activa = false;
            }
        }
    }
}

void dibujarParticulas() {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (particulas[i].activa) {
            float t = particulas[i].vida / particulas[i].vidaMax;
            int tam = (t > 0.5f) ? 6 : 4;        // se encogen al morir
            DrawRectangle((int)particulas[i].x, (int)particulas[i].y, tam, tam,
                Fade(particulas[i].color, t));
        }
    }
}

void limpiarParticulas() {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        particulas[i].activa = false;
    }
}

Vector2 desplazamientoTemblor(float magnitud) {
    if (magnitud <= 0.0f) return Vector2{ 0.0f, 0.0f };
    return Vector2{ aleatorio(-magnitud, magnitud), aleatorio(-magnitud, magnitud) };
}