# Tetris en C++ con Raylib

Proyecto de videojuego clásico **Tetris** desarrollado en **C++** utilizando la librería gráfica y de audio **Raylib 5.5**.

## Estructura del Proyecto
* `src/` - Código fuente modularizado (`main`, `game`, `board`, `piece`, `menu`, `audio`, `effects`, `style`, `save`).
* `assets/` - Efectos de sonido y recursos multimedia del juego.
* `CMakeLists.txt` - Configuración de compilación automatizada (descarga `raylib` automáticamente mediante `FetchContent` y copia la carpeta `assets` junto al ejecutable).

## Cómo compilar y ejecutar en Visual Studio
1. Clonar o descargar este repositorio.
2. Abrir **Visual Studio 2022** y seleccionar **Abrir una carpeta local** (o abrir directamente el `CMakeLists.txt`).
3. Esperar unos segundos a que CMake configure el proyecto y descargue Raylib automáticamente.
4. Seleccionar `Tetris.exe` en el botón verde de inicio superior y presionar **F5** (o hacer clic en Play).

## Autor
* **Kaleb Esponda**
