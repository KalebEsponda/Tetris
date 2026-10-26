#include "raylib.h"
#include <cstring>
#include "game.h"
#include "menu.h"
#include "save.h"
#include "style.h"
#include "effects.h"
#include "audio.h"

// En qué pantalla está el programa
enum Estado {
    ESTADO_PERFILES,
    ESTADO_NOMBRE,
    ESTADO_MENU,
    ESTADO_DIFICULTAD,
    ESTADO_JUGANDO,
    ESTADO_PAUSA
};

// Qué hace cada opción del menú principal
enum Accion {
    ACCION_CONTINUAR,
    ACCION_JUGAR,
    ACCION_GUARDAR,
    ACCION_CARGAR,
    ACCION_SALIR
};

// Lee las 3 ranuras del disco y deja la selección en un lugar útil
static void abrirPerfiles(ModoPerfiles modo, ModoPerfiles& modoActual, Perfil perfiles[TOTAL_PERFILES],
    bool ocupada[TOTAL_PERFILES], int ranuraActual, int& seleccion,
    bool& confirmando, const char*& aviso, float& tiempoAviso) {
    int cuantos = leerPerfiles(perfiles, ocupada);
    modoActual = modo;
    confirmando = false;
    aviso = nullptr;
    tiempoAviso = 0.0f;
    seleccion = 0;

    if (modo == PERFILES_GUARDAR) {
        for (int i = 0; i < TOTAL_PERFILES; i++) {
            if (!ocupada[i]) { seleccion = i; break; }
        }
        if (cuantos >= TOTAL_PERFILES) {
            aviso = "SIN ESPACIO: BORRA UNO";
            tiempoAviso = 3.0f;
        }
    }
    else if (ranuraActual >= 0) {
        seleccion = ranuraActual;
    }
    else {
        for (int i = 0; i < TOTAL_PERFILES; i++) {
            if (ocupada[i]) { seleccion = i; break; }
        }
    }
}

int main() {
    InitWindow(ANCHO_VENTANA, ALTO_VENTANA, "Tetris");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);                          // ESC ya no cierra: sirve para pausar y volver
    ChangeDirectory(GetApplicationDirectory());    // para que encuentre assets y los archivos guardados
    InitAudioDevice();

    Font fuente = LoadFontEx("assets/PressStart2P-Regular.ttf", 16, 0, 0);
    SetTextureFilter(fuente.texture, TEXTURE_FILTER_POINT);

    Sonidos sonidos = {};
    cargarSonidos(sonidos);

    Partida partida = {};
    Perfil perfil = {};                         // el perfil con el que se está jugando
    int ranuraActual = -1;                      // en qué ranura (0 a 2) vive ese perfil; -1 = ninguno
    Perfil perfiles[TOTAL_PERFILES] = {};       // lo que hay en las 3 ranuras (para mostrar la lista)
    bool ocupada[TOTAL_PERFILES] = {};
    ModoPerfiles modoPerfiles = PERFILES_ELEGIR;
    int seleccionPerfil = 0;
    int ranuraNueva = 0;                        // ranura donde se está creando un perfil
    bool confirmandoBorrado = false;
    char nombreEscrito[LARGO_NOMBRE + 1] = "";
    Estado estado = ESTADO_PERFILES;
    int opcionMenu = 0;
    int opcionDificultad = 1;    // empieza marcada NORMAL
    float temblor = 0.0f;
    float tiempo = 0.0f;
    bool salir = false;
    bool resultadoRegistrado = false;
    const char* aviso = nullptr;                // aviso corto de la pantalla actual
    float tiempoAviso = 0.0f;

    // Al abrir el juego se muestra la lista de perfiles, con el último jugador ya marcado
    abrirPerfiles(PERFILES_ELEGIR, modoPerfiles, perfiles, ocupada, -1, seleccionPerfil,
        confirmandoBorrado, aviso, tiempoAviso);
    int ultima = leerUltimaRanura();
    if (ultima >= 0 && ocupada[ultima]) seleccionPerfil = ultima;

    iniciarFondoMenu();
    iniciarMusica(sonidos, 1);   // música del menú

    while (!salir && !WindowShouldClose()) {
        float dt = GetFrameTime();
        tiempo += dt;
        actualizarMusica(sonidos);
        actualizarParticulas(dt);
        actualizarFondoMenu(dt);
        if (temblor > 0.0f) {
            temblor -= 30.0f * dt;
            if (temblor < 0.0f) temblor = 0.0f;
        }
        if (tiempoAviso > 0.0f) tiempoAviso -= dt;

        // Fuera de la pantalla del nombre, las teclas son atajos (M = silencio) y las letras se descartan
        if (estado != ESTADO_NOMBRE) {
            while (GetCharPressed() > 0) {}
            if (IsKeyPressed(KEY_M)) alternarSilencio(sonidos);
        }

        // Los menús usan siempre la paleta 0; la partida usa la de su nivel
        bool enPartida = (estado == ESTADO_JUGANDO || estado == ESTADO_PAUSA);
        const Paleta& pal = enPartida ? obtenerPaleta(datosNivel(partida.nivel).paleta)
            : obtenerPaleta(0);

        bool arriba = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
        bool abajo = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
        bool aceptar = IsKeyPressed(KEY_ENTER);
        bool volver = IsKeyPressed(KEY_ESCAPE);
        bool borrar = IsKeyPressed(KEY_X) || IsKeyPressed(KEY_DELETE);

        // Opciones del menú principal (CONTINUAR solo aparece si hay una partida guardada)
        const char* opciones[5];
        Accion acciones[5];
        int totalOpciones = 0;
        if (perfil.hayPartida) {
            opciones[totalOpciones] = "CONTINUAR";
            acciones[totalOpciones] = ACCION_CONTINUAR;
            totalOpciones++;
        }
        opciones[totalOpciones] = "JUGAR";
        acciones[totalOpciones] = ACCION_JUGAR;
        totalOpciones++;
        opciones[totalOpciones] = "GUARDAR";
        acciones[totalOpciones] = ACCION_GUARDAR;
        totalOpciones++;
        opciones[totalOpciones] = "CARGAR";
        acciones[totalOpciones] = ACCION_CARGAR;
        totalOpciones++;
        opciones[totalOpciones] = "SALIR";
        acciones[totalOpciones] = ACCION_SALIR;
        totalOpciones++;
        if (opcionMenu >= totalOpciones) opcionMenu = 0;

        // 1. Actualizar según la pantalla
        if (estado == ESTADO_PERFILES) {
            if (confirmandoBorrado) {
                if (aceptar) {
                    borrarPerfil(seleccionPerfil);
                    if (seleccionPerfil == ranuraActual) {      // se borró el perfil activo
                        perfil = Perfil{};
                        ranuraActual = -1;
                    }
                    leerPerfiles(perfiles, ocupada);
                    confirmandoBorrado = false;
                    aviso = "PERFIL BORRADO";
                    tiempoAviso = 2.0f;
                    reproducir(sonidos.error);
                }
                else if (volver) {
                    confirmandoBorrado = false;
                }
            }
            else {
                if (arriba) {
                    seleccionPerfil = (seleccionPerfil + TOTAL_PERFILES - 1) % TOTAL_PERFILES;
                    reproducir(sonidos.mover);
                }
                if (abajo) {
                    seleccionPerfil = (seleccionPerfil + 1) % TOTAL_PERFILES;
                    reproducir(sonidos.mover);
                }

                if (aceptar) {
                    bool llena = ocupada[seleccionPerfil];
                    if (llena && modoPerfiles != PERFILES_GUARDAR) {
                        // Cargar: ese perfil pasa a ser el activo
                        perfil = perfiles[seleccionPerfil];
                        ranuraActual = seleccionPerfil;
                        recordarUltimaRanura(ranuraActual);
                        reproducir(sonidos.rotar);
                        opcionMenu = 0;
                        estado = ESTADO_MENU;
                    }
                    else if (!llena && modoPerfiles != PERFILES_CARGAR) {
                        // Guardar: crear un perfil nuevo en esa ranura
                        ranuraNueva = seleccionPerfil;
                        nombreEscrito[0] = '\0';
                        while (GetCharPressed() > 0) {}
                        aviso = nullptr;
                        tiempoAviso = 0.0f;
                        reproducir(sonidos.rotar);
                        estado = ESTADO_NOMBRE;
                    }
                    else {
                        aviso = llena ? "ESA RANURA YA ESTA OCUPADA" : "ESA RANURA ESTA VACIA";
                        tiempoAviso = 2.0f;
                        reproducir(sonidos.error);
                    }
                }
                else if (borrar) {
                    if (ocupada[seleccionPerfil]) {
                        confirmandoBorrado = true;
                    }
                    else {
                        aviso = "ESA RANURA ESTA VACIA";
                        tiempoAviso = 2.0f;
                        reproducir(sonidos.error);
                    }
                }
                else if (volver && ranuraActual >= 0) {
                    estado = ESTADO_MENU;
                }
            }
        }
        else if (estado == ESTADO_NOMBRE) {
            int largo = (int)strlen(nombreEscrito);
            int letra = GetCharPressed();
            while (letra > 0) {
                if (letra < 128 && largo < LARGO_NOMBRE) {
                    char c = (char)letra;
                    if (c >= 'a' && c <= 'z') c = (char)(c - 32);          // todo en mayúsculas
                    if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                        nombreEscrito[largo] = c;
                        largo++;
                        nombreEscrito[largo] = '\0';
                    }
                }
                letra = GetCharPressed();
            }
            if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && largo > 0) {
                nombreEscrito[largo - 1] = '\0';
            }

            if (aceptar && nombreValido(nombreEscrito)) {
                if (nombreEnUso(perfiles, ocupada, nombreEscrito)) {
                    aviso = "ESE NOMBRE YA EXISTE";
                    tiempoAviso = 2.0f;
                    reproducir(sonidos.error);
                }
                else {
                    Perfil nuevo;
                    nuevoPerfil(nuevo, nombreEscrito);
                    if (guardarPerfil(nuevo, ranuraNueva)) {
                        perfil = nuevo;                         // el perfil nuevo pasa a ser el activo
                        ranuraActual = ranuraNueva;
                        recordarUltimaRanura(ranuraActual);
                        reproducir(sonidos.rotar);
                        opcionMenu = 0;
                        estado = ESTADO_MENU;
                    }
                    else {
                        aviso = "NO SE PUDO GUARDAR";
                        tiempoAviso = 2.0f;
                        reproducir(sonidos.error);
                    }
                }
            }
            else if (volver) {                                // vuelve a la lista de perfiles
                abrirPerfiles(modoPerfiles, modoPerfiles, perfiles, ocupada, ranuraActual,
                    seleccionPerfil, confirmandoBorrado, aviso, tiempoAviso);
                seleccionPerfil = ranuraNueva;
                estado = ESTADO_PERFILES;
            }
        }
        else if (estado == ESTADO_MENU) {
            if (arriba) {
                opcionMenu = (opcionMenu + totalOpciones - 1) % totalOpciones;
                reproducir(sonidos.mover);
            }
            if (abajo) {
                opcionMenu = (opcionMenu + 1) % totalOpciones;
                reproducir(sonidos.mover);
            }
            if (aceptar) {
                reproducir(sonidos.rotar);
                Accion accion = acciones[opcionMenu];
                if (accion == ACCION_CONTINUAR) {
                    partida = perfil.guardada;
                    reanudar(partida, sonidos);
                    pausarMusica(sonidos);          // se entra en pausa para tener tiempo de prepararse
                    resultadoRegistrado = false;
                    aviso = nullptr;
                    tiempoAviso = 0.0f;
                    estado = ESTADO_PAUSA;
                }
                else if (accion == ACCION_JUGAR) {
                    estado = ESTADO_DIFICULTAD;
                }
                else if (accion == ACCION_GUARDAR || accion == ACCION_CARGAR) {
                    abrirPerfiles(accion == ACCION_GUARDAR ? PERFILES_GUARDAR : PERFILES_CARGAR,
                        modoPerfiles, perfiles, ocupada, ranuraActual, seleccionPerfil,
                        confirmandoBorrado, aviso, tiempoAviso);
                    estado = ESTADO_PERFILES;
                }
                else {
                    salir = true;
                }
            }
        }
        else if (estado == ESTADO_DIFICULTAD) {
            if (arriba) {
                opcionDificultad = (opcionDificultad + TOTAL_DIFICULTADES - 1) % TOTAL_DIFICULTADES;
                reproducir(sonidos.mover);
            }
            if (abajo) {
                opcionDificultad = (opcionDificultad + 1) % TOTAL_DIFICULTADES;
                reproducir(sonidos.mover);
            }
            if (aceptar) {
                reproducir(sonidos.rotar);
                reiniciar(partida, opcionDificultad, sonidos);   // arranca la música de esa dificultad
                resultadoRegistrado = false;
                estado = ESTADO_JUGANDO;
            }
            else if (volver) {
                estado = ESTADO_MENU;
            }
        }
        else if (estado == ESTADO_JUGANDO) {
            if (volver) {
                if (partida.juegoTerminado) {
                    iniciarMusica(sonidos, 1);
                    opcionMenu = 0;
                    estado = ESTADO_MENU;
                }
                else {
                    pausarMusica(sonidos);
                    aviso = nullptr;
                    tiempoAviso = 0.0f;
                    estado = ESTADO_PAUSA;
                }
            }
            else {
                // TEMPORAL para probar: N sube de nivel (bórralo al terminar de probar)
                if (IsKeyPressed(KEY_N) && !partida.juegoTerminado) subirNivel(partida, sonidos);
                actualizarJuego(partida, pal, sonidos, temblor, dt);

                // Al perder, se anota el récord una sola vez
                if (!partida.juegoTerminado) {
                    resultadoRegistrado = false;
                }
                else if (!resultadoRegistrado) {
                    registrarResultado(perfil, ranuraActual, partida);
                    resultadoRegistrado = true;
                }
            }
        }
        else if (estado == ESTADO_PAUSA) {
            if (volver) {
                reanudarMusica(sonidos);
                estado = ESTADO_JUGANDO;
            }
            else if (IsKeyPressed(KEY_G)) {
                if (partida.animando) {
                    aviso = "AHORA NO";                              // las líneas aún se están borrando
                    reproducir(sonidos.error);
                }
                else if (guardarPartidaEnPerfil(perfil, ranuraActual, partida)) {
                    aviso = "PARTIDA GUARDADA";
                    reproducir(sonidos.habilidad);
                }
                else {
                    aviso = "NO SE PUDO GUARDAR";
                    reproducir(sonidos.error);
                }
                tiempoAviso = 2.0f;
            }
            else if (aceptar) {
                iniciarMusica(sonidos, 1);
                opcionMenu = 0;
                estado = ESTADO_MENU;
            }
        }

        // 2. Dibujar según la pantalla
        const char* avisoVisible = (tiempoAviso > 0.0f) ? aviso : nullptr;
        BeginDrawing();
        ClearBackground(pal.fondo);
        if (estado == ESTADO_PERFILES) {
            dibujarFondoMenu(pal);
            dibujarPantallaPerfiles(fuente, pal, modoPerfiles, perfiles, ocupada, seleccionPerfil,
                avisoVisible, confirmandoBorrado, ranuraActual >= 0, tiempo);
        }
        else if (estado == ESTADO_NOMBRE) {
            dibujarFondoMenu(pal);
            dibujarPantallaNombre(fuente, pal, nombreEscrito, avisoVisible, tiempo);
        }
        else if (estado == ESTADO_MENU) {
            dibujarFondoMenu(pal);
            dibujarMenuPrincipal(fuente, pal, perfil.nombre, opciones, totalOpciones, opcionMenu, tiempo);
        }
        else if (estado == ESTADO_DIFICULTAD) {
            dibujarFondoMenu(pal);
            dibujarMenuDificultad(fuente, pal, opcionDificultad, tiempo, perfil.mejorPuntaje[opcionDificultad]);
        }
        else {
            dibujarJuego(partida, fuente, pal, temblor);
            if (estado == ESTADO_PAUSA) {
                dibujarPausa(fuente, pal, avisoVisible);
                dibujarAyudaPausa(partida, fuente, pal);
            }
        }
        EndDrawing();
    }

    liberarSonidos(sonidos);
    UnloadFont(fuente);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}