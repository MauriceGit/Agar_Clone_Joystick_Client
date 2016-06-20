
#include <iostream>
#include "json/json.hpp"
#include "tinythread/tinythread.h"
#include "tinythread/fast_mutex.h"
//#include <mutex.h>
#include "easywsclient/easywsclient.hpp"
//#include <unordered_map>
#include <map>

#include "client_ws/client_ws.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "logic.h"
#include "joystickCamera.h"
#include "sphere.h"

using easywsclient::WebSocket;
using namespace tthread;
using namespace std;
using json = nlohmann::json;

typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;

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

int G_wsCounter = 0;
int G_global_data = 0;
//char G_WSAddress[] = "ws://192.168.2.187:1234";
char G_WSAddress[] = "ws://127.0.0.1:8080/gui/";

map <int, Food*>    G_FoodMap;
map <int, Toxin*>   G_ToxinMap;
map <int, Bot*>     G_BotMap;
map <int, BotInfo*> G_BotInfoMap;

mutex G_m;

/*
 * =====================================================================
 * GLOBAL STUFF FOR GRAPHICS SECTION
 * =====================================================================
 */

int G_openGLCounter = 0;
double G_Interval;
int G_Width;
int G_Height;
char* G_WindowTitle;
int G_FullScreen = 1;
GLFWwindow * G_Window = NULL;
GLuint G_ShaderColor;
GLuint G_ObjectsBuffer;
GLfloat G_Objects[] = {
    -15.0, -15.0, -15.0,
    15.0, -15.0, -15.0,
    15.0, 15.0, -15.0,
    -15.0, -15.0, -15.0,
    15.0, 15.0, -15.0,
    -15.0, 15.0, -15.0
};
Geometry G_Sphere;
Geometry G_Cube;

/*
 * =====================================================================
 * WEBSOCKET STUFF
 * =====================================================================
 */

void deleteBot(int botId) {
    Bot* bot = G_BotMap[botId];
    if (bot) {
        map<int, Blob*> blobs = bot->blobMap;
        for (auto b : blobs) {
            delete b.second;
            //blobs.erase(b.first);
        }
        delete bot;
        //G_BotMap.erase(botId);
    }
}

void handle_message(const std::string & message)
{

    if (!message.compare("alive_test")) {
        return;
    }
    //printf("1\n"); fflush(stdout);

    json j = json::parse(message);
    //printf("2\n"); fflush(stdout);

    map<string, json> foods = j["createdOrUpdatedFoods"];
    for (auto&& kv : foods) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        Food* food = G_FoodMap[key];
        if (!food) {
            food = new Food(posx, posy, mass);
        } else {
            food->posx = posx;
            food->posy = posy;
            food->mass = mass;
        }
        G_FoodMap[key] = food;
    }
    //printf("3\n"); fflush(stdout);

    for (int i : j["deletedFoods"]) {
        delete G_FoodMap[i];
        G_FoodMap.erase(i);
    }
    //printf("4\n"); fflush(stdout);
    map<string, json> toxins = j["createdOrUpdatedToxins"];
    for (auto&& kv : toxins) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        Toxin* toxin = G_ToxinMap[key];
        if (!toxin) {
            toxin = new Toxin(posx, posy, mass);
        } else {
            toxin->posx = posx;
            toxin->posy = posy;
            toxin->mass = mass;
        }

        G_ToxinMap[key] = toxin;
    }
    //printf("5\n"); fflush(stdout);

    for (int i : j["deletedToxins"]) {
        delete G_ToxinMap[i];
        G_ToxinMap.erase(i);
    }
    //printf("6\n"); fflush(stdout);

    map<string, json> bots = j["createdOrUpdatedBots"];
    //printf("6a\n"); fflush(stdout);
    for (auto&& kv : bots) {
        int botId = stoi(kv.first);
        Bot* bot = new Bot();

        // Probably make this more memory-friendly?
        //deleteBot(botId);

        std::map<string, json> blobs = (kv.second)["blobs"]; // Possible seg-fault here (Why??)
        //printf("6b\n"); fflush(stdout);
        for (auto&& kv2 : blobs) {
            int blobId = stoi(kv2.first);
            //printf("6c\n"); fflush(stdout);
            //cout << kv2.second << "\n";
            int posx =   (kv2.second)["pos"]["X"].get<int>();
            int posy =   (kv2.second)["pos"]["Y"].get<int>();
            float mass = (kv2.second)["mass"].get<float>();
            //printf("6d\n"); fflush(stdout);
            bot->blobMap[blobId] = new Blob(posx, posy, mass);
            //printf("6e\n"); fflush(stdout);
        }
        G_BotMap[botId] = bot;
    }

    //printf("7\n"); fflush(stdout);

    for (int i : j["deletedBots"]) {
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
    }

    //printf("8\n"); fflush(stdout);
    map<string, json> botInfos = j["createdOrUpdatedBotInfos"];
    for (auto&& kv : botInfos) {
        int key = stoi(kv.first);
        int r = (kv.second)["color"]["R"].get<int>();
        int g = (kv.second)["color"]["G"].get<int>();
        int b = (kv.second)["color"]["B"].get<int>();
        string name = (kv.second)["name"].get<string>();

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
    }
    //printf("9\n"); fflush(stdout);
    for (int i : j["deletedBotInfos"]) {
        BotInfo* bi = G_BotInfoMap[i];
        if (bi) {
            delete bi->color;
            delete bi;
            G_BotInfoMap.erase(i);
        }
    }
    //printf("10\n"); fflush(stdout);
}

void handleWSData(void * aArg) {

    if (0) {
        WsClient client("localhost:8080");
        client.onmessage=[&client](shared_ptr<WsClient::Message> message) {

            //auto message_str=message->string();
            handle_message(message->string());
            G_wsCounter++;
            if (G_wsCounter % 100 == 0) {
                //printf("G_wsCounter == %i\n", G_wsCounter);
            }
            //cout << "Client: Message received: \"" << message_str << "\"" << endl;
        };

        client.onopen=[&client]() {
            cout << "Client: Opended " << endl;
        };

        client.onclose=[](int status, const string& reason) {
            cout << "Client: Closed connection with status code " << status << endl;
        };

        client.onerror=[](const boost::system::error_code& ec) {
            cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
        };

        client.start();
    } else {

        WebSocket::pointer ws = WebSocket::from_url(G_WSAddress);
        if (!ws) {
            printf("The websocket could not be initialised. The websocket thread exits.\n");
            return;
        }
        assert(ws);
        while (ws->getReadyState() != WebSocket::CLOSED) {
            ws->poll();
            ws->dispatch(handle_message);
            /*G_wsCounter++;
            if (G_wsCounter % 100 == 0) {
                //printf("G_wsCounter == %i\n", G_wsCounter);
            }*/
        }
        delete ws;
    }
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

void drawColoredQuad(GLfloat r, GLfloat g, GLfloat b) {
    glDisable(GL_CULL_FACE);
    glUseProgram(G_ShaderColor);
        GLfloat mp[16], mv[16];
        glGetFloatv(GL_PROJECTION_MATRIX, mp);
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "projMatrix"),  1, GL_FALSE, &mp[0]);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "viewMatrix"),  1, GL_FALSE, &mv[0]);

        GLfloat color[] = {r, g, b};
        GLfloat cam[] = {GLfloat(getCameraPosition(0)), GLfloat(getCameraPosition(1)), GLfloat(getCameraPosition(2))};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, color);
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "cameraPos"), 1, cam);

        glBindBuffer (GL_ARRAY_BUFFER, G_ObjectsBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(0);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glEnable(GL_CULL_FACE);
}

void drawColoredSphere(GLfloat r, GLfloat g, GLfloat b) {
    glDisable(GL_CULL_FACE);
    glUseProgram(G_ShaderColor);
        GLfloat mp[16], mv[16];
        glGetFloatv(GL_PROJECTION_MATRIX, mp);
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "projMatrix"),  1, GL_FALSE, &mp[0]);
        glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "viewMatrix"),  1, GL_FALSE, &mv[0]);

        GLfloat cam[] = {GLfloat(getCameraPosition(0)), GLfloat(getCameraPosition(1)), GLfloat(getCameraPosition(2))};
        GLfloat light[] = {13,150,19};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "cameraPos"), 1, cam);
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "light"), 1, light);

        glBindVertexArray(G_Sphere.vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
        glBindVertexArray(0);

        //printf("draw start\n"); fflush(stdout);
        GLfloat scale = 1.0;
        // To put the field from -500/500 and not 0/1000.
        GLfloat displacement = -500.0;


        GLfloat translation[] = {GLfloat(light[0]), light[1], GLfloat(light[2])};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);
        GLfloat massLight[] = {500};
        glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, massLight);
        GLfloat colorLight[] = {1, 1, 0};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, colorLight);
        GLint isLight = 1;
        glUniform1i(glGetUniformLocation(G_ShaderColor, "isLight"), isLight);
        glBindVertexArray(G_Sphere.vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
        glBindVertexArray(0);

        isLight = 0;
        glUniform1i(glGetUniformLocation(G_ShaderColor, "isLight"), isLight);

        //printf("start drawing\n"); fflush(stdout);

        // Toxin
        GLfloat colorToxin[] = {r, g, b};
        glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, colorToxin);
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

        //printf("t\n"); fflush(stdout);

        // Food
        GLfloat colorFood[] = {0, 1, 0};
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
        //printf("f\n"); fflush(stdout);
        //printf("drew food: "); fflush(stdout);
        //G_m.lock();
        // Blobs
        for (auto botIt = G_BotMap.begin(); botIt != G_BotMap.end(); botIt++) {
            //printf("1\n"); fflush(stdout);
            Bot* bot = botIt->second;
            //printf("1a\n"); fflush(stdout);
            if (bot) {

                //printf("1b"); fflush(stdout);
                BotInfo* botInfo = G_BotInfoMap[botIt->first];
                if (botInfo) {
                    //printf("2\n"); fflush(stdout);

                    GLfloat r2 = GLfloat(botInfo->color->r/255.0);
                    GLfloat g2 = GLfloat(botInfo->color->g/255.0);
                    GLfloat b2 = GLfloat(botInfo->color->b/255.0);

                    //printf("3\n"); fflush(stdout);
                    GLfloat colorBot[] = {r2, g2, b2};
                    glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, colorBot);
                    //printf("4"); fflush(stdout);
                    //for (auto blobIt = bot->blobMap.begin(); blobIt != bot->blobMap.end(); blobIt++) {
                    map<int, Blob*> blobMap = bot->blobMap;
                    //printf("4a\n"); fflush(stdout);
                    for (auto const& blobIt : blobMap) {
                        //cout << "|" << blobIt.first << "|\n";
                        Blob* blob = blobIt.second;
                        if (blob) {
                            //printf("5"); fflush(stdout);
                            GLfloat translation[] = {GLfloat(displacement + blob->posx * scale), 0, GLfloat(displacement + blob->posy * scale)};
                            glUniform3fv(glGetUniformLocation(G_ShaderColor, "translation"), 1, translation);
                            GLfloat mass[] = {blob->mass};
                            glUniform1fv(glGetUniformLocation(G_ShaderColor, "mass"), 1, mass);
                            //printf("6"); fflush(stdout);

                            glBindVertexArray(G_Sphere.vertexArrayObject);
                            glDrawArrays(GL_TRIANGLES, 0, G_Sphere.numVertices);
                            glBindVertexArray(0);
                            //printf("7"); fflush(stdout);
                        }
                    }
                    //printf("4b\n"); fflush(stdout);
                }
            }
        }
        //printf("\ndraw finish\n"); fflush(stdout);
        //G_m.unlock();


    glUseProgram(0);
    glEnable(GL_CULL_FACE);
}

void cbDisplay (GLFWwindow * window)
{
    int i;
    int modValue = 3000;
    double difValue = 10.0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glViewport (0, 0, G_Width, G_Height);
    setProjection ((double)G_Width/G_Height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (0) {
        gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
             0.0, 0.0, 0.0,
             0.0, 1.0, 0.0);
    } else {
        gluLookAt (getJoyCameraPosition(0), getJoyCameraPosition(1), getJoyCameraPosition(2),
             getJoyCenter(0), getJoyCenter(1), getJoyCenter(2),
             getJoyUp(0), getJoyUp(1), getJoyUp(2));
    }

    //drawColoredQuad(1,0,0);
    drawColoredSphere(1,0,0);

    /* fuer DoubleBuffering */
    glfwSwapBuffers(window);

    glfwSwapInterval(0);
}

void toggleWireframeMode (void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void cbKeyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key)
        {
            case 'q':
            case 'Q':
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case 'f':
            case 'F':
                G_FullScreen = !G_FullScreen;
                createWindow();
                registerCallBacks (G_Window);
                mainLoop (G_Window);
                break;
            case GLFW_KEY_F1:
                toggleWireframeMode();
                break;
        }
    }

    if (action == GLFW_RELEASE) {

        switch (key)
        {
            case GLFW_KEY_LEFT:

                break;
            case GLFW_KEY_RIGHT:

                break;
            case GLFW_KEY_UP:
                logic:setKey (1,0);
                break;
            case GLFW_KEY_DOWN:
                setKey (0,0);
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
    /* Seit dem Programmstart vergangene Zeit in Sekunden */
    G_Interval = glfwGetTime();
    glfwSetTime(0.0);

    //calcTimeRelatedStuff(G_Interval);
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

    while (!glfwWindowShouldClose(window))
    {
        calcJoyCameraMovement();
        cbDisplay (window);
        lastCallTime = cbTimer (lastCallTime);
        glfwPollEvents();
        G_openGLCounter++;
        if (G_openGLCounter % 100 == 0) {
            //printf("Average Draw call per Websocket message: %.2f\n", G_openGLCounter / (float)G_wsCounter);
        }
    }

}

int initScene (void)
{
    glEnable (GL_DEPTH_TEST);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
    glEnable (GL_NORMALIZE);
    glEnable (GL_LIGHTING);
    //initLight ();

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

    /* Read Shader from File was here */

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

int initAndStartIO (char* title, int width, int height)
{
    G_Width = width;
    G_Height = height;
    G_WindowTitle = title;
    G_FullScreen = 0;

    if (!glfwInit())
        return 0;

    if (createWindow())
    {
        initJoyCamera();

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

        glGenBuffers(1, &G_ObjectsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, G_ObjectsBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(G_Objects), G_Objects, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        G_Sphere = createUnitSphere(5);

        printf ("--> Initialisation finished\n"); fflush(stdout);

        mainLoop (G_Window);


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
    char * title = "lots and lots of particles ... yeay ... :-)";
    srand (time (0));

    glfwSetErrorCallback(error_callback);

    if (!initAndStartIO (title, 1920, 1080))
    {
        fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
        glfwTerminate();
        return;
    }

    glfwTerminate();

}

/*
 * =====================================================================
 * MAIN STUFF
 * =====================================================================
 */

int main()
{
    thread handleWS   (handleWSData, 0);
    thread handleRest (handleGraphics, 0);

    handleWS.join();
    handleRest.join();

    return 0;
}
