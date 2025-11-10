#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

const unsigned int WINDOW_WIDTH  = 900;
const unsigned int WINDOW_HEIGHT = 900;

void framebuffer_size_callback(GLFWwindow* w, int width, int height){ glViewport(0,0,width,height); }

//---------------------- Utils de shaders ----------------------
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

//---------------------- Geometrías ------------------------
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
unsigned int createIsoTriangleVAO(){ // isósceles base=1, altura=1
    float v[]={  0.0f, 0.5f,0,  -0.5f,-0.5f,0,  0.5f,-0.5f,0 };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}
unsigned int createEquilateralVAO(){ // equilátero lado≈1
    float r = 0.57735f;
    float v[] = { 0.0f,  r, 0.0f,  -0.5f,-r, 0.0f,   0.5f,-r, 0.0f };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}
// rectángulo CENTRADO (para rotar sobre el centro)
unsigned int createCenteredRectVAO(){
    float v[]={ -0.5f,-0.5f,0,  0.5f,-0.5f,0,  0.5f,0.5f,0,  -0.5f,-0.5f,0,  0.5f,0.5f,0,  -0.5f,0.5f,0 };
    unsigned int VAO,VBO; glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0); return VAO;
}
// Quad texturizado del fondo (-450..450)
unsigned int createTexturedQuadVAO(){
    float x0=-450.f, y0=-450.f, x1=450.f, y1=450.f;
    float v[] = { x0,y0,0,0,  x1,y0,1,0,  x1,y1,1,1,  x0,y0,0,0,  x1,y1,1,1,  x0,y1,0,1 };
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
    stbi_image_free(data); return tex;
}

//---------------------- Main ------------------------
int main(){
    if(!glfwInit()){ std::cerr<<"GLFW init fail\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"Duelo Animacion 2D",nullptr,nullptr);
    if(!win){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win); glfwSetFramebufferSizeCallback(win,framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cerr<<"GLAD fail\n"; return -1; }
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // <<< usamos orden de pintado, no Z

    // --- Shaders ---
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
        out vec2 vUV; uniform mat4 uMVP;
        void main(){ vUV=aUV; gl_Position = uMVP*vec4(aPos,0.0,1.0); })";
    const char* fTex = R"(#version 330 core
        in vec2 vUV; uniform sampler2D uTex; out vec4 FragColor;
        void main(){ FragColor = texture(uTex, vUV); })";
    unsigned int progTex = createProgram(vTex,fTex);
    int uMVPtex = glGetUniformLocation(progTex,"uMVP");
    int uTex    = glGetUniformLocation(progTex,"uTex");

    // --- VAOs / recursos ---
    unsigned int circleVAO = createCircleVAO();
    unsigned int squareVAO = createSquareVAO();
    unsigned int isoTriVAO = createIsoTriangleVAO();
    unsigned int equiTriVAO= createEquilateralVAO();
    unsigned int rectCVAO  = createCenteredRectVAO();
    unsigned int fsqVAO    = createTexturedQuadVAO();
    unsigned int bgTex     = loadTexture2D("fondo.png");

    // --- Colores ---
    glm::vec4 colorRed        = glm::vec4(0.86f,0.09f,0.07f,1.0f);
    glm::vec4 colorSquareGrey = glm::vec4(0.18f,0.19f,0.20f,1.0f);
    glm::vec4 colorSwordBlack = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    glm::vec4 colorSwordGrey  = glm::vec4(0.78f,0.78f,0.78f,1.0f);
    glm::vec4 colorSpotlight  = glm::vec4(1.0f,1.0f,0.85f,0.55f);

    glm::vec4 colTriBlack   = glm::vec4(0.02f,0.02f,0.02f,1.0f);
    glm::vec4 colTriOrange  = glm::vec4(0.55f,0.28f,0.07f,1.0f);
    glm::vec4 colTriRed     = glm::vec4(0.86f,0.09f,0.07f,1.0f);
    glm::vec4 colRectPink   = glm::vec4(0.95f,0.60f,0.62f,1.0f);
    glm::vec4 colRectBlack  = glm::vec4(0.06f,0.06f,0.06f,1.0f);

    // --- Matrices ---
    glm::mat4 proj = glm::ortho(-450.0f, 450.0f, -450.0f, 450.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    auto drawVAO = [&](unsigned int vao, const glm::mat4& model, const glm::vec4& color, int primitive=0){
        glUseProgram(progColor);
        glm::mat4 mvp = proj*view*model;
        glUniformMatrix4fv(uMVP,1,GL_FALSE,glm::value_ptr(mvp));
        glUniform4fv(uColor,1,glm::value_ptr(color));
        glBindVertexArray(vao);
        if(vao==circleVAO) glDrawArrays(GL_TRIANGLE_FAN,0,64+2);
        else if(vao==squareVAO) glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        else { int count = (primitive==6?6:3); glDrawArrays(GL_TRIANGLES,0,count); }
    };

    auto drawSword = [&](glm::vec2 pos, float angleDeg, float length, float thickness, const glm::vec4& col){
        glm::mat4 m=glm::mat4(1);
        m=glm::translate(m,glm::vec3(pos,0));
        m=glm::rotate(m, glm::radians(angleDeg), glm::vec3(0,0,1));
        m=glm::scale(m, glm::vec3(length, thickness, 1));
        drawVAO(rectCVAO, m, col, 6);
    };

    auto lerp2 = [](glm::vec2 a, glm::vec2 b, float s){ return a + s*(b-a); };
    auto ease  = [](float x){ return x<0?0:(x>1?1:(x*x*(3-2*x))); };

    // ----- Tiempos clave -----
    const double T_FADE        = 0.2;
    const double T_PAUSE       = 0.2;
    const double T_AUDI_END    = 1.8;
    const double T_HERO_IN     = T_AUDI_END + T_PAUSE;            // 2.0
    const double T_HERO_IN_END = T_HERO_IN + 3.0;                 // 5.0
    const double T_REFLECTOR   = T_HERO_IN_END + 0.2;             // 5.2
    const double T_SWORDS_ON   = T_REFLECTOR + 0.2;               // 5.4
    const double T_SWORD_SHOW  = 0.2;                             // 5.4–5.6
    const double T_FIGHT_MOVE  = T_SWORDS_ON + T_SWORD_SHOW;      // 5.6
    const double T_COLLISION   = 13.0;

    // --- Audio: motor y pista ---
    ma_engine audioEngine;
    if (ma_engine_init(NULL, &audioEngine) != MA_SUCCESS) {
        std::cerr << "No pude inicializar miniaudio\n";
    }

    ma_sound music;
    if (ma_sound_init_from_file(&audioEngine, "musica.mp3", 0, NULL, NULL, &music) != MA_SUCCESS) {
        std::cerr << "No pude cargar musica.mp3\n";
    } else {
        ma_sound_set_looping(&music, MA_FALSE);   // sólo reproducir una vez
        ma_sound_set_volume(&music, 0.10f);       // 10% al arrancar
        ma_sound_start(&music);                   // ¡Play!
    }

    double t0 = glfwGetTime(); 

    while(!glfwWindowShouldClose(win)){
        double t = glfwGetTime() - t0;
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        // ===== FADE-IN AUDIO (0–3s): 10% -> 80% con smoothstep =====
        {
            float sFade = (float)(t / 3.0);
            if (sFade < 0.0f) sFade = 0.0f;
            if (sFade > 1.0f) sFade = 1.0f;
            float smooth = sFade*sFade*(3.0f - 2.0f*sFade); // smoothstep
            float vol = 0.10f + 0.70f * smooth;             // 0.10 -> 0.80
            ma_sound_set_volume(&music, vol);
        }

        // ===== BACKGROUND =====
        glUseProgram(progTex);
        glm::mat4 mFS=glm::mat4(1.0f);
        glm::mat4 mvpFS=proj*view*mFS;
        glUniformMatrix4fv(uMVPtex,1,GL_FALSE,glm::value_ptr(mvpFS));
        glUniform1i(uTex,0);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,bgTex);
        glBindVertexArray(fsqVAO); glDrawArrays(GL_TRIANGLES,0,6);

        // ==================== PÚBLICO ====================
        glm::vec2 targetUL(-300, 300), targetBR(300,-300);
        struct Arrival { double t0, t1; };
        Arrival A_ul_orange{0.20, 1.40}, A_ul_red{0.50, 1.80}, A_ul_black{0.00, 1.20};
        glm::vec2 startUL(-500, 500);

        glm::vec2 UL_orange = targetUL + glm::vec2(  6,-10);
        glm::vec2 UL_red    = targetUL + glm::vec2(-20,-42);
        glm::vec2 UL_black  = UL_red   + glm::vec2(-16, 22);

        auto posTimed = [&](glm::vec2 s, glm::vec2 e, Arrival a, double tNow){
            if(tNow<=a.t0+T_FADE) return s;
            if(tNow>=a.t1+T_FADE) return e;
            float s01 = ease((float)((tNow - (a.t0+T_FADE))/(a.t1 - a.t0)));
            return lerp2(s,e,s01);
        };

        if (t >= T_FADE){
            glm::vec2 p_ul_or = posTimed(startUL, UL_orange, A_ul_orange, t);
            glm::vec2 p_ul_rd = posTimed(startUL, UL_red,    A_ul_red,    t);
            glm::vec2 p_ul_bk = posTimed(startUL, UL_black,  A_ul_black,  t);
            float angUL = glm::degrees(std::atan2(0.0f - targetUL.y, 0.0f - targetUL.x));

            drawVAO(isoTriVAO, glm::translate(glm::mat4(1),glm::vec3(p_ul_or,0)) *
                               glm::rotate(glm::mat4(1), glm::radians(angUL-90.f), glm::vec3(0,0,1)) *
                               glm::scale (glm::mat4(1), glm::vec3(40,90,1)), colTriOrange);
            drawVAO(equiTriVAO,glm::translate(glm::mat4(1),glm::vec3(p_ul_rd,0)) *
                               glm::rotate(glm::mat4(1), glm::radians(angUL-90.f), glm::vec3(0,0,1)) *
                               glm::scale (glm::mat4(1), glm::vec3(52,52,1)), colTriRed);
            drawVAO(isoTriVAO, glm::translate(glm::mat4(1),glm::vec3(p_ul_bk,0)) *
                               glm::rotate(glm::mat4(1), glm::radians(angUL-90.f), glm::vec3(0,0,1)) *
                               glm::scale (glm::mat4(1), glm::vec3(20,20,1)), colTriBlack);

            Arrival A_br_black{0.00,1.00}, A_br_pink{0.60,1.60};
            glm::vec2 startBR(500,-500);
            glm::vec2 BR_pink  = targetBR + glm::vec2(  0,-22);
            glm::vec2 BR_black = targetBR + glm::vec2(-30,-28);

            auto posTimedBR = [&](glm::vec2 s, glm::vec2 e, Arrival a){
                if(t<=a.t0+T_FADE) return s;
                if(t>=a.t1+T_FADE) return e;
                float s01 = ease((float)((t - (a.t0+T_FADE))/(a.t1 - a.t0)));
                return lerp2(s,e,s01);
            };
            glm::vec2 p_br_bk = posTimedBR(startBR, BR_black, A_br_black);
            glm::vec2 p_br_pk = posTimedBR(startBR, BR_pink,  A_br_pink );

            float angBR = glm::degrees(std::atan2(0.0f - targetBR.y, 0.0f - targetBR.x));
            drawVAO(rectCVAO, glm::translate(glm::mat4(1),glm::vec3(p_br_bk,0)) *
                              glm::rotate(glm::mat4(1), glm::radians(angBR), glm::vec3(0,0,1)) *
                              glm::scale (glm::mat4(1), glm::vec3(40,14,1)), colRectBlack, 6);
            drawVAO(rectCVAO, glm::translate(glm::mat4(1),glm::vec3(p_br_pk,0)) *
                              glm::rotate(glm::mat4(1), glm::radians(angBR), glm::vec3(0,0,1)) *
                              glm::scale (glm::mat4(1), glm::vec3(40,14,1)), colRectPink, 6);
        }

        // ==================== Luchadores ====================
        glm::vec2 redStart(-500,-500), blackStart(500,500);
        glm::vec2 redReady(-150,0),    blackReady(150,0);
        glm::vec2 redPos = redStart, blackPos = blackStart;

        if (t >= T_HERO_IN){
            if (t < T_HERO_IN_END){
                float s=(float)((t - T_HERO_IN)/3.0);
                redPos   = lerp2(redStart,   redReady,   s);
                blackPos = lerp2(blackStart, blackReady, s);
            } else if (t < 9.2){
                redPos = redReady; blackPos = blackReady;
            } else if (t < 11.2){
                float s=(float)((t-9.2)/2.0); float A=glm::radians(180.0f*s); float R=150.0f;
                redPos  = { R*std::cos(A+3.1416f), R*std::sin(A+3.1416f) };
                blackPos= { R*std::cos(A),         R*std::sin(A)         };
            } else if (t < T_COLLISION){
                float R=150.0f; glm::vec2 r12(R*std::cos(glm::radians(180.f)+3.1416f), R*std::sin(glm::radians(180.f)+3.1416f));
                glm::vec2 b12(R*std::cos(glm::radians(180.f)), R*std::sin(glm::radians(180.f)));
                float s=(float)((t-11.2)/(T_COLLISION-11.2));
                redPos   = lerp2(r12, {0,0}, s);
                blackPos = lerp2(b12, {0,0}, s);
            } else { redPos={0,0}; blackPos={0,0}; }
        }

        // ===== Protagonistas x2 antes de colisión =====
        float heroScale = (t < T_COLLISION ? 2.0f : 1.0f);

        // ===== CÍRCULO =====
        float redRX=30.f*heroScale, redRY=30.f*heroScale;
        if      (t>=13.0 && t<14.0){
            float s=(float)((t-13.0)/1.0);
            redRX = 60.f*heroScale + (90.f - 60.f*heroScale)*s;
            redRY = 60.f*heroScale + (40.f - 60.f*heroScale)*s;
        } else if (t>=14.0 && t<15.0){
            redRX = 60.f; redRY = 60.f;
        } else if (t>=15.0 && t<17.0){
            float s=(float)((t-15.0)/2.0);
            redRX = 60 + (240-60)*s; redRY = 60 + (240-60)*s;
        } else if (t>=17.0){ redRX=240; redRY=240; }
        glm::mat4 mRed = glm::translate(glm::mat4(1),glm::vec3(redPos,0));
        mRed = glm::scale(mRed,glm::vec3(redRX,redRY,1));

        // ===== CUADRADO =====
        float jump=0.f;
        if(t>=13.0 && t<14.0){ float s=(float)((t-13.0)/1.0); jump = std::sin(3.1416f*s)*50.f; }
        float baseSize = 60.f*heroScale;
        float sqSize = (t<17.0)? baseSize + (140.f-baseSize)*std::max(0.f,(float)(t-15.0))/2.0f : 140.f;
        glm::mat4 mBlk = glm::translate(glm::mat4(1),glm::vec3(blackPos.x, blackPos.y + jump, 0));
        mBlk = glm::scale(mBlk, glm::vec3(sqSize,sqSize,1));

        // ===== ESPADAS (offsets hacia el centro para que asomen) =====
        glm::vec2 swordOffsetRed   = glm::vec2(+32.0f, -6.0f);
        glm::vec2 swordOffsetBlack = glm::vec2(-70.0f, +12.0f); // empuja la empuñadura hacia el cuadrado
        float L_duel = 220.f, W_duel = 14.f;  
        bool swordsStatic = (t >= T_SWORDS_ON && t < T_SWORDS_ON + T_SWORD_SHOW);
        bool swordsMove   = (t >= T_FIGHT_MOVE && t < T_COLLISION);

        // ===== ORDEN FIJO (painter): espadas -> círculo -> cuadrado =====
        // 1) NEGRA (detrás del círculo)
        if (swordsStatic)  drawSword(redPos   + swordOffsetRed,   60.f,  L_duel, W_duel, colorSwordBlack);
        if (swordsMove) {
            float swingR = (t<10.0? 15.f*std::sin((float)t*2.0f) : 30.f*std::sin((float)t*6.0f));
            drawSword(redPos + swordOffsetRed, 60.f + swingR, L_duel, W_duel, colorSwordBlack);
        }
        // 2) GRIS (debe quedar detrás de AMBOS → se dibuja ANTES que círculo y cuadrado)
        if (swordsStatic)  drawSword(blackPos + swordOffsetBlack, 120.f, L_duel, W_duel, colorSwordGrey);
        if (swordsMove) {
            float swingB = (t<10.0? -15.f*std::sin((float)t*2.2f): -30.f*std::sin((float)t*6.2f));
            drawSword(blackPos + swordOffsetBlack, 120.f + swingB, L_duel, W_duel, colorSwordGrey);
        }
        // 3) CÍRCULO
        if (t < 15.0) drawVAO(circleVAO, mRed, colorRed);
        // 4) CUADRADO
        if (t < 15.0) drawVAO(squareVAO, mBlk, colorSquareGrey);

        // ===== FINAL (t ≥ 15): espadas debajo de todo y luego círculo/cuadrado =====
        if (t>=15.0){
            float s = std::min(1.0, (t-15.0)/2.0);
            float L = 100.f + (840.f - 100.f)*s;
            float W =   8.f + ( 54.f -   8.f)*s;
            // espadas primero (debajo de todo)
            drawSword(glm::vec2( 40, 40), 45.f, L, W, colorSwordBlack);
            drawSword(glm::vec2( 50, 60), 45.f, L, W, colorSwordGrey);
            // luego dueños
            drawVAO(circleVAO, mRed, colorRed);
            drawVAO(squareVAO, mBlk, colorSquareGrey);
        }

        // ===== SOMBRA — se apaga al colisionar =====
        if (t < T_COLLISION){
            float shadowAlpha = (t < T_FADE) ? (1.0f - (float)t/T_FADE * 0.5f) : 0.5f;
            drawVAO(rectCVAO, glm::scale(glm::mat4(1), glm::vec3(900,900,1)),
                    glm::vec4(0,0,0,shadowAlpha), 6);
        }

        // ===== REFLECTOR — hasta antes de la colisión =====
        if (t >= T_REFLECTOR && t < T_COLLISION){
            drawVAO(circleVAO, glm::scale(glm::mat4(1),glm::vec3(250,250,1)), colorSpotlight);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
        if (t>20.0) glfwSetWindowShouldClose(win,true);
    }

    // --- Limpieza de audio ---
    ma_sound_stop(&music);
    ma_sound_uninit(&music);
    ma_engine_uninit(&audioEngine);

    glfwTerminate();
    return 0;
}
