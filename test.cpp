
#include <iostream>
#include "json.hpp"
#include "tinythread.h"
#include "fast_mutex.h"
#include "easywsclient.hpp"
#include <unordered_map>
#include <map>

using easywsclient::WebSocket;
using namespace tthread;
using namespace std;
using json = nlohmann::json;

/*
 * CLASS DEFINITIONS
 */


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


/*
 * GLOBAL VARIABLES
 */

int G_global_data = 0;
mutex G_m;
char G_WSAddress[] = "ws://127.0.0.1:1234";

/*
 * WEBSOCKET GAME DATA
 */

map <int, Food*> G_FoodMap;
map <int, Toxin*> G_ToxinMap;

/*
 * FUNCTIONS
 */

void HelloThread(void * aArg)
{
    lock_guard<mutex> guard(G_m);
    G_global_data++;
    cout << G_global_data << endl;
}


/**
type ServerGuiUpdateMessage struct {
    CreatedOrUpdatedBotInfos    map[string]BotInfo              `json:"createdOrUpdatedBotInfos"`
    DeletedBotInfos             []BotId                         `json:"deletedBotInfos"`
    CreatedOrUpdatedBots        map[string]ServerGuiBot         `json:"createdOrUpdatedBots"`
    DeletedBots                 []BotId                         `json:"deletedBots"`
    *
    CreatedOrUpdatedFoods       map[string]ServerGuiFood        `json:"createdOrUpdatedFoods"`
    DeletedFoods                []FoodId                        `json:"deletedFoods"`
    *
    CreatedOrUpdatedToxins      map[string]ServerGuiToxin       `json:"createdOrUpdatedToxins"`
    DeletedToxins               []ToxinId                       `json:"deletedToxins"`
    *
    StatisticsThisGame          map[string]Statistics           `json:"statisticsLocal"`
    StatisticsGlobal            map[string]Statistics           `json:"statisticsGlobal"`
}
*/

void handle_message(const std::string & message)
{
    if (!message.compare("alive_test")) {
        return;
    }

    json j = json::parse(message);

    std::map<std::string, json> foods = j["createdOrUpdatedFoods"];
    for (auto&& kv : foods) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        G_FoodMap[int(key)] = (Food*)(new Food(posx, posy, mass));
    }
    for (int i : j["deletedFoods"]) {
        G_FoodMap.erase(i);
    }

    std::map<std::string, json> toxins = j["createdOrUpdatedToxins"];
    for (auto&& kv : toxins) {
        int key = stoi(kv.first);
        int posx =   (kv.second)["pos"]["X"].get<int>();
        int posy =   (kv.second)["pos"]["Y"].get<int>();
        float mass = (kv.second)["mass"].get<float>();

        // overwrite old entries
        G_ToxinMap[int(key)] = (Toxin*)(new Toxin(posx, posy, mass));
    }
    for (int i : j["deletedToxins"]) {
        G_ToxinMap.erase(i);
    }




}

void handleWSData(void * aArg) {

    WebSocket::pointer ws = WebSocket::from_url(G_WSAddress);
    assert(ws);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        ws->dispatch(handle_message);
    }
    delete ws;
}

void handleGraphics(void * aArg) {
    // ...
}

int main()
{

    thread handleWS   (handleWSData, 0);
    thread handleRest (handleGraphics, 0);

    handleWS.join();
    handleRest.join();

    return 0;
}
