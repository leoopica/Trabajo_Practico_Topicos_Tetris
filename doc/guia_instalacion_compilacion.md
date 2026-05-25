# Guía de Instalación, Compilación y Uso - Tetris GBT

Este documento proporciona las instrucciones necesarias para que cualquier usuario, incluso sin experiencia previa, pueda configurar el entorno, compilar y ejecutar el juego de Tetris desarrollado para la cátedra de Tópicos de Programación.

---

## 1. Requisitos Previos

Para compilar este proyecto, necesitarás las siguientes herramientas instaladas en tu sistema (Windows):

1.  **Code::Blocks (con MinGW):** Se recomienda descargar la versión que incluye el compilador (ej. `codeblocks-20.03mingw-setup.exe`).
    *   [Descargar Code::Blocks](http://www.codeblocks.org/downloads/binaries/)
2.  **GBT (Graphics Basic Toolkit):**
    *   Descargar la librería desde: [GitLab GBT](https://gitlab.com/RodrigoMaranzana/libgbt-dist/-/archive/main/libgbt-dist-main.zip?ref_type=heads&path=release/GBT_v2026.1C.01)
    *   **Instrucciones:** Extraer el archivo descargado y copiar **todos** los archivos que se encuentran dentro de las carpetas de la librería (`gbt.h`, `libgbt.a`, `gbt.dll`, etc.) directamente en el **directorio raíz** de este proyecto.

---

## 2. Estructura del Proyecto y Bibliotecas

El juego utiliza la biblioteca **GBT (Graphics Basic Toolkit)** para el manejo de gráficos, entrada y temporizadores.

### Ubicación de archivos clave:
*   **Código fuente (`.c` y `.h`):** Se encuentran en la raíz del proyecto (`main.c`, `graficos.c`, `funcionalidades.c`, etc.).
*   **Biblioteca GBT (En la raíz del proyecto):**
    *   Headers: `gbt.h`, `gbt_entrada.h`, `gbt_graficos.h`, `gbt_logger.h`, `gbt_temporizador.h`, `gbt_vector.h`.
    *   Binario estático: `libgbt.a` (Este es el archivo que se debe "linkear").
    *   DLL de ejecución: `gbt.dll` (Debe estar en la misma carpeta que el ejecutable final).

---

## 3. Compilación con Code::Blocks (Paso a Paso)

1.  **Crear un nuevo proyecto:**
    *   `File` -> `New` -> `Project...` -> `Empty Project`.
    *   Dale un nombre (ej. `Tetris_GBT`) y selecciona la carpeta del proyecto.
2.  **Agregar archivos al proyecto:**
    *   Haz clic derecho sobre el proyecto en el panel izquierdo -> `Add files...`.
    *   Selecciona todos los archivos `.c` y `.h` de la carpeta raíz.
3.  **Configurar el Compilador (Search Directories):**
    *   Ve a `Project` -> `Build options...`.
    *   En la pestaña **Search directories**, sub-pestaña **Compiler**:
        *   Haz clic en `Add` y selecciona la carpeta raíz del proyecto (donde están los `.h` de GBT).
4.  **Configurar el Linker (Linker Settings):**
    *   En la misma ventana de `Build options...`, ve a la pestaña **Linker settings**.
    *   En **Link libraries**, haz clic en `Add` y busca el archivo:
        `libgbt.a` (ubicado en la raíz del proyecto).
    *   En **Other linker options**, añade: `-static-libgcc` (para asegurar compatibilidad).
5.  **Compilar:**
    *   Presiona `F9` (Build and Run).

---

## 4. Ejecución y Uso

### Archivos necesarios para jugar:
Para que el juego funcione, el archivo `gbt.dll` **debe** estar en la misma carpeta que el ejecutable (`.exe`). Code::Blocks suele generar el ejecutable en la carpeta `bin/Debug` o `bin/Release` dentro del proyecto. Debes copiar el `gbt.dll` de la raíz a esa carpeta si el programa no inicia.

### Controles básicos:
*   **Flechas Izquierda/Derecha:** Mover la pieza.
*   **Flecha Arriba:** Rotar la pieza.
*   **Flecha Abajo:** Caída suave (acelerar).
*   **P:** Pausar el juego.
*   **Enter:** Confirmar en menús / Iniciar juego.
*   **Q / Esc:** Salir del juego.

---

## 5. Solución de Problemas Comunes

*   **Error: "No se encuentra gbt.dll":** Copia el archivo `gbt.dll` de la raíz del proyecto a la carpeta donde se generó el `.exe` (usualmente `bin/Debug` o `bin/Release`).
*   **Error de Linkeo (Undefined reference to `gbt_...`):** Asegúrate de haber agregado `libgbt.a` correctamente en las opciones del Linker en Code::Blocks.
*   **Pantalla en negro:** Verifica que los archivos de recursos (si los hubiera) o los archivos `.dat` de configuración/partida tengan permisos de escritura.
