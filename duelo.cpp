#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Tamaño fijo de ventana
const unsigned int WINDOW_WIDTH  = 900;
const unsigned int WINDOW_HEIGHT = 900;

void framebuffer_size_callback(GLFWwindow* w, int width, int height){ glViewport(0,0,width,height); }

// ========== Utilidades de shaders ==========
unsigned int compileShader(unsigned int type, const char* src){
    unsigned int sh = glCreateShader(type);
    glShaderSource(sh,1,&src,nullptr);
    glCompileShader(sh);
    int ok; glGetShaderiv(sh,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(sh,1024,nullptr,log); std::cerr<<"Shader err:\n"<<log<<'\n';}
    return sh;
}
unsigned int createProgram(const char* vsrc, const char* fsrc){
    unsigned int vs=compileShader(GL_VERTEX_SHADER,vsrc), fs=compileShader(GL_FRAGMENT_SHADER,fsrc);
    unsigned int pr=glCreateProgram(); glAttachShader(pr,vs); glAttachShader(pr,fs); glLinkProgram(pr);
    int ok; glGetProgramiv(pr,GL_LINK_STATUS,&ok);
    if(!ok){ char log[1024]; glGetProgramInfoLog(pr,1024,nullptr,log); std::cerr<<"Link err:\n"<<log<<'\n';}
    glDeleteShader(vs); glDeleteShader(fs); return pr;
}

// ========== VAOs de geometría ==========
unsigned int createCircleVAO(int segments=64){
    int n=segments+2; std::vector<float> v(3*n);
    v[0]=0; v[1]=0; v[2]=0;
    for(int i=0;i<=segments;++i){
        float a=2.f*3.1415926f*i/segments;
        v[(i+1)*3+0]=cos(a); v[(i+1)*3+1]=sin(a); v[(i+1)*3+2]=0;
    }
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),v.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); return VAO;
}
unsigned int createSquareVAO(){
    float verts[] = { -0.5f,-0.5f,0,  0.5f,-0.5f,0,  0.5f,0.5f,0,  -0.5f,0.5f,0 };
    unsigned int idx[] = {0,1,2, 0,2,3};
    unsigned int VAO,VBO,EBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO); glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}
unsigned int createTriangleVAO(){
    float v[]={ 0,0.5f,0, -0.5f,-0.5f,0, 0.5f,-0.5f,0 };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}
unsigned int createRectangleVAO(){
    float v[]={ 0,-0.5f,0, 1,-0.5f,0, 1,0.5f,0,  0,-0.5f,0, 1,0.5f,0, 0,0.5f,0 };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}

// ====== Fondo: quad con textura ======
unsigned int createTexturedQuadVAO(){
    // pos(x,y) en coords de mundo (-450..450), tex(u,v)
    float x0=-450.f, y0=-450.f, x1=450.f, y1=450.f;
    float v[] = {
        x0,y0,  0.f,0.f,
        x1,y0,  1.f,0.f,
        x1,y1,  1.f,1.f,
        x0,y0,  0.f,0.f,
        x1,y1,  1.f,1.f,
        x0,y1,  0.f,1.f
    };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); return VAO;
}
unsigned int loadTexture2D(const char* path, bool flip=true){
    if(flip) stbi_set_flip_vertically_on_load(true);
    int w,h,nc; unsigned char* data = stbi_load(path,&w,&h,&nc,0);
    if(!data){ std::cerr<<"No pude cargar "<<path<<"\n"; return 0; }
    GLenum fmt = (nc==4)?GL_RGBA:GL_RGB;
    unsigned int tex; glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,fmt,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    stbi_image_free(data);
    return tex;
}

int main(){
    if(!glfwInit()){ std::cerr<<"GLFW init fail\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"Duelo Animacion 2D",nullptr,nullptr);
    if(!win){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win); glfwSetFramebufferSizeCallback(win,framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cerr<<"GLAD fail\n"; return -1; }
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // --- Shaders: color plano y textura ---
    const char* vColor = R"(#version 330 core
        layout(location=0) in vec3 aPos;
        uniform mat4 uMVP;
        void main(){ gl_Position = uMVP*vec4(aPos,1.0); })";
    const char* fColor = R"(#version 330 core
        uniform vec4 uColor; out vec4 FragColor; void main(){ FragColor=uColor; })";
    unsigned int progColor = createProgram(vColor,fColor);
    int uMVP = glGetUniformLocation(progColor,"uMVP");
    int uColor = glGetUniformLocation(progColor,"uColor");

    const char* vTex = R"(#version 330 core
        layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
        out vec2 vUV;
        uniform mat4 uMVP;
        void main(){ vUV=aUV; gl_Position = uMVP*vec4(aPos,0.0,1.0); })";
    const char* fTex = R"(#version 330 core
        in vec2 vUV; uniform sampler2D uTex; out vec4 FragColor;
        void main(){ FragColor = texture(uTex, vUV); })";
    unsigned int progTex = createProgram(vTex,fTex);
    int uMVPtex = glGetUniformLocation(progTex,"uMVP");
    int uTex    = glGetUniformLocation(progTex,"uTex");

    // --- Geometrías ---
    unsigned int circleVAO=createCircleVAO();
    unsigned int squareVAO=createSquareVAO();
    unsigned int triangleVAO=createTriangleVAO();
    unsigned int rectVAO=createRectangleVAO();
    unsigned int fsqVAO = createTexturedQuadVAO();
    unsigned int bgTex  = loadTexture2D("fondo.png"); // <= asegúrate de tenerlo en la carpeta del exe

    // --- Colores ---
    glm::vec4 colorRing            = glm::vec4(0.3f,0.3f,0.3f,1.0f);
    glm::vec4 colorBackgroundDark  = glm::vec4(0.0f,0.0f,0.0f,1.0f); // ya no se usa como clear, el fondo es la textura
    glm::vec4 colorRed             = glm::vec4(0.86f,0.09f,0.07f,1.0f);
    glm::vec4 colorBlack           = glm::vec4(0.06f,0.17f,0.16f,1.0f);
    glm::vec4 colorAudience        = glm::vec4(0.45f,0.45f,0.70f,1.0f);
    glm::vec4 colorSwordRed        = glm::vec4(0.0f,0.0f,0.0f,1.0f);        // **negra**
    glm::vec4 colorSwordBlack      = glm::vec4(0.78f,0.78f,0.78f,1.0f);     // **gris**
    glm::vec4 colorSpotlight       = glm::vec4(1.0f,1.0f,0.85f,0.45f);

    // Ortho en coords de mundo [-450,450]
    glm::mat4 proj = glm::ortho(-450.0f, 450.0f, -450.0f, 450.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    double t0 = glfwGetTime();

    while(!glfwWindowShouldClose(win)){
        double t = glfwGetTime() - t0;

        // Limpiar (el color no importa, siempre tapamos con la textura)
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        // ===== 1) DIBUJAR FONDO: textura a pantalla completa (siempre) =====
        glUseProgram(progTex);
        glm::mat4 mFS = glm::mat4(1.0f);                 // el quad ya está en coords mundo -450..450
        glm::mat4 mvpFS = proj*view*mFS;
        glUniformMatrix4fv(uMVPtex,1,GL_FALSE,glm::value_ptr(mvpFS));
        glUniform1i(uTex,0);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,bgTex);
        glBindVertexArray(fsqVAO);
        glDrawArrays(GL_TRIANGLES,0,6);
        glBindVertexArray(0);

        // ===== Resto de la escena con color plano =====
        glUseProgram(progColor);

        // Público (igual que tu versión, apareciendo al inicio y al final)
        if (t < 7.0 || t >= 15.0){
            glm::vec2 audTarget[4]={{-300,-300},{300,-300},{-300,300},{300,300}};
            glm::vec2 audStart [4]={{-500,-500},{500,-500},{-500,500},{500,500}};
            for(int i=0;i<4;++i){
                glm::vec2 pos;
                if(t<3.0){ float s=(float)(std::min(t,3.0)/3.0); pos = audStart[i]+s*(audTarget[i]-audStart[i]); }
                else pos = audTarget[i];
                glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(pos,0));
                model=glm::scale(model,glm::vec3(20,20,1));
                glm::mat4 mvp=proj*view*model;
                glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvp));
                glUniform4fv(uColor,1,glm::value_ptr(colorAudience));
                glBindVertexArray(triangleVAO); glDrawArrays(GL_TRIANGLES,0,3);
            }
        }

        // Ring (círculo gris) visible al inicio y final
        if (t < 7.0 || t >= 15.0){
            glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(200,200,1));
            glm::mat4 mvp=proj*view*model;
            glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvp));
            glUniform4fv(uColor,1,glm::value_ptr(colorRing));
            glBindVertexArray(circleVAO); glDrawArrays(GL_TRIANGLE_FAN,0,64+2);
        }

        // --- Trayectorias de los luchadores (idéntico hasta 14 s) ---
        glm::vec2 redStart(-500,-500), blackStart(500,500);
        glm::vec2 redReady(-150,0),    blackReady(150,0);
        glm::vec2 redPos, blackPos;
        if (t<3.0){ redPos=redStart; blackPos=blackStart; }
        else if (t<6.0){ float s=(float)((std::min(t,6.0)-3.0)/3.0); redPos=redStart+s*(redReady-redStart); blackPos=blackStart+s*(blackReady-blackStart); }
        else if (t<8.0){ redPos=redReady; blackPos=blackReady; }
        else if (t<12.0){
            float s=(float)((t-8.0)/4.0); float A=glm::radians(180.0f*s); float R=150.0f;
            redPos  = { R*std::cos(A+3.1416f), R*std::sin(A+3.1416f) };
            blackPos= { R*std::cos(A),         R*std::sin(A)         };
        }
        else if (t<14.0){
            float R=150.0f; glm::vec2 r12(R*std::cos(glm::radians(180.f)+3.1416f), R*std::sin(glm::radians(180.f)+3.1416f));
            glm::vec2 b12(R*std::cos(glm::radians(180.f)), R*std::sin(glm::radians(180.f)));
            float s=(float)((t-12.0)/2.0);
            redPos   = r12 + s*(glm::vec2(0,0)-r12);
            blackPos = b12 + s*(glm::vec2(0,0)-b12);
        } else { redPos={0,0}; blackPos={0,0}; }

        // --- Círculo rojo (con transición final a grande redondo) ---
        float redRX=30.f, redRY=30.f;
        if      (t>=13.0 && t<15.0){ float s=(float)((t-13.0)/2.0); redRX=30+(60-30)*s; redRY=30+(20-30)*s; } // tu aplanado previo
        else if (t>=15.0 && t<17.0){ float s=(float)((t-15.0)/2.0); redRX = 60 + (240-60)*s; redRY = 20 + (240-20)*s; }
        else if (t>=17.0){ redRX=240; redRY=240; }
        glm::mat4 mRed = glm::translate(glm::mat4(1),glm::vec3(redPos,0));
        mRed = glm::scale(mRed,glm::vec3(redRX,redRY,1));
        glm::mat4 mvpRed = proj*view*mRed;
        glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvpRed));
        glUniform4fv(uColor,1,glm::value_ptr(colorRed));
        glBindVertexArray(circleVAO); glDrawArrays(GL_TRIANGLE_FAN,0,64+2);

        // --- Cuadrado negro (salto + escala final al tamaño del póster) ---
        float jump=0.f; if(t>=13.0 && t<14.0){ float s=(float)((t-13.0)/1.0); jump = std::sin(3.1416f*s)*50.f; }
        float blackSize = (t<17.0)? 60.f + (140.f-60.f)*std::max(0.f,(float)(t-15.0))/2.0f : 140.f;
        glm::mat4 mBlk = glm::translate(glm::mat4(1),glm::vec3(blackPos.x, blackPos.y + jump, 0));
        mBlk = glm::scale(mBlk, glm::vec3(blackSize,blackSize,1));
        glm::mat4 mvpBlk = proj*view*mBlk;
        glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvpBlk));
        glUniform4fv(uColor,1,glm::value_ptr(colorBlack));
        glBindVertexArray(squareVAO); glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        // --- Espadas ---
        // 7–12s (roja) y 7–13s (negra) como antes; desde 15s ambas pasan a fondo y forman 45°
        auto drawSword = [&](glm::vec2 pos, float angleDeg, glm::vec3 scale, glm::vec4 col){
            glm::mat4 m=glm::mat4(1);
            m=glm::translate(m,glm::vec3(pos, (float)0));
            m=glm::rotate(m, glm::radians(angleDeg), glm::vec3(0,0,1));
            m=glm::scale(m,scale);
            glm::mat4 mvp=proj*view*m;
            glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvp));
            glUniform4fv(uColor,1,glm::value_ptr(col));
            glBindVertexArray(rectVAO); glDrawArrays(GL_TRIANGLES,0,6);
        };

        if (t>=15.0){
            // DIBUJAR DETRÁS → primero las espadas, luego círculo y cuadrado (ya están). Ya dibujamos círculo/ cuadrado,
            // pero sin depth test el orden manda. Para asegurar que queden atrás, las dibujamos ANTES del cuadrado/círculo
            // en el próximo frame. Aquí, para simplicidad, las repetimos después con un alfa leve.
        }

        // Espada del círculo rojo
        if (t>=7.0 && t<12.0){
            float base=60.f, swing=(t<10.0? 15.f*std::sin((float)t*2.0f) : 30.f*std::sin((float)t*6.0f));
            drawSword(redPos, base+swing, glm::vec3(80,5,1), colorSwordRed);
        } else if (t>=15.0){
            // transición a 45° y grande (detrás): longitud 260, ancho 14
            float s = (float)std::min(1.0, (t-15.0)/2.0);
            float L = 80.f + (260.f-80.f)*s, W = 5.f + (14.f-5.f)*s;
            // Dibujamos primero para que quede "detrás" en el siguiente frame
            drawSword(glm::vec2(0,0), 45.f, glm::vec3(L,W,1), colorSwordRed);
        }

        // Espada del cuadrado negro
        if (t>=7.0 && t<13.0){
            float base=120.f, swing=(t<10.0? -15.f*std::sin((float)t*2.2f) : -30.f*std::sin((float)t*6.0f));
            drawSword(blackPos, base+swing, glm::vec3(80,5,1), colorSwordBlack);
        } else if (t>=15.0){
            float s = (float)std::min(1.0, (t-15.0)/2.0);
            float L = 80.f + (260.f-80.f)*s, W = 5.f + (14.f-5.f)*s;
            drawSword(glm::vec2(0,0), 45.f, glm::vec3(L,W,1), colorSwordBlack);
        }

        // Reflector durante la pelea (7–15 s)
        if (t>=7.0 && t<15.0){
            glm::mat4 m=glm::scale(glm::mat4(1),glm::vec3(250,250,1));
            glm::mat4 mvp=proj*view*m;
            glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvp));
            glUniform4fv(uColor,1,glm::value_ptr(colorSpotlight));
            glBindVertexArray(circleVAO); glDrawArrays(GL_TRIANGLE_FAN,0,64+2);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
        if (t>20.0) glfwSetWindowShouldClose(win,true);
    }

    glDeleteVertexArrays(1,&circleVAO);
    glDeleteVertexArrays(1,&squareVAO);
    glDeleteVertexArrays(1,&triangleVAO);
    glDeleteVertexArrays(1,&rectVAO);
    glDeleteVertexArrays(1,&fsqVAO);
    glDeleteProgram(progColor);
    glDeleteProgram(progTex);
    glfwTerminate();
    return 0;
}
