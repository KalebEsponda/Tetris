#include "audio.h"

void cargarSonidos(Sonidos& s) {
    s.mover = LoadSound("assets/mover.wav");
    s.rotar = LoadSound("assets/rotar.wav");
    s.caer = LoadSound("assets/caer.wav");
    s.linea = LoadSound("assets/linea.wav");
    s.tetris = LoadSound("assets/tetris.wav");
    s.gameover = LoadSound("assets/gameover.wav");
    s.nivel = LoadSound("assets/nivel.wav");
    s.habilidad = LoadSound("assets/habilidad.wav");
    s.error = LoadSound("assets/error.wav");

    s.musicas[0] = LoadMusicStream("assets/musica_facil.wav");
    s.musicas[1] = LoadMusicStream("assets/musica_normal.wav");
    s.musicas[2] = LoadMusicStream("assets/musica_dificil.wav");
    for (int i = 0; i < TOTAL_PISTAS; i++) {
        s.musicas[i].looping = true;
        SetMusicVolume(s.musicas[i], 0.35f);
    }
    s.pistaActual = 1;
    s.silencio = false;

    SetSoundVolume(s.mover, 0.5f);
    SetSoundVolume(s.rotar, 0.5f);
    SetSoundVolume(s.caer, 0.7f);
    SetSoundVolume(s.linea, 0.8f);
    SetSoundVolume(s.tetris, 0.9f);
    SetSoundVolume(s.gameover, 0.9f);
    SetSoundVolume(s.nivel, 0.9f);
    SetSoundVolume(s.habilidad, 0.9f);
    SetSoundVolume(s.error, 0.6f);
}

static void liberar(Sound& sonido) {
    if (IsSoundValid(sonido)) UnloadSound(sonido);
}

void liberarSonidos(Sonidos& s) {
    liberar(s.mover);
    liberar(s.rotar);
    liberar(s.caer);
    liberar(s.linea);
    liberar(s.tetris);
    liberar(s.gameover);
    liberar(s.nivel);
    liberar(s.habilidad);
    liberar(s.error);
    for (int i = 0; i < TOTAL_PISTAS; i++) {
        if (IsMusicValid(s.musicas[i])) UnloadMusicStream(s.musicas[i]);
    }
}

// Si el archivo no se cargó, simplemente no suena (el juego sigue funcionando)
void reproducir(const Sound& sonido) {
    if (IsSoundValid(sonido)) PlaySound(sonido);
}

// Solo se actualiza la pista que está sonando
void actualizarMusica(Sonidos& s) {
    if (IsMusicValid(s.musicas[s.pistaActual])) {
        UpdateMusicStream(s.musicas[s.pistaActual]);
    }
}

// Apaga todas las pistas y arranca la que corresponde a la dificultad
void iniciarMusica(Sonidos& s, int pista) {
    for (int i = 0; i < TOTAL_PISTAS; i++) {
        if (IsMusicValid(s.musicas[i])) StopMusicStream(s.musicas[i]);
    }
    if (pista < 0 || pista >= TOTAL_PISTAS) pista = 1;
    s.pistaActual = pista;
    if (IsMusicValid(s.musicas[pista])) PlayMusicStream(s.musicas[pista]);
}

void detenerMusica(Sonidos& s) {
    for (int i = 0; i < TOTAL_PISTAS; i++) {
        if (IsMusicValid(s.musicas[i])) StopMusicStream(s.musicas[i]);
    }
}

void pausarMusica(Sonidos& s) {
    if (IsMusicValid(s.musicas[s.pistaActual])) PauseMusicStream(s.musicas[s.pistaActual]);
}

void reanudarMusica(Sonidos& s) {
    if (IsMusicValid(s.musicas[s.pistaActual])) ResumeMusicStream(s.musicas[s.pistaActual]);
}

void alternarSilencio(Sonidos& s) {
    s.silencio = !s.silencio;
    SetMasterVolume(s.silencio ? 0.0f : 1.0f);
}