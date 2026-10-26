#include "game.h"
#include "effects.h"

static const float DURACION_ANIMACION = 0.45f;   // segundos que parpadean las líneas
static const float DURACION_BANNER = 1.8f;       // segundos que se ve el aviso de nivel
static const float DURACION_MENSAJE = 1.2f;      // segundos que se ve el aviso de habilidad
static const float INTERVALO_RAPIDO = 0.05f;     // caída al mantener ↓ o S
static const float INTERVALO_MINIMO = 0.10f;     // la caída nunca baja de esto

// Teclas de las habilidades, en el mismo orden que datosHabilidad(): CAMBIAR, CONGELAR, LIMPIAR
static const int TECLAS_HABILIDAD[TOTAL_HABILIDADES] = { KEY_Q, KEY_E, KEY_R };

static int puntosPorLineas(int n) {
    switch (n) {
    case 1: return 100;
    case 2: return 300;
    case 3: return 500;
    case 4: return 800;
    default: return 0;
    }
}

// Segundos que tarda en bajar una casilla: la base de la dificultad por el factor del nivel
static float intervaloDeCaida(const Partida& p) {
    float intervalo = p.cfg.intervalo * datosNivel(p.nivel).factorVelocidad;
    if (p.tiempoCongelado > 0.0f) intervalo *= FACTOR_CONGELAR;
    if (intervalo < INTERVALO_MINIMO) intervalo = INTERVALO_MINIMO;
    return intervalo;
}

// Empieza una partida nueva con la dificultad indicada
void reiniciar(Partida& p, int dificultad, Sonidos& sonidos) {
    p.dificultad = dificultad;
    p.cfg = configPorIndice(dificultad);
    iniciarTablero(p.tablero);
    nuevaPieza(p.pieza, p.cfg);
    p.puntaje = 0;
    p.lineas = 0;
    p.nivel = 1;
    p.lineasNivel = 0;
    p.tiempoBanner = 0.0f;
    p.energia = 0;
    p.tiempoCongelado = 0.0f;
    p.mensaje = nullptr;
    p.mensajeError = false;
    p.tiempoMensaje = 0.0f;
    p.temporizador = 0.0f;
    p.juegoTerminado = false;
    p.animando = false;
    p.tiempoAnimacion = 0.0f;
    for (int f = 0; f < FILAS; f++) p.filasLlenas[f] = false;
    limpiarParticulas();
    iniciarMusica(sonidos, p.cfg.pistaMusica);
}

// Pasa al siguiente nivel: cambia la velocidad y la paleta, y avisa en pantalla
void subirNivel(Partida& p, Sonidos& sonidos) {
    if (p.nivel >= TOTAL_NIVELES) return;
    p.nivel++;
    p.lineasNivel = 0;
    p.tiempoBanner = DURACION_BANNER;
    reproducir(sonidos.nivel);
}

// Sube de nivel las veces que haga falta según las líneas borradas
static void revisarNivel(Partida& p, Sonidos& sonidos) {
    while (p.nivel < TOTAL_NIVELES && p.lineasNivel >= datosNivel(p.nivel).lineasParaSubir) {
        int sobrante = p.lineasNivel - datosNivel(p.nivel).lineasParaSubir;
        subirNivel(p, sonidos);
        p.lineasNivel = sobrante;       // las líneas de más cuentan para el nivel nuevo
    }
}

// ------------------------------------------------------------ habilidades
static void avisar(Partida& p, const char* texto, bool esError) {
    p.mensaje = texto;
    p.mensajeError = esError;
    p.tiempoMensaje = DURACION_MENSAJE;
}

static void fallar(Partida& p, Sonidos& sonidos, const char* motivo) {
    avisar(p, motivo, true);
    reproducir(sonidos.error);
}

// Intenta usar la habilidad "i" (0 = CAMBIAR, 1 = CONGELAR, 2 = LIMPIAR)
static void usarHabilidad(Partida& p, int i, const Paleta& pal, Sonidos& sonidos, float& temblor) {
    const Habilidad& h = datosHabilidad(i);

    if ((datosNivel(p.nivel).habilidades & (1 << i)) == 0) {
        fallar(p, sonidos, "BLOQUEADA");
        return;
    }
    if (p.energia < h.costo) {
        fallar(p, sonidos, "SIN ENERGIA");
        return;
    }

    bool funciono = false;

    if (i == 0) {
        // CAMBIAR: la pieza que cae se reemplaza por otra al azar, en el mismo lugar
        Pieza nueva = p.pieza;
        nuevaPieza(nueva, p.cfg);
        nueva.fila = p.pieza.fila;
        nueva.col = p.pieza.col;
        if (!hayColision(p.tablero, nueva)) {
            p.pieza = nueva;
            funciono = true;
        }
    }
    else if (i == 1) {
        // CONGELAR: la caída se hace más lenta por unos segundos
        if (p.tiempoCongelado <= 0.0f) {
            p.tiempoCongelado = DURACION_CONGELAR;
            funciono = true;
        }
    }
    else if (i == 2) {
        // LIMPIAR: borra las filas de abajo y baja todo lo demás
        if (contarBloquesInferiores(p.tablero, FILAS_LIMPIAR) > 0) {
            for (int f = FILAS - FILAS_LIMPIAR; f < FILAS; f++) {
                for (int c = 0; c < COLUMNAS; c++) {
                    if (p.tablero.celdas[f][c] != 0) {
                        float x = (float)(ORIGEN_X + c * TAM_CELDA + TAM_CELDA / 2);
                        float y = (float)(ORIGEN_Y + f * TAM_CELDA + TAM_CELDA / 2);
                        emitirParticula(x, y, colorBloque(pal, p.tablero.celdas[f][c]));
                    }
                }
            }
            borrarFilasInferiores(p.tablero, FILAS_LIMPIAR);
            temblor = 6.0f;
            funciono = true;
        }
    }

    if (!funciono) {
        fallar(p, sonidos, "NO SE PUEDE");
        return;
    }
    p.energia -= h.costo;
    avisar(p, h.nombre, false);
    reproducir(sonidos.habilidad);
}

// ------------------------------------------------------------ partida
// Saca la pieza siguiente; si ya no cabe, se acaba el juego
static void sacarPieza(Partida& p, Sonidos& sonidos) {
    nuevaPieza(p.pieza, p.cfg);
    if (hayColision(p.tablero, p.pieza)) {
        p.juegoTerminado = true;
        reproducir(sonidos.gameover);
        detenerMusica(sonidos);
    }
}

// Se llama cuando la pieza toca el fondo o a otra pieza
static void aterrizar(Partida& p, const Paleta& pal, Sonidos& sonidos, float& temblor) {
    fijarPieza(p.tablero, p.pieza);
    int cantidad = detectarLineas(p.tablero, p.filasLlenas);

    if (cantidad == 0) {
        reproducir(sonidos.caer);
        sacarPieza(p, sonidos);
        return;
    }

    // Hay líneas completas: arranca la animación (se borran cuando termine)
    p.animando = true;
    p.tiempoAnimacion = 0.0f;
    reproducir(cantidad == 4 ? sonidos.tetris : sonidos.linea);
    temblor = 2.0f + 3.0f * cantidad;

    for (int f = 0; f < FILAS; f++) {
        if (p.filasLlenas[f]) {
            for (int c = 0; c < COLUMNAS; c++) {
                float x = (float)(ORIGEN_X + c * TAM_CELDA + TAM_CELDA / 2);
                float y = (float)(ORIGEN_Y + f * TAM_CELDA + TAM_CELDA / 2);
                Color color = colorBloque(pal, p.tablero.celdas[f][c]);
                emitirParticula(x, y, color);
                emitirParticula(x, y, color);
            }
        }
    }
}

void actualizarJuego(Partida& p, const Paleta& pal, Sonidos& sonidos, float& temblor, float dt) {
    if (p.tiempoBanner > 0.0f) p.tiempoBanner -= dt;
    if (p.tiempoMensaje > 0.0f) p.tiempoMensaje -= dt;
    if (p.tiempoCongelado > 0.0f) p.tiempoCongelado -= dt;

    if (p.juegoTerminado) {
        if (IsKeyPressed(KEY_ENTER)) {
            reiniciar(p, p.dificultad, sonidos);
        }
        return;
    }

    if (p.animando) {
        // Las líneas parpadean; al terminar se borran y sigue el juego
        p.tiempoAnimacion += dt;
        if (p.tiempoAnimacion >= DURACION_ANIMACION) {
            int borradas = borrarLineas(p.tablero);
            p.lineas += borradas;
            p.lineasNivel += borradas;
            p.puntaje += puntosPorLineas(borradas);
            p.energia += borradas * ENERGIA_POR_LINEA;
            if (p.energia > ENERGIA_MAXIMA) p.energia = ENERGIA_MAXIMA;
            revisarNivel(p, sonidos);
            p.animando = false;
            p.temporizador = 0.0f;
            sacarPieza(p, sonidos);
        }
        return;
    }

    // 1. Teclado
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT) ||
        IsKeyPressed(KEY_A) || IsKeyPressedRepeat(KEY_A)) {
        p.pieza.col -= 1;
        if (hayColision(p.tablero, p.pieza)) p.pieza.col += 1;
        else reproducir(sonidos.mover);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT) ||
        IsKeyPressed(KEY_D) || IsKeyPressedRepeat(KEY_D)) {
        p.pieza.col += 1;
        if (hayColision(p.tablero, p.pieza)) p.pieza.col -= 1;
        else reproducir(sonidos.mover);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        p.pieza.rotacion = (p.pieza.rotacion + 1) % 4;
        if (hayColision(p.tablero, p.pieza)) {
            p.pieza.rotacion = (p.pieza.rotacion + 3) % 4;
        }
        else {
            reproducir(sonidos.rotar);
        }
    }
    for (int i = 0; i < TOTAL_HABILIDADES; i++) {
        if (IsKeyPressed(TECLAS_HABILIDAD[i])) usarHabilidad(p, i, pal, sonidos, temblor);
    }

    // 2. Caída clásica
    bool bajarRapido = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    float intervaloActual = bajarRapido ? INTERVALO_RAPIDO : intervaloDeCaida(p);

    p.temporizador += dt;
    if (p.temporizador >= intervaloActual) {
        p.temporizador = 0.0f;
        p.pieza.fila += 1;
        if (hayColision(p.tablero, p.pieza)) {
            p.pieza.fila -= 1;
            aterrizar(p, pal, sonidos, temblor);
        }
    }
}

// ------------------------------------------------------------ dibujo
static void dibujarBarra(const Paleta& pal, int x, int y, int ancho, float avance, Color relleno) {
    if (avance > 1.0f) avance = 1.0f;
    if (avance < 0.0f) avance = 0.0f;
    DrawRectangle(x, y, ancho, 18, pal.marco);
    DrawRectangle(x + 3, y + 3, ancho - 6, 12, pal.fondoTablero);
    DrawRectangle(x + 3, y + 3, (int)((ancho - 6) * avance), 12, relleno);
}

// Tres casillas con las habilidades: "?" si siguen bloqueadas, apagadas si falta energía
// y parpadeando cuando ya se pueden usar
static void dibujarCasillasHabilidades(const Partida& p, const Font& fuente, const Paleta& pal, int x0, int y) {
    int desbloqueadas = datosNivel(p.nivel).habilidades;
    bool parpadeo = ((int)(GetTime() * 3.0) % 2 == 0);

    for (int i = 0; i < TOTAL_HABILIDADES; i++) {
        const Habilidad& h = datosHabilidad(i);
        int x = x0 + i * 67;
        bool abierta = (desbloqueadas & (1 << i)) != 0;
        bool lista = abierta && p.energia >= h.costo;

        Color borde = !abierta ? ColorBrightness(pal.marco, -0.6f)
            : lista ? (parpadeo ? pal.texto : pal.marco)
            : ColorBrightness(pal.marco, -0.35f);
        DrawRectangle(x, y, 56, 56, borde);
        DrawRectangle(x + 4, y + 4, 48, 48, pal.fondoTablero);

        if (!abierta) {
            dibujarTextoCentrado(fuente, "?", x + 28, y + 20, 16, ColorBrightness(pal.marco, -0.6f));
        }
        else {
            Color texto = lista ? pal.texto : ColorBrightness(pal.marco, -0.35f);
            dibujarTextoCentrado(fuente, h.tecla, x + 28, y + 10, 16, texto);
            dibujarTextoCentrado(fuente, TextFormat("%d", h.costo), x + 28, y + 32, 16, texto);
        }
    }
}

void dibujarJuego(const Partida& p, const Font& fuente, const Paleta& pal, float temblor) {
    Camera2D camara = {};
    camara.zoom = 1.0f;
    camara.offset = desplazamientoTemblor(temblor);

    bool brillo = p.animando && ((int)(p.tiempoAnimacion * 16.0f) % 2 == 0);
    int centro = ORIGEN_X + COLUMNAS * TAM_CELDA / 2;

    BeginMode2D(camara);

    dibujarTablero(p.tablero, ORIGEN_X, ORIGEN_Y, pal, p.filasLlenas, brillo);
    if (!p.juegoTerminado && !p.animando) {
        int filaFinal = filaFantasma(p.tablero, p.pieza);
        dibujarFantasma(p.pieza, filaFinal, ORIGEN_X, ORIGEN_Y, pal);
        dibujarPieza(p.pieza, ORIGEN_X, ORIGEN_Y, pal);
    }
    dibujarParticulas();

    // Mientras la caída está congelada, el tablero se ve con un tinte helado
    if (p.tiempoCongelado > 0.0f && !p.juegoTerminado) {
        DrawRectangle(ORIGEN_X, ORIGEN_Y, COLUMNAS * TAM_CELDA, FILAS * TAM_CELDA, Fade(SKYBLUE, 0.12f));
        dibujarTextoCentrado(fuente, TextFormat("CONGELADO %d", (int)p.tiempoCongelado + 1),
            centro, ORIGEN_Y + FILAS * TAM_CELDA - 30, 16, SKYBLUE);
    }

    // Panel derecho: puntaje, líneas, nivel, avance de nivel, energía y habilidades
    const int PANEL_X = 390;
    const int PANEL_ANCHO = 190;
    dibujarPanel(fuente, pal, PANEL_X, 50, PANEL_ANCHO, 70, "PUNTAJE",
        TextFormat("%06d", p.puntaje));
    dibujarPanel(fuente, pal, PANEL_X, 140, PANEL_ANCHO, 70, "LINEAS",
        TextFormat("%03d", p.lineas));
    dibujarPanel(fuente, pal, PANEL_X, 230, PANEL_ANCHO, 70, "NIVEL",
        TextFormat("%02d", p.nivel));

    bool maximo = p.nivel >= TOTAL_NIVELES;
    float avance = 1.0f;
    if (!maximo) {
        avance = (float)p.lineasNivel / (float)datosNivel(p.nivel).lineasParaSubir;
    }
    DrawTextEx(fuente, maximo ? "MAXIMO" : "SIGUIENTE", Vector2{ (float)PANEL_X, 315 }, 16, 2, pal.marco);
    dibujarBarra(pal, PANEL_X, 340, PANEL_ANCHO, avance, colorBloque(pal, 1));

    DrawTextEx(fuente, "ENERGIA", Vector2{ (float)PANEL_X, 380 }, 16, 2, pal.marco);
    dibujarBarra(pal, PANEL_X, 405, PANEL_ANCHO, (float)p.energia / (float)ENERGIA_MAXIMA, colorBloque(pal, 3));
    dibujarCasillasHabilidades(p, fuente, pal, PANEL_X, 440);

    DrawTextEx(fuente, nombreDificultad(p.dificultad), Vector2{ (float)PANEL_X, 520 }, 16, 2, pal.texto);
    DrawTextEx(fuente, "ESC PAUSA", Vector2{ (float)PANEL_X, 570 }, 16, 2, pal.marco);
    DrawTextEx(fuente, "M SONIDO", Vector2{ (float)PANEL_X, 600 }, 16, 2, pal.marco);

    // Aviso corto al usar (o fallar) una habilidad
    if (p.tiempoMensaje > 0.0f && p.mensaje != nullptr && !p.juegoTerminado) {
        float opacidad = p.tiempoMensaje / 0.3f;
        if (opacidad > 1.0f) opacidad = 1.0f;
        DrawRectangle(ORIGEN_X, ORIGEN_Y + 8, COLUMNAS * TAM_CELDA, 32, Fade(BLACK, 0.65f * opacidad));
        dibujarTextoCentrado(fuente, p.mensaje, centro, ORIGEN_Y + 16, 16,
            Fade(p.mensajeError ? RED : pal.texto, opacidad));
    }

    // Aviso al subir de nivel: destello, y un letrero que aparece y se desvanece
    if (p.tiempoBanner > 0.0f && !p.juegoTerminado) {
        float transcurrido = DURACION_BANNER - p.tiempoBanner;
        float entra = transcurrido / 0.25f;
        float sale = p.tiempoBanner / 0.6f;
        float opacidad = (entra < 1.0f ? entra : 1.0f) * (sale < 1.0f ? sale : 1.0f);

        // ¿Este nivel desbloquea una habilidad nueva?
        int nueva = -1;
        if (p.nivel > 1) {
            int nuevas = datosNivel(p.nivel).habilidades & ~datosNivel(p.nivel - 1).habilidades;
            for (int i = 0; i < TOTAL_HABILIDADES; i++) {
                if (nuevas & (1 << i)) { nueva = i; break; }
            }
        }

        if (transcurrido < 0.25f) {
            DrawRectangle(ORIGEN_X, ORIGEN_Y, COLUMNAS * TAM_CELDA, FILAS * TAM_CELDA,
                Fade(WHITE, 0.35f * (1.0f - entra)));
        }
        int alto = (nueva >= 0) ? 150 : 90;
        DrawRectangle(ORIGEN_X, 255, COLUMNAS * TAM_CELDA, alto, Fade(BLACK, 0.7f * opacidad));
        dibujarTextoCentrado(fuente, "SUBISTE A", centro, 270, 16, Fade(pal.marco, opacidad));
        dibujarTextoCentrado(fuente, TextFormat("NIVEL %d", p.nivel), centro, 300, 32,
            Fade(pal.texto, opacidad));
        if (nueva >= 0) {
            const Habilidad& h = datosHabilidad(nueva);
            dibujarTextoCentrado(fuente, "NUEVA HABILIDAD", centro, 350, 16, Fade(pal.marco, opacidad));
            dibujarTextoCentrado(fuente, TextFormat("[%s] %s", h.tecla, h.nombre), centro, 380, 16,
                Fade(pal.texto, opacidad));
        }
    }

    if (p.juegoTerminado) {
        DrawRectangle(ORIGEN_X, ORIGEN_Y, COLUMNAS * TAM_CELDA, FILAS * TAM_CELDA,
            Fade(BLACK, 0.75f));
        dibujarTextoCentrado(fuente, "GAME", centro, 250, 32, RED);
        dibujarTextoCentrado(fuente, "OVER", centro, 295, 32, RED);
        dibujarTextoCentrado(fuente, TextFormat("PUNTOS %06d", p.puntaje),
            centro, 360, 16, pal.texto);
        dibujarTextoCentrado(fuente, "ENTER REINICIA", centro, 400, 16, pal.marco);
        dibujarTextoCentrado(fuente, "ESC MENU", centro, 430, 16, pal.marco);
    }

    EndMode2D();
}

// Lista de habilidades que se muestra en la pantalla de pausa (las bloqueadas salen como ???)
void dibujarAyudaPausa(const Partida& p, const Font& fuente, const Paleta& pal) {
    int centro = ANCHO_VENTANA / 2;
    int desbloqueadas = datosNivel(p.nivel).habilidades;

    dibujarTextoCentrado(fuente, "HABILIDADES", centro, 450, 16, pal.marco);
    for (int i = 0; i < TOTAL_HABILIDADES; i++) {
        const Habilidad& h = datosHabilidad(i);
        bool abierta = (desbloqueadas & (1 << i)) != 0;
        const char* texto = abierta ? TextFormat("%s %s %d", h.tecla, h.nombre, h.costo) : "? ???";
        Color color = abierta ? pal.texto : ColorBrightness(pal.marco, -0.5f);
        dibujarTextoCentrado(fuente, texto, centro, 485 + i * 30, 16, color);
    }
}

// Prepara una partida cargada de un archivo para seguir jugándola
void reanudar(Partida& p, Sonidos& sonidos) {
    p.cfg = configPorIndice(p.dificultad);
    p.tiempoBanner = 0.0f;
    p.tiempoCongelado = 0.0f;
    p.mensaje = nullptr;
    p.mensajeError = false;
    p.tiempoMensaje = 0.0f;
    p.temporizador = 0.0f;
    p.juegoTerminado = false;
    p.animando = false;
    p.tiempoAnimacion = 0.0f;
    for (int f = 0; f < FILAS; f++) p.filasLlenas[f] = false;
    limpiarParticulas();
    iniciarMusica(sonidos, p.cfg.pistaMusica);
}