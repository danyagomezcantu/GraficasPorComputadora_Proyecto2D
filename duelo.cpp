#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

// Tamaño fijo de ventana
const unsigned int WINDOW_WIDTH = 900;
const unsigned int WINDOW_HEIGHT = 900;

// Callback para ajustar el viewport al cambiar el tamaño de la ventana (aunque es fija en este caso)
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Función para compilar un shader dado su tipo y código fuente
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    // Verificación de compilación
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error al compilar shader: " << infoLog << std::endl;
    }
    return shader;
}

// Función para crear el programa de shader completo combinando vertex y fragment
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Verificar enlace
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error al enlazar el programa de shaders: " << infoLog << std::endl;
    }
    // Los shaders individuales se pueden borrar luego de enlazar
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

// Creación de geometría para un círculo unitario centrado en el origen (usando triángulo *fan*)
unsigned int createCircleVAO(int segments = 64) {
    // Calcula vértices del círculo (incluyendo el centro)
    int numVertices = segments + 2;
    float* vertices = new float[3 * numVertices];  // (x,y,z) para cada vértice
    vertices[0] = 0.0f; 
    vertices[1] = 0.0f; 
    vertices[2] = 0.0f;  // vértice central en el origen
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.1415926f * i / segments;
        float x = cos(angle);
        float y = sin(angle);
        // Índice de vértice i+1 (ya que 0 es el centro)
        vertices[(i+1)*3 + 0] = x;
        vertices[(i+1)*3 + 1] = y;
        vertices[(i+1)*3 + 2] = 0.0f;
    }
    // Generar VAO y VBO para el círculo
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    // Especificar atributo de posición (3 floats por vértice)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // No se requiere EBO porque dibujaremos con glDrawArrays (modo *fan*)
    glBindVertexArray(0);
    delete[] vertices;
    return VAO;
}

// Creación de geometría para un cuadrado unitario centrado en el origen (lado de longitud 1)
unsigned int createSquareVAO() {
    float vertices[] = {
        // Coordenadas de los 4 vértices (dos triángulos que forman el cuadrado)
        -0.5f, -0.5f, 0.0f,  // inferior izquierdo
         0.5f, -0.5f, 0.0f,  // inferior derecho
         0.5f,  0.5f, 0.0f,  // superior derecho
        -0.5f,  0.5f, 0.0f   // superior izquierdo
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };  // define dos triángulos (0-1-2 y 0-2-3)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    // Cargar datos de vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Cargar índices de elementos (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Especificar atributo de posición (3 floats por vértice)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return VAO;
}

// Creación de geometría para un triángulo unitario (altura ~1, base ~1) centrado en el origen
unsigned int createTriangleVAO() {
    float vertices[] = {
        0.0f,  0.5f, 0.0f,  // vértice superior (punta del triángulo)
       -0.5f, -0.5f, 0.0f,  // vértice inferior izquierdo
        0.5f, -0.5f, 0.0f   // vértice inferior derecho
    };
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // Cargar datos de vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Especificar atributo de posición (3 floats por vértice)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return VAO;
}

// Creación de geometría para un rectángulo unitario (largo 1 en X, alto 1 en Y) con un extremo en el origen (0,0)
unsigned int createRectangleVAO() {
    // Definir un rectángulo de longitud 1 (eje X) y altura 1 (eje Y), con el extremo izquierdo en (0,0)
    // (Se usará para representar una espada; el origen (0,0) será el extremo donde se sostiene)
    float vertices[] = {
        // Triángulo 1 (mitad inferior de la hoja)
        0.0f, -0.5f, 0.0f,   // esquina inferior izquierda (en el origen)
        1.0f, -0.5f, 0.0f,   // esquina inferior derecha
        1.0f,  0.5f, 0.0f,   // esquina superior derecha
        // Triángulo 2 (mitad superior de la hoja)
        0.0f, -0.5f, 0.0f,   // esquina inferior izquierda (repetida)
        1.0f,  0.5f, 0.0f,   // esquina superior derecha (repetida)
        0.0f,  0.5f, 0.0f    // esquina superior izquierda
    };
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // Cargar datos de vértices del rectángulo
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Especificar atributo de posición (3 floats por vértice)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return VAO;
}

int main() {
    // Inicialización de la biblioteca GLFW
    if (!glfwInit()) {
        std::cerr << "No se pudo inicializar GLFW\n";
        return -1;
    }
    // Configuración de la ventana OpenGL (versión 3.3 core, no redimensionable)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Duelo Animacion 2D", NULL, NULL);
    if (!window) {
        std::cerr << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Cargar todas las funciones de OpenGL mediante GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "No se pudo inicializar GLAD\n";
        return -1;
    }
    // Configurar el viewport (coincide con el tamaño fijo de la ventana)
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Habilitar el *blending* para soportar transparencia (alfa)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Compilar los shaders para dibujar (posición transformada y color uniforme)
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 uMVP;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core
        uniform vec4 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = uColor;
        }
    )";
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);
    // Obtener ubicaciones de los *uniforms* en el shader
    int uMVP_loc = glGetUniformLocation(shaderProgram, "uMVP");
    int uColor_loc = glGetUniformLocation(shaderProgram, "uColor");
    // Crear las geometrías de las primitivas necesarias (VAOs)
    unsigned int circleVAO   = createCircleVAO();    // círculo unitario (para ring, círculo rojo, reflector)
    unsigned int squareVAO   = createSquareVAO();    // cuadrado unitario (para luchador cuadrado negro)
    unsigned int triangleVAO = createTriangleVAO();  // triángulo unitario (para espectadores/público)
    unsigned int rectVAO     = createRectangleVAO(); // rectángulo unitario (para espadas)
    // Definir colores RGBA utilizados en la escena
    glm::vec4 colorRing            = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);  // ring (gris oscuro)
    glm::vec4 colorBackgroundBright= glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);  // fondo con luces encendidas (gris claro)
    glm::vec4 colorBackgroundDark  = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);  // fondo con luces apagadas (casi negro)
    glm::vec4 colorRed             = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // luchador círculo rojo
    glm::vec4 colorBlack           = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // luchador cuadrado negro
    glm::vec4 colorAudience        = glm::vec4(0.4f, 0.4f, 0.8f, 1.0f);  // público (ejemplo: azul grisáceo)
    glm::vec4 colorSword           = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);  // espadas (blanco metálico)
    glm::vec4 colorSpotlight       = glm::vec4(1.0f, 1.0f, 0.8f, 0.5f);  // reflector (luz amarillenta semitransparente)
    // Matriz de proyección ortográfica para coordenadas en rango [-450,450] (origen en el centro de la ventana)
    glm::mat4 proj = glm::ortho(-450.0f, 450.0f, -450.0f, 450.0f, -1.0f, 1.0f);
    // Registrar el tiempo inicial
    double startTime = glfwGetTime();
    // Bucle principal de la animación
    while (!glfwWindowShouldClose(window)) {
        // Calcular tiempo (segundos) transcurrido desde el inicio
        double time = glfwGetTime() - startTime;
        // Determinar color de fondo según la fase de iluminación
        if (time < 6.0) {
            // Antes de 6s: estadio a oscuras (iluminación parcial inicial)
            // Interpolar de negro a gris oscuro durante los primeros 2 segundos
            float factor = 1.0f;
            if (time < 2.0) {
                factor = (float)(time / 2.0);
            }
            glm::vec4 bgColor = glm::mix(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), colorBackgroundDark, factor);
            glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        } else if (time < 7.0) {
            // Entre 6s y 7s: luces encendidas plenamente (fondo claro)
            glClearColor(colorBackgroundBright.r, colorBackgroundBright.g, colorBackgroundBright.b, colorBackgroundBright.a);
        } else if (time < 15.0) {
            // 7s a 15s: solo reflector (fondo oscuro nuevamente)
            glClearColor(colorBackgroundDark.r, colorBackgroundDark.g, colorBackgroundDark.b, colorBackgroundDark.a);
        } else {
            // 15s en adelante: luces encendidas en la escena final
            glClearColor(colorBackgroundBright.r, colorBackgroundBright.g, colorBackgroundBright.b, colorBackgroundBright.a);
        }
        // Limpiar el *color buffer* con el color de fondo determinado
        glClear(GL_COLOR_BUFFER_BIT);
        // Matriz de vista (identidad, no hay cámara en 2D)
        glm::mat4 view = glm::mat4(1.0f);
        // === Dibujar elementos de la escena según la fase de la animación ===

        // ** Público / Espectadores **
        if (time < 7.0 || time >= 15.0) {
            // Mostrar público antes de la pelea (t < 7s) y al final (t >= 15s)
            // Posiciones finales fijas de 4 espectadores alrededor del ring
            glm::vec2 audTargetPos[4] = {
                glm::vec2(-300.0f, -300.0f),  // esquina inferior izquierda del estadio
                glm::vec2( 300.0f, -300.0f),  // esquina inferior derecha
                glm::vec2(-300.0f,  300.0f),  // esquina superior izquierda
                glm::vec2( 300.0f,  300.0f)   // esquina superior derecha
            };
            // Posiciones iniciales (fuera de la vista, más allá de las esquinas de la ventana)
            glm::vec2 audStartPos[4] = {
                glm::vec2(-500.0f, -500.0f),
                glm::vec2( 500.0f, -500.0f),
                glm::vec2(-500.0f,  500.0f),
                glm::vec2( 500.0f,  500.0f)
            };
            // Dibujar cada espectador (representado como pequeño triángulo)
            for (int i = 0; i < 4; ++i) {
                glm::vec2 pos;
                if (time < 3.0) {
                    // De 0 a 3s: interpolar la posición desde el inicio hacia su lugar final
                    float tnorm = (float)(std::min(time, 3.0) / 3.0);
                    pos = audStartPos[i] + tnorm * (audTargetPos[i] - audStartPos[i]);
                } else {
                    // Luego de 3s: mantener posición final (ya llegaron)
                    pos = audTargetPos[i];
                }
                // Configurar transformación del espectador i
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
                model = glm::scale(model, glm::vec3(20.0f, 20.0f, 1.0f));  // reducir tamaño (triángulo pequeño)
                glm::mat4 mvp = proj * view * model;
                glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvp));
                glUniform4fv(uColor_loc, 1, glm::value_ptr(colorAudience));
                glBindVertexArray(triangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }

        // ** Ring (área de combate) **
        if (time < 7.0 || time >= 15.0) {
            // Dibujar el ring (un gran círculo gris) antes de la pelea y en la escena final
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(200.0f, 200.0f, 1.0f));  // radio ~200
            glm::mat4 mvp = proj * view * model;
            // Ajustar transparencia del ring al inicio para simular encendido gradual de luces (0 a 2s)
            float alpha = 1.0f;
            if (time < 2.0) {
                alpha = (float)(time / 2.0);
            }
            glm::vec4 ringColor = colorRing;
            ringColor.a = alpha;
            glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform4fv(uColor_loc, 1, glm::value_ptr(ringColor));
            glBindVertexArray(circleVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 64+2);  // dibujar círculo con triangle fan
        }

        // ** Luchadores (círculo rojo y cuadrado negro) **
        // Posiciones iniciales fuera del ring (antes de entrar)
        glm::vec2 redStart(-500.0f, -500.0f);
        glm::vec2 blackStart(500.0f, 500.0f);
        // Posiciones preparadas en el ring (frente a frente, a los lados opuestos del centro)
        glm::vec2 redReady(-150.0f, 0.0f);
        glm::vec2 blackReady(150.0f, 0.0f);
        glm::vec2 redPos;
        glm::vec2 blackPos;
        // Determinar posición actual de cada luchador según el tiempo
        if (time < 3.0) {
            // Antes de 3s: los luchadores aún no aparecen (se mantienen fuera de la ventana)
            redPos = redStart;
            blackPos = blackStart;
        } else if (time < 6.0) {
            // 3s a 6s: los luchadores se desplazan linealmente desde fuera de escena hasta sus posiciones en el ring
            float tnorm = (float)((std::min(time, 6.0) - 3.0) / (6.0 - 3.0));
            redPos   = redStart   + tnorm * (redReady   - redStart);
            blackPos = blackStart + tnorm * (blackReady - blackStart);
        } else if (time < 8.0) {
            // 6s a 8s: permanecen en sus posiciones iniciales en el ring (esperando el inicio del duelo)
            redPos = redReady;
            blackPos = blackReady;
        } else if (time < 12.0) {
            // 8s a 12s: durante la pelea se mueven girando alrededor del centro del ring
            float tnorm = (float)((time - 8.0) / (12.0 - 8.0));
            float angleDeg = 180.0f * tnorm;              // giran hasta 180 grados (media vuelta) en 4s
            float angleRad = glm::radians(angleDeg);
            float radius = 150.0f;                       // radio de la trayectoria circular desde el centro
            // Posicionar al rojo y al negro en lados opuestos del círculo de radio dado, separados 180°
            redPos.x   =  radius * cos(angleRad + 3.1416f);  // desplazamiento del rojo (inicia en PI radianes = 180°)
            redPos.y   =  radius * sin(angleRad + 3.1416f);
            blackPos.x =  radius * cos(angleRad);           // desplazamiento del negro (inicia en 0 radianes)
            blackPos.y =  radius * sin(angleRad);
        } else if (time < 14.0) {
            // 12s a 14s: fase final, los luchadores convergen al centro del ring
            // Posiciones en t=12 (final de la rotación): opuestos en extremos del diámetro horizontal
            float radius = 150.0f;
            float angleRad12 = glm::radians(180.0f);  // 180° en radianes
            glm::vec2 redPos12   ( radius * cos(angleRad12 + 3.1416f), radius * sin(angleRad12 + 3.1416f) );
            glm::vec2 blackPos12 ( radius * cos(angleRad12),            radius * sin(angleRad12) );
            // Interpolar ambas posiciones desde su valor en t=12 hacia el centro (0,0) en t=14
            float tnorm = (float)((time - 12.0) / (14.0 - 12.0));
            redPos   = redPos12   + tnorm * (glm::vec2(0.0f, 0.0f) - redPos12);
            blackPos = blackPos12 + tnorm * (glm::vec2(0.0f, 0.0f) - blackPos12);
        } else {
            // >=14s: ambos luchadores en el centro (el cuadrado negro encima del círculo rojo)
            redPos = glm::vec2(0.0f, 0.0f);
            blackPos = glm::vec2(0.0f, 0.0f);
        }

        // Dibujar luchador rojo (círculo)
        glm::mat4 modelRed = glm::mat4(1.0f);
        modelRed = glm::translate(modelRed, glm::vec3(redPos.x, redPos.y, 0.0f));
        // Escalar el círculo rojo (expansión al ser derrotado)
        float redScaleX = 30.0f;
        float redScaleY = 30.0f;
        if (time >= 13.0 && time < 15.0) {
            // De 13s a 15s: el círculo rojo se aplasta y expande (pierde forma por la derrota)
            float tnorm = (float)((time - 13.0) / (15.0 - 13.0));
            redScaleX = 30.0f + tnorm * (60.0f - 30.0f);  // ancho se duplica (30->60)
            redScaleY = 30.0f + tnorm * (20.0f - 30.0f);  // alto se reduce (30->20)
        } else if (time >= 15.0) {
            // Estado final (círculo completamente expandido/aplanado)
            redScaleX = 60.0f;
            redScaleY = 20.0f;
        }
        modelRed = glm::scale(modelRed, glm::vec3(redScaleX, redScaleY, 1.0f));
        glm::mat4 mvpRed = proj * view * modelRed;
        glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvpRed));
        glUniform4fv(uColor_loc, 1, glm::value_ptr(colorRed));
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 64+2);

        // Dibujar luchador negro (cuadrado)
        glm::mat4 modelBlack = glm::mat4(1.0f);
        // Añadir desplazamiento vertical para el "salto" del cuadrado negro entre 13s y 14s
        float jumpOffset = 0.0f;
        if (time >= 13.0 && time < 14.0) {
            float tnorm = (float)((time - 13.0) / (14.0 - 13.0));
            // Trayectoria de salto: seno para subir y bajar (sin(π * t) -> 0 a 1 a 0)
            jumpOffset = sin(3.1416f * tnorm) * 50.0f;
        }
        modelBlack = glm::translate(modelBlack, glm::vec3(blackPos.x, blackPos.y + jumpOffset, 0.0f));
        modelBlack = glm::scale(modelBlack, glm::vec3(60.0f, 60.0f, 1.0f));  // cuadrado de lado ~60
        glm::mat4 mvpBlack = proj * view * modelBlack;
        glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvpBlack));
        glUniform4fv(uColor_loc, 1, glm::value_ptr(colorBlack));
        glBindVertexArray(squareVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // ** Espadas **
        // Determinar si las espadas deben mostrarse (del 7s hasta sueltan)
        bool showRedSword   = (time >= 7.0 && time < 12.0);
        bool showBlackSword = (time >= 7.0 && time < 13.0);
        if (showRedSword) {
            // Calcular orientación de la espada del luchador rojo
            float baseAngleDeg = 60.0f;  // ángulo base (espada levantada diagonalmente)
            float swing = 0.0f;
            if (time < 10.0) {
                // 7s a 10s: oscilaciones lentas de poca amplitud
                swing = 15.0f * sin((float)time * 2.0f);
            } else if (time < 12.0) {
                // 10s a 12s: oscilaciones más rápidas y amplias
                swing = 30.0f * sin((float)time * 6.0f);
            }
            float angleDeg = baseAngleDeg + swing;
            float angleRad = glm::radians(angleDeg);
            // Transformación de la espada roja
            glm::mat4 modelSword = glm::mat4(1.0f);
            modelSword = glm::translate(modelSword, glm::vec3(redPos.x, redPos.y, 0.0f));
            modelSword = glm::rotate(modelSword, angleRad, glm::vec3(0.0f, 0.0f, 1.0f));
            // Escalar a tamaño real: ~80 unidades de largo x ~5 de ancho
            modelSword = glm::scale(modelSword, glm::vec3(80.0f, 5.0f, 1.0f));
            glm::mat4 mvpSword = proj * view * modelSword;
            glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvpSword));
            glUniform4fv(uColor_loc, 1, glm::value_ptr(colorSword));
            glBindVertexArray(rectVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        if (showBlackSword) {
            // Orientación de la espada del luchador negro
            float baseAngleDeg = 120.0f;  // ángulo base (diagonal opuesta)
            float swing = 0.0f;
            if (time < 10.0) {
                swing = -15.0f * sin((float)time * 2.2f);  // leve oscilación
            } else if (time < 12.0) {
                swing = -30.0f * sin((float)time * 6.0f);
            }
            float angleDeg = baseAngleDeg + swing;
            float angleRad = glm::radians(angleDeg);
            // Transformación de la espada negra
            glm::mat4 modelSword = glm::mat4(1.0f);
            modelSword = glm::translate(modelSword, glm::vec3(blackPos.x, blackPos.y, 0.0f));
            modelSword = glm::rotate(modelSword, angleRad, glm::vec3(0.0f, 0.0f, 1.0f));
            modelSword = glm::scale(modelSword, glm::vec3(80.0f, 5.0f, 1.0f));
            glm::mat4 mvpSword = proj * view * modelSword;
            glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvpSword));
            glUniform4fv(uColor_loc, 1, glm::value_ptr(colorSword));
            glBindVertexArray(rectVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // ** Reflector (spotlight) **
        if (time >= 7.0 && time < 15.0) {
            // Dibujar un círculo grande semitransparente simulando el haz de luz del reflector sobre el ring
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(250.0f, 250.0f, 1.0f));  // tamaño del círculo de luz
            glm::mat4 mvp = proj * view * model;
            glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform4fv(uColor_loc, 1, glm::value_ptr(colorSpotlight));
            glBindVertexArray(circleVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 64+2);
        }

        // Intercambiar los *buffers* (doble buffer) y procesar eventos de entrada
        glfwSwapBuffers(window);
        glfwPollEvents();
        // (Opcional) Salir automáticamente después de cierto tiempo para evitar bucle infinito en ejecución no interactiva
        if (time > 20.0) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    // Liberar recursos (VAOs y programa de shaders)
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteProgram(shaderProgram);
    // Terminar GLFW y cerrar ventana
    glfwTerminate();
    return 0;
}
