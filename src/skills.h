#pragma once

// Cada habilidad es un bit (1, 2, 4...). Así cada nivel puede combinarlas con "|":
// por ejemplo HAB_CAMBIAR | HAB_CONGELAR
const int HAB_CAMBIAR = 1;     // bit 0
const int HAB_CONGELAR = 2;    // bit 1
const int HAB_LIMPIAR = 4;     // bit 2
const int HAB_TODAS = 7;

const int TOTAL_HABILIDADES = 3;

const int ENERGIA_MAXIMA = 100;
const int ENERGIA_POR_LINEA = 12;       // energía que da cada línea borrada
const int FILAS_LIMPIAR = 3;            // filas que borra LIMPIAR (las de abajo)
const float DURACION_CONGELAR = 6.0f;   // segundos que dura CONGELAR
const float FACTOR_CONGELAR = 2.5f;     // la caída tarda 2.5 veces más mientras está congelado

struct Habilidad {
    const char* nombre;   // se muestra al usarla
    const char* tecla;    // letra que se muestra en pantalla
    int costo;            // energía que gasta
};

// El número de habilidad (0, 1, 2) coincide con su bit: 0 = CAMBIAR, 1 = CONGELAR, 2 = LIMPIAR
inline const Habilidad& datosHabilidad(int indice) {
    static const Habilidad HABILIDADES[TOTAL_HABILIDADES] = {
        { "CAMBIAR",  "Q", 25 },
        { "CONGELAR", "E", 50 },
        { "LIMPIAR",  "R", 80 }
    };
    return HABILIDADES[indice];
}