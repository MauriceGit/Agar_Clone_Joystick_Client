
#include <iostream>
#include <map>
#include "json/json.hpp"
#include "tinythread/tinythread.h"
#include "tinythread/fast_mutex.h"
#include "easywsclient/easywsclient.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "logic.h"
#include "mtXboxController.h"
#include "sphere.h"

using easywsclient::WebSocket;
using namespace tthread;
using namespace std;
using json = nlohmann::json;

/*
 * =====================================================================
 * GLOBAL STUFF FOR WEBSOCKET SECTION
 * =====================================================================
 */
class Color {
    public:
        float r,g,b;
        Color(float r, float g, float b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }
        ~Color(){}
        Color(Color & c) {
            r = c.r;
            g = c.g;
            b = c.b;
        }
};
class BotInfo {
    public:
        Color* color;
        string name;

        BotInfo(Color* color, string name){
            this->color = color;
            this->name = name;
        }
        ~BotInfo(){}
        BotInfo(BotInfo & bi){
            color = bi.color;
            name = bi.name;
        }

};
class Food {
    public:
        int posx, posy;
        float mass;

        Food(int posx, int posy, float mass) {
            this->posx = posx;
            this->posy = posy;
            this->mass = mass;
        }
        ~Food() {}
        Food( Food & f) {
            posx = f.posx;
            posy = f.posy;
            mass = f.mass;
        }
};
typedef Food Toxin;
typedef Food Blob;

class ViewWindow {
    public:
        int posx, posy;
        int sizex, sizey;

        ViewWindow(int posx, int posy, int sizex, int sizey) {
            this->posx = posx;
            this->posy = posy;
            this->sizex = sizex;
            this->sizey = sizey;
        }
        ~ViewWindow(){}
        ViewWindow(ViewWindow & v) {
            posx = v.posx;
            posy = v.posy;
            sizex = v.sizex;
            sizey = v.sizey;
        }
};

class Bot {
    public:
        map <int, Blob*> blobMap;

        Bot() {}
        ~Bot(){}
        Bot(Bot & b){
            blobMap = b.blobMap;
        }
};

char G_WSAddress[] = "ws://cagine.fh-wedel.de:8080/gui/";

map <int, Food*>    G_FoodMap;
map <int, Toxin*>   G_ToxinMap;
map <int, Bot*>     G_BotMap;
map <int, BotInfo*> G_BotInfoMap;

int G_GraphicsFinished = 0;
int G_WSFinished = 0;

fast_mutex G_FoodMutex;
fast_mutex G_ToxinMutex;
fast_mutex G_BotMutex;
fast_mutex G_BotInfoMutex;

/*
 * =====================================================================
 * GLOBAL STUFF FOR GRAPHICS SECTION
 * =====================================================================
 */

int G_JoystickInput = 1;
int G_JoystickWorking;
double G_Interval;
int G_Width;
int G_Height;
char* G_WindowTitle;
int G_FullScreen = 1;
GLFWwindow * G_Window = NULL;
GLuint G_ShaderColor;

double G_FieldSize = 1000.0;
GLfloat G_LightPos[] = {13,150,19};
GLfloat G_LightMass[] = {500};
GLfloat G_LightColor[] = {1,1,0};
GLfloat G_ToxinColor[] = {1,0,0};

Geometry G_Sphere;

/*
 * =====================================================================
 * WEBSOCKET STUFF
 * =====================================================================
 */

void handle_message(const std::string & message)
{

    if (!message.compare("alive_test")) {
        return;
    }

    json j = json::parse(message);

    map<string, json> foods = j["createdOrUpdatedFoods"];
    for (auto&& kv : foods) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        G_FoodMutex.lock();
        Food* food = G_FoodMap[key];
        if (!food) {
            food = new Food(posx, posy, mass);
        } else {
            food->posx = posx;
            food->posy = posy;
            food->mass = mass;
        }
        G_FoodMap[key] = food;
        G_FoodMutex.unlock();
    }

    for (int i : j["deletedFoods"]) {
        G_FoodMutex.lock();
        delete G_FoodMap[i];
        G_FoodMap.erase(i);
        G_FoodMutex.unlock();
    }
    map<string, json> toxins = j["createdOrUpdatedToxins"];
    for (auto&& kv : toxins) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        G_ToxinMutex.lock();
        Toxin* toxin = G_ToxinMap[key];
        if (!toxin) {
            toxin = new Toxin(posx, posy, mass);
        } else {
            toxin->posx = posx;
            toxin->posy = posy;
            toxin->mass = mass;
        }

        G_ToxinMap[key] = toxin;
        G_ToxinMutex.unlock();
    }

    for (int i : j["deletedToxins"]) {
        G_ToxinMutex.lock();
        delete G_ToxinMap[i];
        G_ToxinMap.erase(i);
        G_ToxinMutex.unlock();
    }

    map<string, json> bots = j["createdOrUpdatedBots"];
    for (auto&& kv : bots) {
        int botId = stoi(kv.first);
        Bot* bot = new Bot();

        std::map<string, json> blobs = (kv.second)["blobs"];
        for (auto&& kv2 : blobs) {
            int blobId = stoi(kv2.first);
            int posx =   (kv2.second)["pos"]["X"].get<int>();
            int posy =   (kv2.second)["pos"]["Y"].get<int>();
            float mass = (kv2.second)["mass"].get<float>();
            bot->blobMap[blobId] = new Blob(posx, posy, mass);
        }
        G_BotMutex.lock();
        G_BotMap[botId] = bot;
        G_BotMutex.unlock();
    }

    for (int i : j["deletedBots"]) {
        G_BotMutex.lock();
        Bot* bot = G_BotMap[i];
        if (bot) {
            map<int, Blob*> blobs = bot->blobMap;
            for (auto b : blobs) {
                delete b.second;
                blobs.erase(b.first);
            }
            delete bot;
            G_BotMap.erase(i);
        }
        G_BotMutex.unlock();
    }

    map<string, json> botInfos = j["createdOrUpdatedBotInfos"];
    for (auto&& kv : botInfos) {
        int key = stoi(kv.first);
        int r = (kv.second)["color"]["R"].get<int>();
        int g = (kv.second)["color"]["G"].get<int>();
        int b = (kv.second)["color"]["B"].get<int>();
        string name = (kv.second)["name"].get<string>();

        G_BotInfoMutex.lock();
        BotInfo* botInfo = G_BotInfoMap[key];
        if (!botInfo) {
            botInfo = new BotInfo(new Color(r, g, b), name);
        } else {
            botInfo->color->r = r;
            botInfo->color->g = g;
            botInfo->color->b = b;
            botInfo->name = name;
        }

        G_BotInfoMap[key] = botInfo;
        G_BotInfoMutex.unlock();
    }
    for (int i : j["deletedBotInfos"]) {
        G_BotInfoMutex.lock();
        BotInfo* bi = G_BotInfoMap[i];
        if (bi) {
            delete bi->color;
            delete bi;
            G_BotInfoMap.erase(i);
        }
        G_BotInfoMutex.unlock();
    }
}

void handleWSData(void * aArg) {

    WebSocket::pointer ws = WebSocket::from_url(G_WSAddress);
    if (!ws) {
        printf("The websocket could not be initialised. The websocket thread exits.\n");
        return;
    }
    assert(ws);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        if (G_GraphicsFinished) {
            break;
        }
        ws->poll();
        ws->dispatch(handle_message);
    }
    delete ws;
    G_WSFinished = 1;
}

/*
 * =====================================================================
 * GRAPHICS STUFF
 * =====================================================================
 */

/* HEADER for forward declarations */
void cbReshape (GLFWwindow* window, int w, int h);
int  createWindow(void);
void registerCallBacks (GLFWwindow * window);
void mainLoop (GLFWwindow * window);
void setProjection (GLdouble aspect);

void drawColoredSpheres() {
    glDisable(GL_CULL_FACE);
    glUseProgram(G_ShaderColor);
        GLfloat mp[16], mv[16];
        glGetFloatv(GL_PROJECTION_MATRIX, mp);
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "projMatrix"),  1, GL_FALSE, &mp[0]);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "viewMatrix"),  1, GL_FALSE, &mv[0]);

        GLfloat cam[] = {GLfloat(getCameraPosition(0)), GLfloat(getCameraPosition(1)), GLfloat(getCameraPosition(2))};

        glUniform3fv(glGetUniformLocation(G_ShaderColor, "cameraPos"), 1, cam);
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "light"), 1, G_LightPos);

        glBindVertexArray(G_Sphere.vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
        glBindVertexArray(0);

        GLfloat scale = 1.0;
        // To put the field from -500/500 and not 0/1000.
        GLfloat displacement = -G_FieldSize/2.0;

        GLfloat translation[] = {GLfloat(G_LightPos[0]), G_LightPos[1], GLfloat(G_LightPos[2])};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);

        glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, G_LightMass);

        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, G_LightColor);

        glUniform1i(glGetUniformLocation(G_ShaderColor, "isLight"), 1);
        glBindVertexArray(G_Sphere.vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
        glBindVertexArray(0);

        glUniform1i(glGetUniformLocation(G_ShaderColor, "isLight"), 0);

        // Toxin
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, G_ToxinColor);
        G_ToxinMutex.lock();
        for(auto toxinIt = G_ToxinMap.begin(); toxinIt != G_ToxinMap.end(); toxinIt++) {
            Toxin* toxin = toxinIt->second;
            if (toxin) {

                GLfloat translation[] = {GLfloat(displacement + toxin->posx * scale), 0, GLfloat(displacement + toxin->posy * scale)};
                glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);
                GLfloat mass[] = {toxin->mass};
                glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, mass);

                glBindVertexArray(G_Sphere.vertexArrayObject);
                glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
                glBindVertexArray(0);
            }
        }
        G_ToxinMutex.unlock();

        // Food
        GLfloat colorFood[] = {0, 1, 0};
        G_FoodMutex.lock();
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, colorFood);
        for(auto foodIt = G_FoodMap.begin(); foodIt != G_FoodMap.end(); foodIt++) {
            Food* food = foodIt->second;
            if (food) {

                GLfloat translation[] = {GLfloat(displacement + food->posx * scale), 0, GLfloat(displacement + food->posy * scale)};
                glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);
                GLfloat mass[] = {food->mass};
                glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, mass);

                glBindVertexArray(G_Sphere.vertexArrayObject);
                glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
                glBindVertexArray(0);
            }
        }
        G_FoodMutex.unlock();

        // Blobs
        G_BotMutex.lock();
        for (auto botIt = G_BotMap.begin(); botIt != G_BotMap.end(); botIt++) {
            Bot* bot = botIt->second;
            if (bot) {

                G_BotInfoMutex.lock();
                BotInfo* botInfo = G_BotInfoMap[botIt->first];
                if (botInfo) {

                    GLfloat r2 = GLfloat(botInfo->color->r/255.0);
                    GLfloat g2 = GLfloat(botInfo->color->g/255.0);
                    GLfloat b2 = GLfloat(botInfo->color->b/255.0);

                    GLfloat colorBot[] = {r2, g2, b2};
                    glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, colorBot);
                    map<int, Blob*> blobMap = bot->blobMap;
                    for (auto const& blobIt : blobMap) {
                        Blob* blob = blobIt.second;
                        if (blob) {
                            GLfloat translation[] = {GLfloat(displacement + blob->posx * scale), 0, GLfloat(displacement + blob->posy * scale)};
                            glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);
                            GLfloat mass[] = {blob->mass};
                            glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, mass);

                            glBindVertexArray(G_Sphere.vertexArrayObject);
                            glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
                            glBindVertexArray(0);
                        }
                    }
                }
                G_BotInfoMutex.unlock();
            }
        }
        G_BotMutex.unlock();

    glUseProgram(0);
    glEnable(GL_CULL_FACE);
}

void cbDisplay (GLFWwindow * window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport (0, 0, G_Width, G_Height);
    setProjection ((double)G_Width/G_Height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (!G_JoystickInput) {
        gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
             0.0, 0.0, 0.0,
             0.0, 1.0, 0.0);
    } else {
        MTVec3D cam = mtGetJoyPosition();
        MTVec3D center = mtGetJoyCenter();
        MTVec3D up = mtGetJoyUp();
        gluLookAt (cam.x,    cam.y,    cam.z,
                   center.x, center.y, center.z,
                   up.x,     up.y,     up.z);
    }

    drawColoredSpheres();

    glfwSwapBuffers(window);

    glfwSwapInterval(0);
}

void toggleWireframeMode (void)
{
    static GLboolean wireframe = GL_FALSE;

    wireframe = !wireframe;

    if (wireframe)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void printHelp() {
    printf("\n\n");
    printf("Call:\n");
    printf("    ./agar_joystick [JS_SOURCE]\n");
    printf("Description:\n");
    printf("    This is a client for our programming challenge WS16. It is able to connect\n");
    printf("    to the websocket and imitates a browser interface. But renders the blobs\n");
    printf("    in 3D, allowing a better emerging.\n");
    printf("    Additionally, the view can be managed, using a Joystick (i.e. XBox Gamepad).\n");
    printf("Parameter:\n");
    printf("    JS_SOURCE:\n");
    printf("        A complete path to a the joystick usb stream.\n");
    printf("        As standart, the joystick is bound to: /dev/input/js0.\n");
    printf("        If this is not the case or the joystick, please provide the correct source.\n");
    printf("Key assignments:\n");
    printf("    'h' | 'H': Print this usage.\n");
    printf("    'i' | 'I': Toggle between the Joystick and Mouse for camera control.\n");
    printf("    'F1': Toggle wireframe-Mode.\n");
    printf("    'q' | 'Q' | 'Esc': Exit the program.\n");
    printf("\n\n");
}

void cbKeyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key)
        {
            case 'h':
            case 'H':
                printHelp();
                break;
            case 'i':
            case 'I':
                printf("joystick: %i\n", G_JoystickInput);
                G_JoystickInput = !G_JoystickInput;

                if (!G_JoystickWorking) {
                    G_JoystickInput = 0;
                }

                break;
            case 'q':
            case 'Q':
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_F1:
                toggleWireframeMode();
                break;
        }
    }
}

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
void cbMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {

        if (action == GLFW_RELEASE)
            setMouseState(NONE);
        else
            setMouseState(MOVE);

    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {

        if (action == GLFW_RELEASE)
            setMouseState(NONE);
        else
            setMouseState(ZOOM);
    }
}

static void cbMouseMotion (GLFWwindow* window, double x, double y)
{
    if (getMouseEvent() == MOVE)
        setCameraMovement(x,y);

    if (getMouseEvent() == ZOOM)
        setCameraZoom(x,y);

    setMouseCoord(x,y);
}

void setProjection (GLdouble aspect)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (90.0, aspect, 1.0, 100000.0 );
}

void cbReshape (GLFWwindow* window, int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  setProjection ((GLdouble) w / (GLdouble) h);
}

double cbTimer (int lastCallTime)
{
    G_Interval = glfwGetTime();
    glfwSetTime(0.0);

    return G_Interval;
}

int createWindow(void)
{
    if (G_Window)
        glfwDestroyWindow(G_Window);

    glfwDefaultWindowHints();

    if (G_FullScreen)
        G_Window = glfwCreateWindow(1920, 1080, G_WindowTitle, glfwGetPrimaryMonitor(), NULL);
    else
        G_Window = glfwCreateWindow(G_Width, G_Height, G_WindowTitle, NULL, NULL);

    if (G_Window) {
        glfwMakeContextCurrent(G_Window);
        glfwGetFramebufferSize(G_Window, &G_Width, &G_Height);
    } else {
        return 0;
    }

    return 1;
}

void registerCallBacks (GLFWwindow * window)
{

    /* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
    * Erzeugen oder Groessenaenderungen des Fensters) */
    glfwSetFramebufferSizeCallback (window, cbReshape);

    glfwSetKeyCallback (window, cbKeyboard);

    glfwSetCursorPosCallback (window, cbMouseMotion);

    glfwSetMouseButtonCallback (window, cbMouseButton);
}

void mainLoop (GLFWwindow * window)
{
    double lastCallTime = cbTimer(0.0);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    while (!glfwWindowShouldClose(window) && !G_WSFinished)
    {
        if (G_JoystickWorking) {
            mtCalcJoyMovement(lastCallTime);
        }
        cbDisplay (window);
        lastCallTime = cbTimer (lastCallTime);
        glfwPollEvents();
    }

}

int initScene (void)
{
    glEnable (GL_DEPTH_TEST);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
    glEnable (GL_NORMALIZE);
    glEnable (GL_LIGHTING);

    return 1;
}

void readFile (char * name, char ** buffer) {
    FILE *f = fopen(name, "rb");
    fseek(f, 0, SEEK_END);
    int pos = ftell(f);
    fseek(f, 0, SEEK_SET);

    (*buffer) = (char*)malloc(pos+1);
    if (!fread(*buffer, pos-1, 1, f)) {
        return;
    }
    (*buffer)[pos-1] = '\0';
    fclose(f);
}

GLuint loadShaders(char * vertexShader, char * fragmentShader){

    /* Create the shaders */
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    /* Compile Vertex Shader */
    printf("Compiling Vertex shader\n");
    GLchar * VertexSourcePointer;
    readFile(vertexShader, &VertexSourcePointer);
    glShaderSource(VertexShaderID, 1, (const GLchar**)&VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    /* Check Vertex Shader */
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * vertexShaderErrorMessage = (char*)calloc(InfoLogLength, sizeof(char));
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &(vertexShaderErrorMessage[0]));
    fprintf(stdout, "vertexShaderErrorMessage: %s\n", vertexShaderErrorMessage);

    /* Compile Fragment Shader */
    printf("Compiling Fragment shader\n");
    char * FragmentSourcePointer = NULL;
    readFile(fragmentShader, &FragmentSourcePointer);

    glShaderSource(FragmentShaderID, 1, (const GLchar**)&FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    /* Check Fragment Shader */
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * fragmentShaderErrorMessage = (char*)calloc(InfoLogLength, sizeof(char));
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &(fragmentShaderErrorMessage[0]));
    fprintf(stdout, "fragmentShaderErrorMessage: %s\n", fragmentShaderErrorMessage);

    /*  Link the program */
    GLuint ProgramID = glCreateProgram();

    glAttachShader(ProgramID, VertexShaderID);

    glAttachShader(ProgramID, FragmentShaderID);

    glLinkProgram(ProgramID);

    /* Check the program */
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * programErrorMessage = (char*)calloc(InfoLogLength, sizeof(char));
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &(programErrorMessage[0]));
    fprintf(stdout, "programErrorMessage: %s\n", programErrorMessage);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

int initAndStartIO (char* title, char* joystickSrc, int width, int height)
{
    G_Width = width;
    G_Height = height;
    G_WindowTitle = title;
    G_FullScreen = 1;

    if (!glfwInit())
        return 0;

    if (createWindow())
    {
        G_JoystickWorking = mtInitJoyControl(joystickSrc);
        G_JoystickInput = G_JoystickWorking;

        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
          /* Problem: glewInit failed, something is seriously wrong. */
          printf("Error: %s\n", glewGetErrorString(err));
          glfwDestroyWindow(G_Window);
          exit(1);
        }
        printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

        initScene();
        initGame();
        G_ShaderColor = loadShaders("colorVertexShader.vert", "colorFragmentShader.frag");

        registerCallBacks (G_Window);

        G_Sphere = createUnitSphere(5);

        printf ("--> Initialisation finished\n"); fflush(stdout);

        mainLoop (G_Window);

        G_JoystickWorking = 0;
        mtFinishJoyControl();


    } else {
        return 0;
    }

    glfwDestroyWindow(G_Window);

    return 1;
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void handleGraphics(void * aArg) {
    char * title = "lots and lots of blobs ... yeay ... :-)";
    srand (time (0));

    glfwSetErrorCallback(error_callback);

    char* joystickSrc = (char*)(aArg);

    if (!initAndStartIO (title, joystickSrc, 1920, 1080))
    {
        fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
        glfwTerminate();
        return;
    }

    glfwTerminate();

    G_GraphicsFinished = 1;
}

/*
 * =====================================================================
 * MAIN STUFF
 * =====================================================================
 */

int main(int argc, char* argv[])
{
    printHelp();

    G_FoodMutex    = fast_mutex();
    G_ToxinMutex   = fast_mutex();
    G_BotMutex     = fast_mutex();
    G_BotInfoMutex = fast_mutex();

    char* name = "/dev/input/js0";

    if (argc > 1) {
        name = argv[1];
    }

    // Two synchronised threads handling graphics and data input parallel so a smooth
    // animation can be accomplished.
    thread handleWS   (handleWSData, 0);
    thread handleGraphic (handleGraphics, name);

    handleWS.join();
    handleGraphic.join();

    return 0;
}
