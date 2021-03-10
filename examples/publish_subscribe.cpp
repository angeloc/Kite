#include <stdio.h>
#include <string>
#include <iostream>

#include "EventLoop.hpp"
#include "Unixbus.hpp"
#include "Timer.hpp"

using namespace Kite;

const int MAX_SEQ = 500;

static const std::string SystemServiceHelloRoute = "session:service:hello";

class SystemServiceHello: public Unixbus
{
public:
    SystemServiceHello(std::weak_ptr<EventLoop> ev)
        : Kite::Unixbus(ev)
    {
        if (!bind(SystemServiceHelloRoute))
            std::cerr << "Error registering service binding\n";

        Kite::Timer::later(ev, [ev, this]() {
            char buff[10];
            for (int i=0; i < MAX_SEQ; i++){
                snprintf(buff, sizeof(buff), "%d", i);
                std::string buffAsStdStr = buff;
                this->sendBusMessage(buffAsStdStr);
            }
            return true;
        }, 1000);
    }
};

class SystemServiceRecv: public Unixbus
{
public:
    int count = 0;
    int recv = 0;
    SystemServiceRecv(std::weak_ptr<EventLoop> ev, int recv)
        : recv(recv), Kite::Unixbus(ev)
    {
        if(!connect(SystemServiceHelloRoute))
            fprintf(stderr, "error connecting");
    }
protected:
    void onBusMessage(const std::string &data, int address)
    {
        if (std::stoi(data.c_str()) == 0)
            count = 0;
        if (count == std::stoi(data.c_str())){
            count++;
        } else {
            fprintf(stdout, "recv%d expecting packet (%d)\n", recv, count);
        }
        if (count == MAX_SEQ)
            fprintf(stdout, "recv%d completed correctly\n", recv);
    }
};

int main()
{
    std::shared_ptr<EventLoop> ev(new EventLoop);
    std::shared_ptr<SystemServiceHello> hello(new SystemServiceHello(ev));
    std::shared_ptr<SystemServiceRecv> recv1(new SystemServiceRecv(ev, 1));
    std::shared_ptr<SystemServiceRecv> recv2(new SystemServiceRecv(ev, 2));
    std::shared_ptr<SystemServiceRecv> recv3(new SystemServiceRecv(ev, 3));
        
    return ev->exec();
}
