# "Duelo", Animación en OpenGL (C++)

***Instituto Tecnológico Autónomo de México (ITAM)***

*COM-14106-001 Gráficas por Computadora*

Este proyecto es una animación desarrollada para el curso de Gráficas por Computadora. La escena representa un duelo entre dos figuras geométricas (un círculo y un cuadrado) que entran a un estadio, se enfrentan y terminan con una escena final (propuesta en clase).

La animación se basa en un storyboard propio incluido en este repositorio, y fue implementada por tiempos usando transformaciones, interpolaciones suaves y orden de dibujo manual. El proyecto está programado en C++ con OpenGL.

## Funcionalidades principales

* Dibujado con OpenGL Core 3.3, VAOs y shaders custom.
* Transformaciones 2D animadas usando glm (traslación, escala, rotación).
* Fondo (imagen PNG) para simular un estadio.
* Audio integrado con la librería miniaudio para sincronizar música y animación.
* Aparición escalonada de elementos (público, luces, armas, personajes).
* Secuencia narrativa: entrada, duelo, clímax y cierre.

## Librerías utilizadas
* GLFW:	Creación de ventana y contexto OpenGL
* GLAD: Carga de funciones OpenGL
* GLM:	Transformaciones y matrices
* stb_image:	Carga de texturas PNG
* miniaudio:	Reproducción de audio en tiempo real

## Archivos
* duelo.cpp:	Código principal de animación y lógica de escena
* fondo.png:	Imagen de fondo del estadio
* musica.mp3:	Tema musical para la animación
* miniaudio.h:	Librería para audio (single-header)
* background/:	Archivos de referencia para el diseño del fondo (arcos y formas)

## Instrucciones de uso

1. Copiar miniaudio.h a la misma carpeta donde compilas tu código (junto con tus shaders).
2. Asegurar que fondo.png y musica.mp3 estén en la misma carpeta que duelo.cpp.
3. Compilar con soporte para OpenGL, GLFW, GLAD y GLM.
4. Ejecutar duelo.exe o correr el binario resultante.

## Música

La música utilizada es 灼熱の刃 ～ ディノバルド 三味線だけで弾いてみたら火傷したぜ
(Monster Hunter：Glavenus Battle Theme - Japanese Music Cover).

Disponible en: https://youtu.be/9bcMnCk6tuA?si=V-SVNKlfYvIn90Xh

Crédito al autor original. Solo se usa con fines académicos.
