
#include <iostream>
#include "json.hpp"
#include "tinythread.h"
#include "fast_mutex.h"
#include "easywsclient.hpp"

using easywsclient::WebSocket;
using namespace tthread;
using namespace std;
using json = nlohmann::json;

int G_global_data = 0;
mutex m;

void test1(json input) {
    cout << input << endl;
}

void HelloThread(void * aArg)
{
    lock_guard<mutex> guard(m);
    G_global_data++;
    cout << G_global_data << endl;
}

void test2(json input) {
    cout << input << endl;
}

int main()
{

    json j;
    j.push_back("foo");
    j.push_back(1);
    j.push_back(true);

    json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {
            {"everything", 42}
        }},
        {"list", {1, 0, 2}},
        {"object", {
            {"currency", "USD"},
            {"value", 42.99}
        }}
    };

    //thread t(test, 0);
    thread t  (HelloThread, 0);
    thread t2 (HelloThread, 0);
    thread t3 (HelloThread, 0);
    thread t4 (HelloThread, 0);
    cout << j << endl;
    cout << j2 << endl;
    t.join();
    t2.join();
    t3.join();
    t4.join();

    WebSocket::pointer ws = WebSocket::from_url("ws://localhost:1234/servercommand/");
    assert(ws);

    ws->poll();
    ws->send("11");
    //ws->dispatch(handle_message);
    delete ws;

    return 0;
}
