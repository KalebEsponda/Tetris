#include "save.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

static bool ranuraValida(int ranura) {
    return ranura >= 0 && ranura < TOTAL_PERFILES;
}

static void rutaDeRanura(int ranura, char* ruta, int tam) {
    snprintf(ruta, tam, "perfil_%d.sav", ranura + 1);
}

bool nombreValido(const char* nombre) {
    int n = (int)strlen(nombre);
    if (n < 1 || n > LARGO_NOMBRE) return false;
    for (int i = 0; i < n; i++) {
        char c = nombre[i];
        bool bien = (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        if (!bien) return false;
    }
    return true;
}

void nuevoPerfil(Perfil& pf, const char* nombre) {
    pf = Perfil{};
    snprintf(pf.nombre, sizeof(pf.nombre), "%s", nombre);
}

// ---------------------------------------------------------------- guardar
bool guardarPerfil(const Perfil& pf, int ranura) {
    if (!ranuraValida(ranura) || !nombreValido(pf.nombre)) return false;
    char ruta[64];
    rutaDeRanura(ranura, ruta, sizeof(ruta));

    std::ofstream out(ruta);
    if (!out) return false;

    out << "TETRIS 1\n";
    out << "NOMBRE " << pf.nombre << "\n";
    out << "RECORDS";
    for (int i = 0; i < TOTAL_DIFICULTADES; i++) out << " " << pf.mejorPuntaje[i];
    out << "\n";
    out << "PARTIDA " << (pf.hayPartida ? 1 : 0) << "\n";

    if (pf.hayPartida) {
        const Partida& p = pf.guardada;
        out << "DIFICULTAD " << p.dificultad << "\n";
        out << "NIVEL " << p.nivel << "\n";
        out << "LINEAS " << p.lineas << "\n";
        out << "LINEAS_NIVEL " << p.lineasNivel << "\n";
        out << "PUNTAJE " << p.puntaje << "\n";
        out << "ENERGIA " << p.energia << "\n";
        out << "PIEZA " << p.pieza.tipo << " " << p.pieza.rotacion << " "
            << p.pieza.fila << " " << p.pieza.col << "\n";
        out << "TABLERO\n";
        for (int f = 0; f < FILAS; f++) {
            for (int c = 0; c < COLUMNAS; c++) {
                out << p.tablero.celdas[f][c] << (c < COLUMNAS - 1 ? " " : "");
            }
            out << "\n";
        }
    }
    out << "FIN\n";
    return out.good();
}

bool guardarPartidaEnPerfil(Perfil& pf, int ranura, const Partida& partida) {
    Perfil copia = pf;
    copia.guardada = partida;
    copia.hayPartida = true;
    if (!guardarPerfil(copia, ranura)) return false;
    pf = copia;
    return true;
}

bool registrarResultado(Perfil& pf, int ranura, const Partida& partida) {
    if (partida.dificultad < 0 || partida.dificultad >= TOTAL_DIFICULTADES) return false;
    if (partida.puntaje <= pf.mejorPuntaje[partida.dificultad]) return true;   // no hay récord nuevo
    pf.mejorPuntaje[partida.dificultad] = partida.puntaje;
    return guardarPerfil(pf, ranura);
}

// ---------------------------------------------------------------- cargar
static bool leerEntero(std::ifstream& in, const char* clave, int& valor) {
    std::string k;
    return (in >> k >> valor) && k == clave;
}

// Lee la partida guardada y comprueba que todo tenga sentido (un archivo dañado no debe romper el juego)
static bool leerPartida(std::ifstream& in, Partida& destino) {
    Partida g = {};
    if (!leerEntero(in, "DIFICULTAD", g.dificultad) || g.dificultad < 0 || g.dificultad >= TOTAL_DIFICULTADES) return false;
    if (!leerEntero(in, "NIVEL", g.nivel) || g.nivel < 1 || g.nivel > TOTAL_NIVELES) return false;
    if (!leerEntero(in, "LINEAS", g.lineas) || g.lineas < 0) return false;
    if (!leerEntero(in, "LINEAS_NIVEL", g.lineasNivel) || g.lineasNivel < 0) return false;
    if (!leerEntero(in, "PUNTAJE", g.puntaje) || g.puntaje < 0) return false;
    if (!leerEntero(in, "ENERGIA", g.energia) || g.energia < 0 || g.energia > ENERGIA_MAXIMA) return false;

    std::string k;
    if (!(in >> k) || k != "PIEZA") return false;
    if (!(in >> g.pieza.tipo >> g.pieza.rotacion >> g.pieza.fila >> g.pieza.col)) return false;
    if (g.pieza.tipo < 1 || g.pieza.tipo > TOTAL_TIPOS) return false;
    if (g.pieza.rotacion < 0 || g.pieza.rotacion > 3) return false;
    if (g.pieza.fila < 0 || g.pieza.fila >= FILAS) return false;
    if (g.pieza.col < -3 || g.pieza.col >= COLUMNAS) return false;

    if (!(in >> k) || k != "TABLERO") return false;
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            if (!(in >> g.tablero.celdas[f][c])) return false;
            if (g.tablero.celdas[f][c] < 0 || g.tablero.celdas[f][c] > TOTAL_TIPOS) return false;
        }
    }

    g.cfg = configPorIndice(g.dificultad);
    if (hayColision(g.tablero, g.pieza)) return false;   // la pieza no puede estar encima de bloques
    destino = g;
    return true;
}

bool cargarPerfil(Perfil& pf, int ranura) {
    if (!ranuraValida(ranura)) return false;
    char ruta[64];
    rutaDeRanura(ranura, ruta, sizeof(ruta));

    std::ifstream in(ruta);
    if (!in) return false;

    Perfil leido = {};
    std::string clave, nombreLeido;
    int version = 0;
    if (!(in >> clave >> version) || clave != "TETRIS" || version != 1) return false;
    if (!(in >> clave >> nombreLeido) || clave != "NOMBRE" || !nombreValido(nombreLeido.c_str())) return false;
    if (!(in >> clave) || clave != "RECORDS") return false;
    for (int i = 0; i < TOTAL_DIFICULTADES; i++) {
        if (!(in >> leido.mejorPuntaje[i]) || leido.mejorPuntaje[i] < 0) return false;
    }
    snprintf(leido.nombre, sizeof(leido.nombre), "%s", nombreLeido.c_str());

    int hay = 0;
    if (!(in >> clave >> hay) || clave != "PARTIDA") return false;
    // Si solo la partida está dañada, se conservan el nombre y los récords
    leido.hayPartida = (hay == 1) && leerPartida(in, leido.guardada);

    pf = leido;
    return true;
}

bool borrarPerfil(int ranura) {
    if (!ranuraValida(ranura)) return false;
    char ruta[64];
    rutaDeRanura(ranura, ruta, sizeof(ruta));
    return remove(ruta) == 0;
}

// Lee las 3 ranuras. Una ranura vacía o dañada cuenta como libre.
int leerPerfiles(Perfil perfiles[TOTAL_PERFILES], bool ocupada[TOTAL_PERFILES]) {
    int cuantos = 0;
    for (int i = 0; i < TOTAL_PERFILES; i++) {
        ocupada[i] = cargarPerfil(perfiles[i], i);
        if (ocupada[i]) cuantos++;
        else perfiles[i] = Perfil{};
    }
    return cuantos;
}

bool nombreEnUso(const Perfil perfiles[TOTAL_PERFILES], const bool ocupada[TOTAL_PERFILES], const char* nombre) {
    for (int i = 0; i < TOTAL_PERFILES; i++) {
        if (ocupada[i] && strcmp(perfiles[i].nombre, nombre) == 0) return true;
    }
    return false;
}

// ---------------------------------------------------------------- última ranura usada
bool recordarUltimaRanura(int ranura) {
    if (!ranuraValida(ranura)) return false;
    std::ofstream out("ultima_ranura.txt");
    if (!out) return false;
    out << ranura << "\n";
    return out.good();
}

int leerUltimaRanura() {
    std::ifstream in("ultima_ranura.txt");
    int ranura = -1;
    if (!in || !(in >> ranura) || !ranuraValida(ranura)) return -1;
    return ranura;
}