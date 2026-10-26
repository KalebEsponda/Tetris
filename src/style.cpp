#include "style.h"
static const Paleta PALETAS[] = {
    {   // 0: Arcade
        "Arcade",
        {10, 10, 30, 255}, {4, 4, 14, 255}, {190, 190, 220, 255},
        {28, 28, 56, 255}, {255, 255, 255, 255},
        { {0, 220, 255, 255}, {255, 220, 0, 255}, {180, 70, 255, 255},
          {60, 230, 90, 255}, {255, 60, 70, 255}, {60, 110, 255, 255},
          {255, 150, 30, 255} }
    },
    {   // 1: Atardecer
        "Atardecer",
        {38, 10, 28, 255}, {20, 4, 14, 255}, {255, 170, 70, 255},
        {70, 24, 48, 255}, {255, 235, 200, 255},
        { {255, 90, 90, 255}, {255, 200, 60, 255}, {255, 130, 200, 255},
          {255, 150, 50, 255}, {200, 60, 120, 255}, {255, 230, 120, 255},
          {230, 110, 70, 255} }
    },
    {   // 2: Terminal
        "Terminal",
        {0, 12, 4, 255}, {0, 5, 2, 255}, {70, 255, 120, 255},
        {0, 50, 20, 255}, {170, 255, 190, 255},
        { {70, 255, 120, 255}, {190, 255, 90, 255}, {40, 200, 160, 255},
          {120, 255, 200, 255}, {30, 170, 90, 255}, {160, 230, 60, 255},
          {90, 220, 140, 255} }
    },
    {   // 3: Oceano
        "Oceano",
        {2, 14, 30, 255}, {0, 8, 20, 255}, {80, 200, 230, 255},
        {8, 40, 64, 255}, {220, 245, 255, 255},
        { {0, 200, 220, 255}, {80, 230, 200, 255}, {60, 140, 255, 255},
          {150, 220, 255, 255}, {0, 120, 200, 255}, {120, 255, 230, 255},
          {40, 180, 160, 255} }
    },
    {   // 4: Neon
        "Neon",
        {20, 0, 32, 255}, {10, 0, 18, 255}, {255, 70, 210, 255},
        {60, 10, 70, 255}, {255, 240, 255, 255},
        { {255, 60, 200, 255}, {0, 240, 255, 255}, {255, 240, 60, 255},
          {160, 80, 255, 255}, {255, 110, 60, 255}, {60, 255, 150, 255},
          {80, 140, 255, 255} }
    },
    {   // 5: Hielo
        "Hielo",
        {12, 26, 42, 255}, {6, 16, 30, 255}, {190, 225, 250, 255},
        {30, 56, 84, 255}, {245, 252, 255, 255},
        { {200, 235, 255, 255}, {140, 200, 255, 255}, {100, 160, 240, 255},
          {230, 245, 255, 255}, {120, 220, 235, 255}, {170, 170, 255, 255},
          {90, 200, 255, 255} }
    },
    {   // 6: Lava
        "Lava",
        {28, 4, 4, 255}, {16, 2, 2, 255}, {255, 120, 40, 255},
        {64, 16, 10, 255}, {255, 230, 200, 255},
        { {255, 70, 40, 255}, {255, 150, 30, 255}, {255, 210, 60, 255},
          {200, 30, 30, 255}, {255, 100, 100, 255}, {255, 180, 90, 255},
          {230, 90, 20, 255} }
    },
    {   // 7: Oro
        "Oro",
        {10, 8, 2, 255}, {5, 4, 0, 255}, {255, 200, 60, 255},
        {48, 40, 10, 255}, {255, 245, 210, 255},
        { {255, 215, 0, 255}, {240, 170, 40, 255}, {255, 240, 150, 255},
          {200, 140, 30, 255}, {255, 190, 70, 255}, {230, 200, 100, 255},
          {210, 160, 60, 255} }
    }
};

static const int TOTAL_PALETAS = sizeof(PALETAS) / sizeof(PALETAS[0]);
const Paleta& obtenerPaleta(int indice) { return PALETAS[indice % TOTAL_PALETAS]; }
int totalPaletas() { return TOTAL_PALETAS; }
Color colorBloque(const Paleta& pal, int tipo) { return pal.bloques[(tipo - 1) % 7]; }
void dibujarBloque(int x, int y, int tam, Color color) {
    int b = tam / 8; if (b < 2) b = 2;
    DrawRectangle(x, y, tam, tam, ColorBrightness(color, -0.45f));
    DrawRectangle(x, y, tam - b, tam - b, ColorBrightness(color, 0.45f));
    DrawRectangle(x + b, y + b, tam - 2 * b, tam - 2 * b, color);
    DrawRectangle(x + b + 1, y + b + 1, b, b, Fade(WHITE, 0.6f));
}
void dibujarMarcoTablero(const Paleta& pal, int x, int y, int ancho, int alto) {
    DrawRectangle(x - 8, y - 8, ancho + 16, alto + 16, pal.marco);
    DrawRectangle(x - 4, y - 4, ancho + 8, alto + 8, ColorBrightness(pal.marco, -0.6f));
    DrawRectangle(x, y, ancho, alto, pal.fondoTablero);
}
void dibujarPanel(const Font& fuente, const Paleta& pal, int x, int y, int ancho, int alto, const char* titulo, const char* valor) {
    DrawRectangle(x, y, ancho, alto, pal.marco);
    DrawRectangle(x + 4, y + 4, ancho - 8, alto - 8, pal.fondoTablero);
    DrawTextEx(fuente, titulo, Vector2{ (float)(x + 12), (float)(y + 12) }, 16, 2, pal.marco);
    DrawTextEx(fuente, valor, Vector2{ (float)(x + 12), (float)(y + alto - 30) }, 16, 2, pal.texto);
}
void dibujarTextoCentrado(const Font& fuente, const char* texto, int centroX, int y, float tam, Color color) {
    float espacio = tam / 8.0f;
    Vector2 medida = MeasureTextEx(fuente, texto, tam, espacio);
    DrawTextEx(fuente, texto, Vector2{ centroX - medida.x / 2.0f, (float)y }, tam, espacio, color);
}