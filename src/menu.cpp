#include "menu.h"
#include "game.h"
#include <cmath>

// ---------------------------------------------------------------- fondo
// Piezas translúcidas que caen despacio detrás del menú
struct Decor {
    float x, y, velocidad;
    int tipo, rotacion;
};

static const int TOTAL_DECOR = 12;
static Decor decor[TOTAL_DECOR] = {};

static void reubicar(Decor& d, bool desdeArriba) {
    d.x = (float)GetRandomValue(0, ANCHO_VENTANA - 100);
    d.y = desdeArriba ? (float)GetRandomValue(-200, -100)
        : (float)GetRandomValue(-100, ALTO_VENTANA);
    d.velocidad = (float)GetRandomValue(30, 90);
    d.tipo = GetRandomValue(1, 7);
    d.rotacion = GetRandomValue(0, 3);
}

void iniciarFondoMenu() {
    for (int i = 0; i < TOTAL_DECOR; i++) reubicar(decor[i], false);
}

void actualizarFondoMenu(float dt) {
    for (int i = 0; i < TOTAL_DECOR; i++) {
        decor[i].y += decor[i].velocidad * dt;
        if (decor[i].y > ALTO_VENTANA) reubicar(decor[i], true);
    }
}

void dibujarFondoMenu(const Paleta& pal) {
    for (int k = 0; k < TOTAL_DECOR; k++) {
        int forma[4][4];
        obtenerForma(decor[k].tipo, decor[k].rotacion, forma);
        Color color = Fade(colorBloque(pal, decor[k].tipo), 0.30f);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (forma[i][j] == 1) {
                    dibujarBloque((int)decor[k].x + j * 24, (int)decor[k].y + i * 24, 24, color);
                }
            }
        }
    }
}

// ---------------------------------------------------------------- pantallas
// Título con cada letra de un color y un pequeño rebote
static void dibujarTitulo(const Font& fuente, const Paleta& pal, int centroX, int y, float tiempo) {
    const char* letras = "TETRIS";
    const float tam = 48.0f;
    const float espacio = 4.0f;
    float paso = tam + espacio;
    float inicio = centroX - (6 * paso - espacio) / 2.0f;

    for (int i = 0; i < 6; i++) {
        float rebote = sinf(tiempo * 3.0f + i * 0.7f) * 5.0f;
        char texto[2] = { letras[i], '\0' };
        Color color = colorBloque(pal, i + 1);
        DrawTextEx(fuente, texto, Vector2{ inicio + i * paso + 4, y + rebote + 4 }, tam, 0,
            ColorBrightness(color, -0.6f));                       // sombra
        DrawTextEx(fuente, texto, Vector2{ inicio + i * paso, y + rebote }, tam, 0, color);
    }
}

// Las 3 ranuras de perfil: nombre, récords y si tienen partida guardada
void dibujarPantallaPerfiles(const Font& fuente, const Paleta& pal, ModoPerfiles modo,
    const Perfil perfiles[TOTAL_PERFILES], const bool ocupada[TOTAL_PERFILES],
    int seleccion, const char* aviso, bool confirmandoBorrado,
    bool puedeVolver, float tiempo) {
    int centro = ANCHO_VENTANA / 2;
    const char* titulo = (modo == PERFILES_GUARDAR) ? "GUARDAR PERFIL"
        : (modo == PERFILES_CARGAR) ? "CARGAR PERFIL"
        : "ELIGE PERFIL";
    dibujarTextoCentrado(fuente, titulo, centro, 60, 32, pal.texto);

    bool parpadeo = ((int)(tiempo * 3.0f) % 2 == 0);
    Color apagado = ColorBrightness(pal.marco, -0.45f);

    for (int i = 0; i < TOTAL_PERFILES; i++) {
        int y = 130 + i * 120;
        bool elegida = (i == seleccion);
        Color borde = elegida ? (parpadeo ? pal.texto : pal.marco) : apagado;
        DrawRectangle(30, y, 540, 104, borde);
        DrawRectangle(34, y + 4, 532, 96, pal.fondoTablero);
        DrawTextEx(fuente, TextFormat("%d", i + 1), Vector2{ 50.0f, (float)(y + 34) }, 32, 0, colorBloque(pal, i + 1));

        if (ocupada[i]) {
            const Perfil& pf = perfiles[i];
            DrawTextEx(fuente, pf.nombre, Vector2{ 110.0f, (float)(y + 14) }, 32, 4, pal.texto);
            DrawTextEx(fuente, TextFormat("F %d  N %d  D %d", pf.mejorPuntaje[0], pf.mejorPuntaje[1], pf.mejorPuntaje[2]),
                Vector2{ 110.0f, (float)(y + 58) }, 16, 2, pal.marco);
            DrawTextEx(fuente, pf.hayPartida ? TextFormat("PARTIDA NIVEL %d", pf.guardada.nivel) : "SIN PARTIDA",
                Vector2{ 110.0f, (float)(y + 78) }, 16, 2, pf.hayPartida ? pal.texto : apagado);
        }
        else {
            DrawTextEx(fuente, "VACIO", Vector2{ 110.0f, (float)(y + 34) }, 32, 4, apagado);
        }
    }

    if (aviso != nullptr) {
        dibujarTextoCentrado(fuente, aviso, centro, 505, 16, RED);
    }

    if (modo == PERFILES_ELEGIR) {
        dibujarTextoCentrado(fuente, "ENTER ELEGIR  X BORRAR", centro, 600, 16, pal.marco);
    }
    else {
        const char* accion = (modo == PERFILES_GUARDAR) ? "ENTER CREAR" : "ENTER CARGAR";
        dibujarTextoCentrado(fuente, TextFormat("%s  ESC VOLVER", accion), centro, 600, 16, pal.marco);
        dibujarTextoCentrado(fuente, "X BORRAR", centro, 630, 16, pal.marco);
    }
    (void)puedeVolver;

    // Confirmación antes de borrar un perfil
    if (confirmandoBorrado && ocupada[seleccion]) {
        DrawRectangle(0, 0, ANCHO_VENTANA, ALTO_VENTANA, Fade(BLACK, 0.7f));
        DrawRectangle(60, 250, 480, 170, pal.marco);
        DrawRectangle(64, 254, 472, 162, pal.fondoTablero);
        dibujarTextoCentrado(fuente, "BORRAR PERFIL?", centro, 275, 32, RED);
        dibujarTextoCentrado(fuente, perfiles[seleccion].nombre, centro, 335, 32, pal.texto);
        dibujarTextoCentrado(fuente, "ENTER SI  ESC NO", centro, 385, 16, pal.marco);
    }
}

// Pantalla para escribir el nombre del perfil nuevo
void dibujarPantallaNombre(const Font& fuente, const Paleta& pal, const char* nombre, const char* aviso, float tiempo) {
    int centro = ANCHO_VENTANA / 2;
    dibujarTextoCentrado(fuente, "NUEVO PERFIL", centro, 120, 32, pal.texto);
    dibujarTextoCentrado(fuente, "ESCRIBE TU NOMBRE", centro, 200, 16, pal.marco);

    DrawRectangle(100, 260, 400, 80, pal.marco);
    DrawRectangle(104, 264, 392, 72, pal.fondoTablero);
    DrawTextEx(fuente, nombre, Vector2{ 120.0f, 286.0f }, 32, 4, pal.texto);

    // Cursor que parpadea justo después de la última letra
    if ((int)(tiempo * 2.0f) % 2 == 0) {
        Vector2 medida = MeasureTextEx(fuente, nombre, 32, 4);
        DrawRectangle((int)(120 + medida.x + 4), 316, 24, 6, pal.texto);
    }

    dibujarTextoCentrado(fuente, "LETRAS Y NUMEROS", centro, 370, 16, pal.marco);
    dibujarTextoCentrado(fuente, "MAXIMO 8", centro, 400, 16, pal.marco);
    if (aviso != nullptr) {
        dibujarTextoCentrado(fuente, aviso, centro, 450, 16, RED);
    }
    dibujarTextoCentrado(fuente, "ENTER ACEPTAR  ESC VOLVER", centro, 630, 16, pal.marco);
}

void dibujarMenuPrincipal(const Font& fuente, const Paleta& pal, const char* jugador,
    const char* const* opciones, int total, int opcion, float tiempo) {
    int centro = ANCHO_VENTANA / 2;
    dibujarTitulo(fuente, pal, centro, 130, tiempo);
    dibujarTextoCentrado(fuente, "EDICION RETRO", centro, 215, 16, pal.marco);
    dibujarTextoCentrado(fuente, TextFormat("JUGADOR %s", jugador), centro, 255, 16, pal.texto);

    bool parpadeo = ((int)(tiempo * 3.0f) % 2 == 0);

    for (int i = 0; i < total; i++) {
        bool elegida = (i == opcion);
        Color color = elegida ? pal.texto : ColorBrightness(pal.marco, -0.35f);
        const char* texto = elegida ? (parpadeo ? TextFormat("> %s <", opciones[i])
            : TextFormat("  %s  ", opciones[i]))
            : opciones[i];
        dibujarTextoCentrado(fuente, texto, centro, 310 + i * 60, 32, color);
    }

    dibujarTextoCentrado(fuente, "ARRIBA ABAJO + ENTER", centro, 630, 16, pal.marco);
}

void dibujarMenuDificultad(const Font& fuente, const Paleta& pal, int opcion, float tiempo, int record) {
    int centro = ANCHO_VENTANA / 2;
    dibujarTextoCentrado(fuente, "DIFICULTAD", centro, 90, 32, pal.texto);

    bool parpadeo = ((int)(tiempo * 3.0f) % 2 == 0);
    for (int i = 0; i < TOTAL_DIFICULTADES; i++) {
        bool elegida = (i == opcion);
        Color color = elegida ? colorBloque(pal, i + 1) : ColorBrightness(pal.marco, -0.35f);
        const char* nombre = nombreDificultad(i);
        const char* texto = elegida ? (parpadeo ? TextFormat("> %s <", nombre)
            : TextFormat("  %s  ", nombre))
            : nombre;
        dibujarTextoCentrado(fuente, texto, centro, 190 + i * 65, 32, color);
    }

    // Recuadro con lo único que se muestra de la dificultad elegida
    static const char* DESCRIPCION[TOTAL_DIFICULTADES] = {
        "PIEZAS PEQUENAS", "PIEZAS CLASICAS", "PIEZAS GRANDES"
    };
    int cajaY = 430;
    DrawRectangle(60, cajaY, 480, 80, pal.marco);
    DrawRectangle(64, cajaY + 4, 472, 72, pal.fondoTablero);
    dibujarTextoCentrado(fuente, DESCRIPCION[opcion], centro, cajaY + 32, 16, colorBloque(pal, opcion + 1));

    dibujarTextoCentrado(fuente, TextFormat("RECORD %06d", record), centro, 540, 16, pal.texto);
    dibujarTextoCentrado(fuente, "ENTER JUGAR  ESC VOLVER", centro, 630, 16, pal.marco);
}

void dibujarPausa(const Font& fuente, const Paleta& pal, const char* aviso) {
    int centro = ANCHO_VENTANA / 2;
    DrawRectangle(0, 0, ANCHO_VENTANA, ALTO_VENTANA, Fade(BLACK, 0.65f));
    dibujarTextoCentrado(fuente, "PAUSA", centro, 260, 48, pal.texto);
    if (aviso != nullptr) {
        dibujarTextoCentrado(fuente, aviso, centro, 318, 16, pal.texto);
    }
    dibujarTextoCentrado(fuente, "ESC CONTINUAR", centro, 345, 16, pal.marco);
    dibujarTextoCentrado(fuente, "G GUARDAR", centro, 375, 16, pal.marco);
    dibujarTextoCentrado(fuente, "ENTER MENU", centro, 405, 16, pal.marco);
}