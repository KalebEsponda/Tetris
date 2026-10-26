#pragma once
#include "raylib.h"
#include "style.h"
#include "save.h"

// Para qué se abrió la pantalla de perfiles
enum ModoPerfiles {
    PERFILES_ELEGIR,    // al abrir el juego: elegir uno o crear uno
    PERFILES_GUARDAR,   // desde el menú: crear un perfil nuevo (solo ranuras vacías)
    PERFILES_CARGAR     // desde el menú: cambiar a un perfil que ya existe
};

void iniciarFondoMenu();
void actualizarFondoMenu(float dt);
void dibujarFondoMenu(const Paleta& pal);
void dibujarPantallaPerfiles(const Font& fuente, const Paleta& pal, ModoPerfiles modo,
    const Perfil perfiles[TOTAL_PERFILES], const bool ocupada[TOTAL_PERFILES],
    int seleccion, const char* aviso, bool confirmandoBorrado,
    bool puedeVolver, float tiempo);
void dibujarPantallaNombre(const Font& fuente, const Paleta& pal, const char* nombre, const char* aviso, float tiempo);
void dibujarMenuPrincipal(const Font& fuente, const Paleta& pal, const char* jugador,
    const char* const* opciones, int total, int opcion, float tiempo);
void dibujarMenuDificultad(const Font& fuente, const Paleta& pal, int opcion, float tiempo, int record);
void dibujarPausa(const Font& fuente, const Paleta& pal, const char* aviso);