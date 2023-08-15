#include <iostream>
#include <map>
#include <string>
#include <tchar.h>

// GLEW
#define GLEW_STATIC

#include <GL/glew.h>


#define FOURCC_DXT1 827611204
#define FOURCC_DXT3 861165636
#define FOURCC_DXT5 894720068

#define M_PI 3.14159265358979323846

// GLFW
#include <GLFW/glfw3.h>

#include "shader.h"
#include "stb_image.h"

#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <math.h>



#include <ft2build.h>
#include FT_FREETYPE_H




#include "ObjLoader.h"
#include "vboIndexer.h"
#include "hMoving.h"







// Function prototypes
void RenderText(shader &shader, std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void key_callback_w1(GLFWwindow* window, int key, int scancode, int action, int mode);
void win1_closeCallback(GLFWwindow* window);
GLFWwindow* window1;
GLFWwindow* window;
GLuint loadDDS(const char * imagepath);

GLchar sName[32] = {0};

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals; // Won't be used at the moment.
glm::vec3 camDir = glm::vec3(0.0, 2.0, 10.0);
glm::vec3 camPos = glm::vec3(30.0, 2.0, 0.0);
glm::vec3 camUp = glm::vec3(0.0, 1.0, 0.0);

std::vector<unsigned short> g_indices;
std::vector<unsigned short> g_indices2;

std::vector<glm::vec4> vInd;
GLint g_UV_el[512] = {0};
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
GLdouble wX = WIDTH/2, wY = HEIGHT/2;
GLfloat camX = 0.0f, camY = 0.0f, camZ = 0.0f;
GLfloat angleX = 0, angleY = 0, WX= -200.0, WY= 300.0f;
GLdouble nTime=0, dTime=0, lTime=0, t_wX=0, t_wY=0;
GLint frame = 0, loop = 1, actWin = 0;
int FR = 0;
GLfloat xOffset = 0.0f;

bool iKey[256];
GLfloat speed = 0.0f, s = 0.05f;

GLuint VBO[5], VAO[6], EBO[5], TBO[5];

struct Character {
    GLuint cTexID;
    glm::ivec2 cSize;
    glm::ivec2 cBearing;
    GLuint cAdvance;
};

struct Bullet {
    glm::vec2 position;
    GLfloat angle;
    GLint bStatus;
    GLfloat speed;
};

Bullet bullets[256];
GLint bCount = -1;
GLint shotDelay = 0;

std::map<wchar_t, Character> Characters;



// The MAIN function, from here we start the application and run the game loop
int main()
{

    GLdouble d=0.0f;
    GLfloat w1[240] = {0.0f};
    GLfloat wall1[240][3] = {0};
    GLfloat w1z = 60.0f;
    GLfloat w2[240] = {0.0f};
    GLfloat wall2[240][3] = {0};
    GLfloat temp = 0;
    for(int i = 0; i < 240; i++)
    {
        wall1[i][0] = 60.0f;
        wall1[i][1] = 0.0f;
        wall1[i][2] = w1z;
        w1z -= 0.5f;
        //std::cout<<wall1[i][0]<<" . "<<wall1[i][1]<<" . "<<wall1[i][2]<<std::endl;
    }
    w1z = 60.0f;
    for(int i = 0; i < 240; i++)
    {
        wall2[i][0] = w1z;
        wall2[i][1] = 0.0f;
        wall2[i][2] = 59.0f;
        w1z -= 0.5f;
        //std::cout<<wall1[i][0]<<" . "<<wall1[i][1]<<" . "<<wall1[i][2]<<std::endl;
    }


    // Init GLFW
    glfwInit();


    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    //glfwWindowHint(GLFW_DECORATED, NULL); // Remove the border and titlebar..
    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

    /*GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);




    glfwSetWindowMonitor(window, monitor, 0, 0, WIDTH, HEIGHT, mode->refreshRate);*/
    glfwMakeContextCurrent(window);



     // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    //glfwSetKeyCallback(window1, key_callback_w1);
    //glfwSetWindowCloseCallback(window1, win1_closeCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;

    if (FT_New_Face(ft, "consolab.ttf", 0, &face))
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;






    glEnable(GL_TEXTURE_2D);
   // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    glm::vec3 wCoord;
    GLfloat wSizeX = 40.0f, wSizeY = 2.0f;



    shader mShader("shader.vs", "shader.frag");
    shader txtShader("txtShader.vs", "txtShader.frag");
    shader colShader("colShader.vs", "colShader.frag");


    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat colPlane[] = {
        // Positions          // Colors           // Texture Coords
         -0.25f, -0.0f, -0.0f,
         0.25f, -0.0f, -0.0f,
         0.25f,  2.0f, -0.0f,
         0.25f,  2.0f, -0.0f,
        -0.25f,  2.0f, -0.0f,
        -0.25f, -0.0f, -0.0f,

    };

    //Стена
    GLfloat lineIns[]={
        0.0f + (wSizeX/2), 0.0f + wSizeY, 0.0f,
        0.0f + (wSizeX/2), 0.0f, wCoord.z,
        0.0f - (wSizeX/2), 0.0f, wCoord.z,
        0.0f + (wSizeX/2), 0.0f + wSizeY, 0.0f,
        0.0f - (wSizeX/2), 0.0f, wCoord.z,
        0.0f - (wSizeX/2), 0.0f+ wSizeY, 0.0f,

    };

    GLfloat tUV[] = {
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f

    };

    GLuint colPlaneInd[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };


    //координаты стены
    wCoord = glm::vec3(20.0f, 0.0f, 5.0f);

    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector< glm::vec3 > vertices2;
    std::vector< glm::vec2 > uvs2;
    std::vector< glm::vec3 > normals2; // Won't be used at the moment.
    std::vector<glm::vec3> indexed_vertices2;
    std::vector<glm::vec2> indexed_uvs2;
    std::vector<glm::vec3> indexed_normals2;
    glm::vec3 tempPos = {0.0f, 2.0f, 0.0f};
    glm::vec3 boxPos = {0.0f, 1.0f, 0.0f};


    glGenVertexArrays(5, VAO);
    glGenBuffers(5, VBO);
    glGenBuffers(5, EBO);
    glGenBuffers(5, TBO);





    bool res = ObjLoader::LoadObj("boxh.obj", vertices2, uvs2, normals2);
    if (!res) return 0;

    indexVBO_slow(vertices2, uvs2, normals2, g_indices, indexed_vertices2, indexed_uvs2, indexed_normals2);

     // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices2.size()*sizeof(glm::vec3), &indexed_vertices2[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_indices.size() * sizeof(unsigned short), &g_indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TBO[0]);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs2.size()*sizeof(glm::vec2), &indexed_uvs2[0], GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), tUV, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);





    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0);

    bool res2 = ObjLoader::LoadObj("box.obj", vertices, uvs, normals);
    if (!res2) return 0;

    indexVBO_slow(vertices, uvs, normals, g_indices2, indexed_vertices, indexed_uvs, indexed_normals);

     // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size()*sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_indices2.size() * sizeof(unsigned short), &g_indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TBO[1]);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size()*sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), tUV, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);




    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0);

    glBindVertexArray(VAO[3]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colPlane), colPlane, GL_STATIC_DRAW);

   /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colPlaneInd), colPlaneInd, GL_STATIC_DRAW);*/

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0);

    glBindVertexArray(VAO[5]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colPlane), colPlane, GL_STATIC_DRAW);

   /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colPlaneInd), colPlaneInd, GL_STATIC_DRAW);*/

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0);

    glBindVertexArray(VAO[4]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineIns), lineIns, GL_STATIC_DRAW);

   /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colPlaneInd), colPlaneInd, GL_STATIC_DRAW);*/

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO



    GLfloat zRot = 0.0f;
    GLfloat xCube = -2.0f;
    float zZ = 30.0f;
    float yY =2.0f;
    float xX = 0.0f;
    float fov = 45.0f;


    GLuint texture[4], texDDS;
    glGenTextures(4, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]); // все последующие GL_TEXTURE_2D-операции теперь будут влиять на данный текстурный объект

    // Установка параметров наложения текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // установка метода наложения текстуры GL_REPEAT (стандартный метод наложения)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Установка параметров фильтрации текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Загрузка изображения, создание текстуры и генерирование мипмап-уровней
    int width, height, nrChannels;
    unsigned char* data = stbi_load("texture.tga", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    glBindTexture(GL_TEXTURE_2D, texture[1]); // все последующие GL_TEXTURE_2D-операции теперь будут влиять на данный текстурный объект

    // Установка параметров наложения текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // установка метода наложения текстуры GL_REPEAT (стандартный метод наложения)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Установка параметров фильтрации текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Загрузка изображения, создание текстуры и генерирование мипмап-уровней
    data = stbi_load("t.tga", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    FT_Set_Pixel_Sizes(face, 0, 16);



    wchar_t x;
    wchar_t c;
    std::cout<<sizeof(c)<<std::endl;
    x = L'я';
    printf("%d ", x);

    for (c = 0; c < 256; c++)
    {

        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate texture


        /*for(int i = 0; i < face->glyph->bitmap.rows; i++)
        {
            for(int y = 0; y < face->glyph->bitmap.width; y++){
                x = face->glyph->bitmap.buffer[i*face->glyph->bitmap.width+y];
                if(x==0) putchar(' ');
                else if(c<128) putchar('+');
                else putchar('0');
            }
            putchar('\n');
        }*/

        GLuint tText;
        glGenTextures(1, &tText);
        glBindTexture(GL_TEXTURE_2D, tText);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            tText,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        Characters.insert(std::pair<int, Character>(c, character));

    }
    glBindTexture(GL_TEXTURE_2D, 0);

    for (c = 1000; c < 1128; c++)
    {

        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate texture


        /*for(int i = 0; i < face->glyph->bitmap.rows; i++)
        {
            for(int y = 0; y < face->glyph->bitmap.width; y++){
                x = face->glyph->bitmap.buffer[i*face->glyph->bitmap.width+y];
                if(x==0) putchar(' ');
                else if(c<128) putchar('+');
                else putchar('0');
            }
            putchar('\n');
        }*/

        GLuint tText;
        glGenTextures(1, &tText);
        glBindTexture(GL_TEXTURE_2D, tText);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            tText,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        Characters.insert(std::pair<int, Character>(c, character));

    }







    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    // Game loop
    GLdouble timer = glfwGetTime();
    double result = 0;
    bool moving = false;
    int direction = -1;
    int sec = 1;

    GLboolean intersect= false;
    glm::vec2 barry;
    GLfloat dist = 0.0f;
    GLfloat dot_product = 0.0f;
    int iSect = -1;
    bool collisionX, collisionZ, colisionLine;

        glm::vec2 center[3];
        glm::vec2 aabb_he[3];
        glm::vec2 aabb_center[3];
        glm::vec2 diff[3];
        glm::vec2 clmp[3];
        glm::vec2 closest[3];

    camPos = glm::vec3(10.0f,2.0f,10.0f);
    float cA = 0.0;
    glm::vec3 cross_prod;
    glm::vec2 Pb;
    glm::vec4 vTest, w_end, w_begin;
    GLfloat velocity = 0.0f, distAct, distPrev;
    bool hit[240] = {false};

    GLfloat equalizer = 0.0f;
    GLfloat eqalCo = 0.9f;
    glm::vec2 posPrev;
    GLfloat col_equalizer[240] = {0.0f};
    GLint boxHits = 0;
    GLfloat dHits[256] = {0.0f};
    GLfloat accuracy = 0.0f;
    GLfloat K = 1;
    GLint accDelay = 0;
    GLfloat rot = 0.0f;
    //while (!glfwWindowShouldClose(w;indow))
    while (loop!=0)
    {

    std::wstringstream strStatus;
    std::wstring sStatus;




       // speed = 0.0f;


        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions

        glfwPollEvents();



        nTime = glfwGetTime();
        dTime += ((nTime - lTime)*150);
        lTime = nTime;


        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
         //gluPerspective( 45.0f, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f );
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw the triangle







        /*camZ = 10.0f*sin(angleY)*sin(angleX);
        camX = 10.0f*cos(angleX)*sin(angleY);
        camY = 10.0f*cos(angleY);*/


        //camX = 10.0f*cos(angleX)*sin(angleY);
        //camY = 10.0f*cos(angleY);



        glm::mat4 modelview;
        glm::mat4 matRotate;
        glm::mat4 matTrans;
        glm::mat4 scalTrans;

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), float(WIDTH / HEIGHT), 0.1f, 1000.0f);
        glm::mat4 orthoProj = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT), 0.1f, 100.0f);
       // glm::mat4 rotation         = glm::rotate(glm::mat4(1.0f), glm::radians(zRot), glm::vec3(1.0f , 1.0f , 0.0f)) ;
        glm::mat4 view             = glm::lookAt(glm::vec3(camPos.x, 2.0, camPos.z), glm::vec3(camPos.x, 2.0, camPos.z) + (glm::normalize(camDir)), camUp);
        glm::mat4 modelTransform   = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) ;
        GLint texLoc;
        GLuint transformLoc;
        GLuint projLoc, cLoc;






        for (int i = 0; i < 240; i++){
        w1[i] = sqrt(pow((wall1[i][0] - camPos.x),2) + pow((wall1[i][2] - camPos.z), 2));
        }
        for (int i = 0; i < 240; i++){
        w2[i] = sqrt(pow((wall2[i][0] - camPos.x),2) + pow((wall2[i][2] - camPos.z), 2));
        }

        for(int s=240/2; s>0; s/=2){
        for(int i=0; i<240; i++){
            for(int j=i+s; j<240; j+=s){
                if(w1[i] > w1[j]){
                    GLfloat temp = w1[j];

                    w1[j] = w1[i];

                    w1[i] = temp;

                }
            }
        }
    }
    for(int s=240/2; s>0; s/=2){
        for(int i=0; i<240; i++){
            for(int j=i+s; j<240; j+=s){
                if(w2[i] > w2[j]){
                    GLfloat temp = w2[j];
                    w2[j] = w2[i];
                    w2[i] = temp;
                }
            }
        }
    }





        //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
       // GLint vertexColorLocation = glGetUniformLocation(mShader.program, "ourColor");





        mShader.use();


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[0]);

        texLoc = glGetUniformLocation(mShader.program, "ourTexture");
        glUniform1i(texLoc, 0);


        glBindVertexArray(VAO[0]);
        //not rotating cube
        transformLoc = glGetUniformLocation(mShader.program, "uModelTranslate");

        matRotate   =    glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f,1.0f,0.0f));
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) ;
        modelTransform = matTrans * matRotate;
        modelview        =  projectionMatrix * view * modelTransform ;
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &modelview[0][0]);
        glDrawElements(GL_TRIANGLES, g_indices.size(), GL_UNSIGNED_SHORT, 0);



        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        texLoc = glGetUniformLocation(mShader.program, "ourTexture");
        glUniform1i(texLoc, 0);
        glBindVertexArray(VAO[1]);
        //not rotating cube
        //transformLoc = glGetUniformLocation(mShader.program, "uModelTranslate");

        matRotate   =    glm::rotate(glm::mat4(1.0f), glm::radians(zRot), glm::vec3(0.0f,1.0f,0.0f));
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(vTest.x, vTest.y, vTest.z)) ;
        modelTransform = matTrans * matRotate;
        modelview        =  projectionMatrix * view * modelTransform ;
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &modelview[0][0]);

        glDrawElements(GL_TRIANGLES, g_indices2.size(), GL_UNSIGNED_SHORT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);





        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);


        colShader.use();

        GLuint colLoc = glGetUniformLocation(colShader.program, "uModelTranslate");
        cLoc = glGetUniformLocation(colShader.program, "cColor");




        glBindVertexArray(VAO[3]);
        for (int i = 0; i <= 239; i++){
        if(hit[i]) {glUniform4f(cLoc, std::abs(cos(col_equalizer[i])), col_equalizer[i] , 1.0f - col_equalizer[i], 0.8f);}
        else glUniform4f(cLoc, 0.1f, 0.5f, 0.1f, 0.8f);


        matRotate   =    glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f,1.0f,0.0f));
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(wall2[i][0], 0.0f, wall2[i][2])) ;
        modelTransform = matTrans * matRotate;
        modelview        =  projectionMatrix * view * modelTransform ;
        glUniformMatrix4fv(colLoc, 1, GL_FALSE, &modelview[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        for(int i = 0; i<255; i++){
        if(bullets[i].bStatus == 1){
        glBindVertexArray(VAO[3]);
        glUniform4f(cLoc, 1.0f, 0.01f, 0.5f, 1.0f);
        matRotate   =    glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) - glm::radians(bullets[i].angle), glm::vec3(0.0f,1.0f,0.0f));
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(bullets[i].position.x, 1.0f, bullets[i].position.y)) ;
        modelTransform = matTrans * matRotate;
        modelview        =  projectionMatrix * view * modelTransform ;
        glUniformMatrix4fv(colLoc, 1, GL_FALSE, &modelview[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        }
        }



        colLoc = glGetUniformLocation(colShader.program, "uModelTranslate");
        GLint vPos = glGetUniformLocation(colShader.program, "vPos");
        cLoc = glGetUniformLocation(colShader.program, "cColor");
        glBindVertexArray(VAO[4]);
        if(colisionLine) glUniform4f(cLoc, 1.0f, 0.0f, 0.0f, 0.8f);
        else glUniform4f(cLoc, rot, 1.0f, 0.0f, 1.0f);
        matRotate   =    glm::rotate(glm::mat4(1.0f), glm::radians(zRot), glm::vec3(0.0f,1.0f,0.0f));
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)) ;
        modelTransform = matTrans * matRotate;
        modelview        =  projectionMatrix * view * modelTransform ;
        glUniformMatrix4fv(colLoc, 1, GL_FALSE, &modelview[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        vTest = modelTransform * glm::vec4(10.0f, 1.0f, 0.0f, 1.0f);

        w_begin = modelTransform * glm::vec4(0.0f - (wSizeX/2), 0.0f, 0.0f, 1.0f); //Расчет начала координат стены после матричнного преобразования
        w_end = modelTransform * glm::vec4(0.0f + (wSizeX/2), 0.0f, 0.0f, 1.0f);   //Расчет конца координат стены после матричнного преобразования


        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);



        cLoc = glGetUniformLocation(colShader.program, "cColor");
        glUniform4f(cLoc, 0, 160, 227, 0.2);
        glBindVertexArray(VAO[3]);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(WIDTH/2, 0.0f, -2.0f)) ;
        scalTrans  = glm::scale(glm::mat4(1.0f), glm::vec3(WIDTH, 20.0f, 1.0f));
        modelTransform = matTrans * scalTrans;
        modelview        =  orthoProj * modelTransform ;
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &modelview[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        cross_prod = glm::cross(glm::vec3(diff[0].x, 2.0f, diff[0].y), glm::vec3(camPos.x, 2.0f, camPos.z));

        //d = sqrt(pow((wall1[0][0] - camPos.x),2) + pow((wall1[0][2] - camPos.z), 2));


        //нормализованный вектор стены
        GLubyte pix[3];
        glm::vec3 n_w = glm::normalize(w_end - w_begin);
        strStatus <<L"ИНФОРМАЦИЯ >> "<< shotDelay << "||" << K << "||" << boxHits;
        sStatus = strStatus.str();

        //txtShader.use();
        RenderText(txtShader, sStatus, 0.0f, 10.0f, 1.0f, glm::vec3(255, 0, 0));
        projLoc = glGetUniformLocation(txtShader.program, "projection");
        //not rotating cube
        //transformLoc = glGetUniformLocation(mShader.program, "uModelTranslate");
        matTrans   = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        modelview = orthoProj * matTrans;
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &modelview[0][0]);


        glReadPixels(WIDTH/2, HEIGHT/2, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);


      // Swap the screen buffers


        center[0] = glm::vec2(camPos.x, camPos.z) + 0.25f;
        aabb_he[0] = glm::vec2(2.0f, 120.0f);
        aabb_center[0] = glm::vec2(wall1[239][0] + aabb_he[0].x, wall1[239][2] + aabb_he[0].y);
        diff[0] = center[0] - aabb_center[0];
        clmp[0] = glm::clamp(diff[0], -aabb_he[0], aabb_he[0]);
        closest[0] = aabb_center[0] + clmp[0];
        diff[0] = closest[0] - center[0];
        if(glm::length(diff[0]) < 0.25f) collisionZ = true;
        else collisionZ = false;

        for (int i = 0; i <= 239; i++){
        center[1] = glm::vec2(camPos.x, camPos.z) + 0.5f;
        aabb_he[1] = glm::vec2(0.5f, 4.0f);
        aabb_center[1] = glm::vec2(wall2[i][0] + aabb_he[1].x, wall2[i][2] + aabb_he[1].y);
        diff[1] = center[1] - aabb_center[1];
        clmp[1] = glm::clamp(diff[1], -aabb_he[1], aabb_he[1]);
        closest[1] = aabb_center[1] + clmp[1];
        diff[1] = closest[1] - center[1];
        if(glm::length(diff[1]) < 0.5f) {collisionX = true; break;}
        else collisionX = false;
        }

        /*center[1] = glm::vec2(camPos.x, camPos.z) + 0.25f;
        aabb_he[1] = glm::vec2(120.0f, 2.0f);
        aabb_center[1] = glm::vec2(wall2[239][0] + aabb_he[1].x, wall2[239][2] + aabb_he[1].y);
        diff[1] = center[1] - aabb_center[1];
        clmp[1] = glm::clamp(diff[1], -aabb_he[1], aabb_he[1]);
        closest[1] = aabb_center[1] + clmp[1];
        diff[1] = closest[1] - center[1];
        if(glm::length(diff[1]) < 0.25f) collisionX = true;
        else collisionX = false;*/

        //Расчет расстояния от камеры до линии стены по-классике
        glm::vec2 v = glm::vec2(w_end.x, w_end.z) - glm::vec2(w_begin.x, w_begin.z);
        glm::vec2 w = glm::vec2(camPos.x, camPos.z) - glm::vec2(w_begin.x, w_begin.z);
        GLdouble c1  = glm::dot(w,v);
        GLdouble c2 = glm::dot(v,v);
        GLfloat b = c1/c2;
        Pb = glm::vec2(w_begin.x, w_begin.z) + b * v ;
        if(c1 <= 0) {
                //d = sqrt(pow((camPos.x - (w_end.x, w_end.z)),2) + pow((camPos.z - (wCoord.z)), 2));
                colisionLine = false;
        }

        else if(c2 <= c1) {
                //d = sqrt(pow((camPos.x - (w_end.x, w_end.z)),2) + pow((camPos.z - (wCoord.z)), 2));
                colisionLine = false;
        }
        else {
        d = sqrt(pow((camPos.x - Pb.x),2) + pow((camPos.z - Pb.y), 2));

        if(d <= 0.5) colisionLine = true; //Если расстояние от камеры до стены меньше радиуса, то столкновение
        else colisionLine = false;
        }

        accuracy = cos(glfwGetTime() * (K*2));


        if(dTime>=1.0){

                velocity = sqrt(pow((camPos.x - posPrev.x),2) + pow((camPos.z - posPrev.y), 2));
                posPrev = glm::vec2(camPos.x, camPos.z);

                zRot+= 1.0f;
                equalizer = cos(glm::radians(zRot))*eqalCo;
                eqalCo -= 0.0002f;





        boxPos.y = 1.0f;



        if(collisionZ) {

                //glm::vec2 nPos =  (glm::vec2(camPos.x, camPos.z) + 0.1f)*(speed/200);
           //speed *=(speed/2);
                camPos.x -= 0.25 - std::abs(diff[0].x);
        }

        if(collisionX) {
                /*glm::vec2 nPos =  ((glm::vec2(1.0f, 0.0f)*((glm::dot(glm::vec2(1.0f, 0.0f), -glm::vec2(camPos.x, camPos.z)))))+ glm::vec2(camPos.x, camPos.z))*(speed/500);
                camPos -= glm::vec3(nPos.x, 0.0f, nPos.y);
                speed *=0.05f;*/
                camPos.z -= 0.5f - std::abs(diff[1].y);

        }
        //Реакция на столкновение со стеной в зависимости от направления вектора нормали
        if(colisionLine) {
                    GLfloat interComp = (0.5 - std::abs(d)); //расстояние компенсации проникновения за границу

                    if(n_w.x < 0 && n_w.z > 0){
                    camPos.x -= interComp*std::abs(n_w.z);
                    camPos.z -= interComp*std::abs(n_w.x);
                    }
                    if(n_w.x > 0 && n_w.z > 0){
                    camPos.x -= interComp*std::abs(n_w.z);
                    camPos.z -= interComp*std::abs(n_w.x)*-1.0f;
                    }
                    if(n_w.x > 0 && n_w.z < 0){
                    camPos.x += interComp*std::abs(n_w.z);
                    camPos.z += interComp*std::abs(n_w.x);
                    }
                    if(n_w.x < 0 && n_w.z < 0){
                    camPos.x += interComp*std::abs(n_w.z);
                    camPos.z += interComp*std::abs(n_w.x)*-1.0f;;
                    }
                    if(n_w.x > 0 && n_w.z == 0){
                    //camPos.x -= interComp*std::abs(n_w.z);
                    camPos.z -= interComp*std::abs(n_w.x);
                    }









        }

        /*glm::vec2 v = glm::vec2(wCoord.x + (wSizeX/2), wCoord.z) - glm::vec2(wCoord.x-(wSizeX/2), wCoord.z);
        glm::vec2 w = glm::vec2(camPos.x, camPos.z) - glm::vec2(wCoord.x-(wSizeX/2), wCoord.z);
        GLdouble c1  = glm::dot(w,v);
        GLdouble c2 = glm::dot(v,v);
        GLfloat b = c1/c2;
        Pb = glm::vec2(wCoord.x-(wSizeX/2), wCoord.z) + b * v ;
        if(c1 <= 0) {
                d = sqrt(pow((camPos.x - (wCoord.x + (wSizeX/2))),2) + pow((camPos.z - (wCoord.z)), 2));
                colisionLine = false;
        }

        else if(c2 <= c1) {
                d = sqrt(pow((camPos.x - (wCoord.x + (wSizeX/2))),2) + pow((camPos.z - (wCoord.z)), 2));
                colisionLine = false;
        }
        else {
        d = sqrt(pow((camPos.x - Pb.x),2) + pow((camPos.z - Pb.y), 2));

        if(d <= 0.5) colisionLine = true;
        else colisionLine = false;
        }*/






        if(!(iKey['W']) || !(iKey['S']) || !(iKey['A']) || !(iKey['D'])) moving = false;

        firstPersonMovingCalc(window, camDir, angleX, WX, WY);




        if(iKey['W']) {

                if(direction==1) speed = -speed;
                firstPersonMoveForward(angleX, camPos, speed);
                moving = true;
                direction = 0;


                }
        if(iKey['S']) {
                if(direction==0) speed = -speed;
                firstPersonMoveBackward(angleX, camPos, speed);
                moving = true;
                direction = 1;
        }
        if(iKey['A']) {
                if(direction==3) speed = -speed;
                firstPersonStrafeLeft(camDir, camUp, camPos, speed);
                moving = true;
                direction = 2;
        }
        if(iKey['D']) {
                if(direction==2) speed = -speed;
                firstPersonStrafeRight(camDir, camUp, camPos, speed);
                moving = true;
                direction = 3;
        }

        if(shotDelay == 0){
        if(iKey[GLFW_KEY_SPACE]) {

                bCount++;
                if(bCount>255) bCount = 0;
                bullets[bCount].bStatus = 1;
                bullets[bCount].position = glm::vec2(camPos.x , camPos.z ) + glm::vec2(camDir.x, camDir.z) *2.0f;
                bullets[bCount].angle = angleX + accuracy;
                bullets[bCount].speed = 2.0f;
                shotDelay = 10;
                accDelay = 50;
                K++;
                if (K > 50.0f) K = 50.0f;
                }


        }
        shotDelay--;
        accDelay--;
        if(shotDelay <= 0) shotDelay = 0;
        if(accDelay <= 0) {accDelay = 0; K = 0;}

        for(int i=0; i<255; i++){
        if(bullets[i].bStatus == 1){
        bullets[i].position.y -= (sin(glm::radians(bullets[i].angle)))*bullets[i].speed;
        bullets[i].position.x -= (cos(glm::radians(bullets[i].angle)))*bullets[i].speed;
        dHits[i]  = sqrt(pow((vTest.x - bullets[i].position.x),2) + pow((vTest.z - bullets[i].position.y), 2));
            if(dHits[i] <= 1.0f) {bullets[i].bStatus = -1; boxHits++; rot += 0.05af;}
            if(rot >= 1.0f) rot = 0.0f;

        }
        }
                //if (collisionX && collisionZ) camPos = tempPos-speed;
        //if (w2[1] < 1.0f) camPos = tempPos - speed;





        if(moving) {

                speed += 0.05; if(speed >= 0.1) speed = 0.1;
                if(iKey['S'] && iKey['D'] || iKey['S'] && iKey['A'] || iKey['W'] && iKey['A'] || iKey['W'] && iKey['D'])  speed*=0.7;

        }

        /*if(colisionLine){
                    matRotate = glm::rotate(glm::mat4(1.0f), glm::radians(zRot/20.0f), glm::vec3(0.0f,1.0f,0.0f));
                    glm::vec4 temp, tempP ;

                    temp  = glm::vec4(camPos.x, camPos.y, camPos.z, 1.0f) * matRotate;
                    camDir += (glm::vec3(temp.x, 0.0f, temp.z));

                    }*/



        if(!moving) {
            if(direction==0){
            speed -=0.005;
            firstPersonMoveForward(angleX, camPos, speed);
            }
            if(direction==1){
            speed -=0.005;
            firstPersonMoveBackward(angleX, camPos, speed);}
            if(direction==2){
            speed -=0.005;
            camPos -= glm::normalize(glm::cross(camDir, camUp))*speed;
            }
            if(direction==3){
            speed -=0.005;
            camPos += glm::normalize(glm::cross(camDir, camUp))*speed;
            }
            if(direction==4){
            speed -=0.005;
            angleX+=(M_PI*-0.75)*10;
            firstPersonMoveBackward(angleX, camPos, speed);}

            if(speed<=0.0) {direction = -1; speed = 0;}
            }
        tempPos = camPos;

        for(int i = 0; i < 240; i++){
        if(intersect = glm::intersectRayTriangle(camPos, camDir, glm::vec3(wall2[i][0]+colPlane[0], wall2[i][1]+colPlane[1], wall2[i][2]+colPlane[2]),
                                              glm::vec3(wall2[i][0]+colPlane[3], wall2[i][1]+colPlane[4], wall2[i][2]+colPlane[5]),
                                                        glm::vec3(wall2[i][0]+colPlane[6], wall2[i][1]+colPlane[7], wall2[i][2]+colPlane[8]), barry, dist)) {iSect = i;
                                                        hit[i]=true;
                                                        col_equalizer[i] = equalizer*10.0f;
                                                        eqalCo = 0.9f;
                                                        if(col_equalizer[i]<0) col_equalizer[i] *= -1.0f;}
        if(intersect = glm::intersectRayTriangle(camPos, camDir, glm::vec3(wall2[i][0]+colPlane[9], wall2[i][1]+colPlane[10], wall2[i][2]+colPlane[11]),
                                              glm::vec3(wall2[i][0]+colPlane[12], wall2[i][1]+colPlane[13], wall2[i][2]+colPlane[14]),
                                                        glm::vec3(wall2[i][0]+colPlane[15], wall2[i][1]+colPlane[16], wall2[i][2]+colPlane[17]), barry, dist)){iSect = i;
                                                        hit[i]=true;
                                                        eqalCo = 0.9f;
                                                        col_equalizer[i] = equalizer*10.0f;
                                                        if(col_equalizer[i]<0) col_equalizer[i] *= -1.0f;}


        }

        /*collisionX = camPos.x + 1.0f >= -1.0f && 1.0f >= camPos.x - 1.0f;
        collisionZ = camPos.z + 1.0f >= -1.0f && 1.0f >= camPos.z - 1.0f;*/


        //if (d < 2.0f) camPos = tempPos + speed;








        /*glfwGetCursorPos(window, &wX, &wY);

        deltaX = wX - t_wX;
        deltaY = t_wY - wY;
        WX += deltaX*0.08;
        WY += deltaY*0.08;

        t_wX = WIDTH/2;
        t_wY = HEIGHT/2;



        if(WY<182) WY=182;
        if(WY>358) WY=358;



        camDir.z = 5*sin(glm::radians(WY))*sin(glm::radians(WX));
        camDir.x = 5*cos(glm::radians(WX))*sin(glm::radians(WY));
        camDir.y = 5*cos(glm::radians(WY));


                    if(iKey['W']) {camPos.z-= sin(glm::radians(WX))*speed; camPos.x -=cos(glm::radians(WX))*speed;}
                    if(iKey['S']) {camPos.z+= sin(glm::radians(WX))*speed; camPos.x +=cos(glm::radians(WX))*speed;}
                    if(iKey['A']) camPos-= glm::normalize(glm::cross(camDir, camUp))*speed;
                    if(iKey['D']) camPos+= glm::normalize(glm::cross(camDir, camUp))*speed;

        if (iKey[GLFW_KEY_UP]) fov += s;
        if (iKey[GLFW_KEY_DOWN]) fov -= s;

          if(speed!=0){
                       // cz+= sin(cAngle)*speed;
                        //cx+= cos(cAngle)*speed;
                        zZ += sin(angleY)*sin(angleX)*speed;
                        xX += cos(angleX)*sin(angleY)*speed;
                        printf("%f:%f\n", angleX, zZ);
                        }*/


                        //glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);





        dTime = 0;
        /*GLint sSec;
        if((nTime/sec)>=1) {sec++; sSec++;
        if(sSec >= 5){
            wall2[frame][2] = 59.0f;
            printf("DICK\n");
            frame++;
            sSec = 0;
            if(frame > 239) frame = 0;
        }

        }*/


        }
        //if((nTime)==1) frame = 0;

        glfwSwapBuffers(window);







    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(6, VAO);
    glDeleteBuffers(5, VBO);
    glDeleteBuffers(5, EBO);
    glDeleteBuffers(5, TBO);
    glDeleteTextures(4, texture);
    Characters.clear();
    std::cout<<w1[0]<<std::endl;

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        loop = 0;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }


        if(action == GLFW_PRESS) iKey[key] = true;
        if(action == GLFW_RELEASE) {iKey[key] = false;}
    //if (key == GLFW_KEY_W && action == GLFW_PRESS) xOffset += 0.01;
    //if (key == GLFW_KEY_S && action == GLFW_PRESS) xOffset -= 0.01;
}


void win1_closeCallback(GLFWwindow* window)
{

    glfwSetWindowShouldClose(window, GL_TRUE);
    glfwDestroyWindow(window);
}

void RenderText(shader &shader, std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader.use();
    GLuint tLoc;
    tLoc = glGetUniformLocation(shader.program, "textColor");
    glUniform3f(tLoc, color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO[2]);


    // Iterate through all characters
    std::wstring::const_iterator c;
    //for (c = text.begin(); c != text.end(); c++)
    for (c = text.begin(); c != text.end(); c++)
    {

        Character ch = Characters[*c];

        GLfloat xpos = x + ch.cBearing.x * scale;
        GLfloat ypos = y - (ch.cSize.y - ch.cBearing.y) * scale;

        GLfloat w = ch.cSize.x * scale;
        GLfloat h = ch.cSize.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.cTexID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.cAdvance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
