#pragma once
#include "game.h"

const int LARGO_NOMBRE = 8;
const int TOTAL_PERFILES = 3;   // cuántos perfiles se pueden guardar

// Un perfil = un jugador: su nombre, sus récords y (si la hay) su partida guardada
struct Perfil {
    char nombre[LARGO_NOMBRE + 1];
    int mejorPuntaje[TOTAL_DIFICULTADES];   // récord en Fácil, Normal y Difícil
    bool hayPartida;                        // true si tiene una partida guardada
    Partida guardada;                       // la partida guardada (solo vale si hayPartida)
};

// Cada perfil vive en una "ranura" numerada de 0 a TOTAL_PERFILES - 1 (archivo perfil_1.sav, perfil_2.sav...)
bool nombreValido(const char* nombre);                      // 1 a 8 letras mayúsculas o números
void nuevoPerfil(Perfil& pf, const char* nombre);
bool guardarPerfil(const Perfil& pf, int ranura);
bool cargarPerfil(Perfil& pf, int ranura);                  // false si la ranura está vacía o dañada
bool borrarPerfil(int ranura);
int leerPerfiles(Perfil perfiles[TOTAL_PERFILES], bool ocupada[TOTAL_PERFILES]);   // devuelve cuántos hay
bool nombreEnUso(const Perfil perfiles[TOTAL_PERFILES], const bool ocupada[TOTAL_PERFILES], const char* nombre);

bool guardarPartidaEnPerfil(Perfil& pf, int ranura, const Partida& partida);
bool registrarResultado(Perfil& pf, int ranura, const Partida& partida);   // sube el récord si lo superó

bool recordarUltimaRanura(int ranura);
int leerUltimaRanura();                                     // -1 si no hay ninguna