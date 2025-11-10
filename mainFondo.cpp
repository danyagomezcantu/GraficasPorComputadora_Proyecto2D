#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <shader_s.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <ctime>
#include <sys\timeb.h> 

#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//Función para crear un círculo en nuestro programa
void createCircleVertices(std::vector<float>& vertices, float radius, int segments) {
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * float(i) / float(segments);

        float x = radius * cos(angle);
        float y = radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f); // coordenada z para el círculo en el plano xy
    }
}


void createThickQuarterCircleVertices(std::vector<float>& vertices, float innerRadius, float outerRadius, int segments, float angulo) {
    float startAngle = 0.0f;
    float endAngle = glm::radians(angulo); // Un cuarto de círculo (90 grados)

    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + i * (endAngle - startAngle) / float(segments);

        // Vértices del borde interior
        float innerX = innerRadius * cos(angle);
        float innerY = innerRadius * sin(angle);
        float texInnerX = (innerX + 1.0f) / 2.0f;  // Coordenada de textura x
        float texInnerY = (innerY + 1.0f) / 2.0f;  // Coordenada de textura y

        // Vértices del borde exterior
        float outerX = outerRadius * cos(angle);
        float outerY = outerRadius * sin(angle);
        float texOuterX = (outerX + 1.0f) / 2.0f;  // Coordenada de textura x
        float texOuterY = (outerY + 1.0f) / 2.0f;  // Coordenada de textura y

        // Agregar vértices (borde interior)
        vertices.push_back(innerX);
        vertices.push_back(innerY);
        vertices.push_back(0.0f); // Coordenada z
        // Agregar coordenadas de textura para el vértice interior
        vertices.push_back(texInnerX);
        vertices.push_back(texInnerY);

        // Agregar vértices (borde exterior)
        vertices.push_back(outerX);
        vertices.push_back(outerY);
        vertices.push_back(0.0f); // Coordenada z
        // Agregar coordenadas de textura para el vértice exterior
        vertices.push_back(texOuterX);
        vertices.push_back(texOuterY);
    }
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation             width, height
    GLFWwindow* window = glfwCreateWindow(1600, 1600, "Texturas OpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    // build and compile our shader program
    Shader ourShader("5.1.transform.vs", "5.1.transform.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes

    //Rectangulo para la estrella del centro
    float verticesRectangulo0[] = {
        // positions          // texture coords
         0.2f,  0.1f, 0.0f,   1.0f, 1.0f, // top right
         0.2f, -0.1f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.2f, -0.1f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.2f,  0.1f, 0.0f,   0.0f, 1.0f  // top left 
    };

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco1;
    createThickQuarterCircleVertices(arco1, 0.89f, 1.0f, 100, 40.48f); 
    //                              Radio de 0.89 (hasta 1.0, para el grosor) y 40.48 grados

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco2;
    createThickQuarterCircleVertices(arco2, 1.0f, 1.2f, 100, 59.21f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco3;
    createThickQuarterCircleVertices(arco3, 0.85f, 0.89f, 100, 17.35f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco4;
    createThickQuarterCircleVertices(arco4, 1.02f, 1.26f, 100, 29.38f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco5;
    createThickQuarterCircleVertices(arco5, 1.0f, 1.09f, 100, 69.6f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco6;
    createThickQuarterCircleVertices(arco6, 1.11f, 1.34f, 100, 14.69f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco7;
    createThickQuarterCircleVertices(arco7, 0.93f, 1.1f, 100, 29.2f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco8;
    createThickQuarterCircleVertices(arco8, 0.93f, 1.03f, 100, 17.89f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco9;
    createThickQuarterCircleVertices(arco9, 1.1f, 1.35f, 100, 22.46f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco10;
    createThickQuarterCircleVertices(arco10, 0.84f, 0.98f, 100, 40.68f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco11;
    createThickQuarterCircleVertices(arco11, 0.94f, 1.03f, 100, 13.11f);

    //Variables para crear una semi-circunferencia en sentido opuesto a las manecillas del reloj
    std::vector<float> arco12;
    createThickQuarterCircleVertices(arco12, 1.02f, 1.025f, 100, 27.15f);

    float verticesTrianguloRojo[] = {
        // positions         
       -0.08f,  -0.12f, 0.0f,// bottom left
        0.16f,  -0.04f, 0.0f,//bottom right
       -0.04f,   0.12f, 0.0f,//top
    };

    float verticesTrianguloNaranja[] = {
        // positions         
        0.11f,  -0.16f, 0.0f,// bottom left
       -0.16f,   0.04f, 0.0f,//bottom right
         0.0f,   0.16f, 0.0f,//top
    };

    //Rectángulo
    float verticesRectangulo15[] = {
        // positions          // texture coords
         0.1f,  0.0375f, 0.0f,   1.0f, 1.0f, // top right
         0.1f, -0.0375f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.1f, -0.0375f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.1f,  0.0375f, 0.0f,   0.0f, 1.0f  // top left 
    };

    //Rectángulo
    float verticesRectangulo16[] = {
        // positions          // texture coords
         0.15f,  0.125f, 0.0f,   1.0f, 1.0f, // top right
         0.15f, -0.125f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.15f, -0.125f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.15f,  0.125f, 0.0f,   0.0f, 1.0f  // top left 
    };

    //Rectángulo
    float verticesRectangulo17[] = {
        // positions          // texture coords
         0.1f,  0.075f, 0.0f,   1.0f, 1.0f, // top right
         0.1f, -0.075f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.1f, -0.075f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.1f,  0.075f, 0.0f,   0.0f, 1.0f  // top left 
    };

    //Rectángulo
    float verticesRectangulo18[] = {
        // positions          // texture coords
         0.075f,  0.025f, 0.0f,   1.0f, 1.0f, // top right
         0.075f, -0.025f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.075f, -0.025f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.075f,  0.025f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int indicesTriangulo[] = {
        0, 1, 2, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO[19], VAO[19], EBO[19];
    glGenVertexArrays(19, VAO);
    glGenBuffers(19, VBO);
    glGenBuffers(19, EBO);

    //Agregamos verticesRectangulo0--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangulo0), verticesRectangulo0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 1------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[1]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, arco1.size() * sizeof(float), arco1.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 2------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[2]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, arco2.size() * sizeof(float), arco2.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 3------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[3]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, arco3.size() * sizeof(float), arco3.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 4------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[4]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, arco4.size() * sizeof(float), arco4.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 5------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[5]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, arco5.size() * sizeof(float), arco5.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 6------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[6]);  // Bind del VAO para la circunferencia
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, arco6.size() * sizeof(float), arco6.data(), GL_STATIC_DRAW);
    // Posición del atributo (posición 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Primeros 3 floats son las posiciones
    glEnableVertexAttribArray(0);
    // Posición del atributo (coordenada de textura, posición 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Los últimos 2 floats son las coordenadas de textura
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 7------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, arco7.size() * sizeof(float), arco7.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 8------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[8]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, arco8.size() * sizeof(float), arco8.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 9------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[9]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, arco9.size() * sizeof(float), arco9.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 10------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[10]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);
    glBufferData(GL_ARRAY_BUFFER, arco10.size() * sizeof(float), arco10.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 11------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[11]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
    glBufferData(GL_ARRAY_BUFFER, arco11.size() * sizeof(float), arco11.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos la semi-cirunferencia 12------------------------------------------------------------------------------------------
    glBindVertexArray(VAO[12]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
    glBufferData(GL_ARRAY_BUFFER, arco12.size() * sizeof(float), arco12.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos el triángulo rojo--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[13]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTrianguloRojo), verticesTrianguloRojo, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[13]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTriangulo), indicesTriangulo, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //Agregamos el triángulo naranja--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[14]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTrianguloNaranja), verticesTrianguloNaranja, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[14]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTriangulo), indicesTriangulo, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Agregamos verticesRectangulo15--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[15]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[15]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangulo15), verticesRectangulo15, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[15]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos verticesRectangulo16--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[16]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[16]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangulo16), verticesRectangulo16, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[16]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos verticesRectangulo17--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[17]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[17]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangulo17), verticesRectangulo17, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[17]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Agregamos verticesRectangulo18--------------------------------------------------------------------------------------
    glBindVertexArray(VAO[18]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[18]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangulo18), verticesRectangulo18, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[18]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // load and create a texture---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("C:/OCruz/Computer Science - ITAM/Gráficas 2/Tarea 2 - Imagen/lis.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture");
    }
    stbi_image_free(data);
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture", 0);

    int colorLocation = glGetUniformLocation(ourShader.ID, "ourColor");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // keyboard input
        processInput(window);
        // render
        glClearColor(0.843f, 0.8f, 0.69f, 1.0f);    //Color de fondo (crema)
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);


        // Primera figura - Rectángulo naranja---------------------------------------------------------------------------------
        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // Aplica la transformación
        // render container
        ourShader.use();
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.9125, 0.8875f, 0.0f));
        float rotationAngle = glm::radians(-55.0f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        unsigned int useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        // Rectángulo naranja
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //2da figura (Arco 1 - verde)--------------------------------------------------------------------------------------------------
        //Arriba
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //Movemos la figura a la posición correcta, una vez que ya la rotamos
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        //Rota el arco para que quede en su posición, alrededor del "ring"
        rotationAngle = glm::radians(81.87f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform)); // this time take the matrix value array's first element as its memory pointer value
        //Dibuja la semi-circunferencia en verde y sin transparencia
        glUniform4f(colorLocation, 0.106f, 0.208f, 0.18f, 1.0f);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2); // Dibujar la circunferencia

        //4ta figura (Arco 3 - naranja)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(72.65f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[3]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //5ta figura (Arco 4 - rojo)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(36.87f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.466667f, 0.043137f, 0.043137f, 1.0f);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //3era figura (Arco 2 - gris)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(59.28f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.898039f, 0.870588f, 0.847059f, 1.0f);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //6ta figura (Arco 5 - naranja claro)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(321.12f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.725490f, 0.486275f, 0.372549f, 1.0f);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //7ma figura (Arco 6 - naranja)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(309.47f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[6]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //8va figura (Arco 7 - gris)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(292.38f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.784314f, 0.760784f, 0.698039f, 1.0f);
        glBindVertexArray(VAO[7]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //9na figura (Arco 8 - rojo)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(228.3f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.650f, 0.133f, 0.106f, 1.0f);
        glBindVertexArray(VAO[8]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //10ma figura (Arco 9 - naranja)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(210.07f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[9]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //11va figura (Arco 10 - naranja claro)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(164.74f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.729412f, 0.313725f, 0.156863f, 1.0f);
        glBindVertexArray(VAO[10]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //12va figura (Arco 11 - gris)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(180.0f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.66f, 0.69f, 0.694f, 1.0f);
        glBindVertexArray(VAO[11]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);

        //13va figura (Arco 12 - naranja)--------------------------------------------------------------------------------------------------
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, -0.15f, 0.0f));
        rotationAngle = glm::radians(137.96f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[12]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (100 + 1) * 2);


        // 14va figura (Triángulo rojo)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // Aplica la transformación
        // render container
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.6875f, 0.4f, 0.0f));
        rotationAngle = glm::radians(-23.5f);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, -0.15f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        //Triángulo naranja
        glUniform4f(colorLocation, 0.650f, 0.133f, 0.106f, 1.0f);
        glBindVertexArray(VAO[13]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 15va figura (Triángulo naranja)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // Aplica la transformación
        // render container
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.5875f, 0.6125f, 0.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        //Triángulo naranja
        glUniform4f(colorLocation, 0.53f, 0.259f, 0.204f, 1.0f);
        glBindVertexArray(VAO[14]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 16va figura - Rectángulo 15 (verde)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.875f, 0.6625f, 0.0f));
        rotationAngle = glm::radians(-49.0f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        // Rectángulo naranja
        glUniform4f(colorLocation, 0.106f, 0.208f, 0.18f, 1.0f);
        glBindVertexArray(VAO[15]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 17va figura - Rectángulo 16 (verde)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.725f, 0.925f, 0.0f));
        rotationAngle = glm::radians(-56.0f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        // Rectángulo naranja
        glUniform4f(colorLocation, 0.106f, 0.208f, 0.18f, 1.0f);
        glBindVertexArray(VAO[16]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 18va figura - Rectángulo 17 (verde)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.625f, 0.975f, 0.0f));
        rotationAngle = glm::radians(-56.0f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        // Rectángulo naranja
        glUniform4f(colorLocation, 0.106f, 0.208f, 0.18f, 1.0f);
        glBindVertexArray(VAO[17]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 19va figura - Rectángulo 18 (crema)---------------------------------------------------------------------------------
        transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.use();
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        transform = glm::translate(transform, glm::vec3(-0.725f, 0.95f, 0.0f));
        rotationAngle = glm::radians(-56.0f); // esto hará que rote continuamente sobre su propio eje
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        // Desactivar la textura
        useTextureLoc = glGetUniformLocation(ourShader.ID, "useTexture");
        glUniform1i(useTextureLoc, GL_FALSE); // Desactivar textura
        // Rectángulo naranja
        glUniform4f(colorLocation, 0.890196f, 0.874510f, 0.729412f, 1.0f);
        glBindVertexArray(VAO[18]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();




    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(14, VAO);
    glDeleteBuffers(14, VBO);
    glDeleteBuffers(14, EBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
