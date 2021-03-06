#include "Unixbus.hpp"
#include "3rdparty/afunix_polyfill/afunix_polyfill.h"
#include <iostream>

using namespace Kite;

Unixbus::Unixbus(const std::weak_ptr<EventLoop> &ev)
    : File(ev)
{
    int fd = afunix_socket(0);
    setFile(fd);
}

Unixbus::~Unixbus()
{
    close();
}

bool Unixbus::invoke(const std::string &path, const std::string &data)
{
    int fd = afunix_socket(0);
    if (fd < 0) {
        perror("afunix_socket");
        return false;
    }
    if (afunix_connect(fd, path.c_str(), AFUNIX_PATH_CONVENTION) != 0) {
        perror("afunix_connect");
        return false;
    }
    if (::send(fd, data.data(), data.length(), 0) != data.length()) {
        perror("send");
        return false;
    }
    afunix_close(fd);
    return true;
}

bool Unixbus::bind (const std::string &path)
{
    return (afunix_bind(d_fd, path.c_str(), AFUNIX_PATH_CONVENTION | AFUNIX_MAKE_PATH) == 0);
}

bool Unixbus::connect (const std::string &path)
{
    return (afunix_connect(d_fd, path.c_str(), AFUNIX_PATH_CONVENTION) == 0);
}

void Unixbus::close()
{
    if (d_fd == 0)
        return;
    afunix_close(d_fd);
    setFile(0);
    onBusClosed();
}

bool Unixbus::sendBusMessage(const std::string &data, int address)
{
    return (afunix_sendto(d_fd, (void*)data.data(), data.length(), 0, address)
            == data.length());
}

void Unixbus::onBusClosed()
{
}

void Unixbus::onBusMessage(const std::string &data, int address)
{
}

void Unixbus::onActivated(int fd, int e)
{
    if (!d_fd) {
        fprintf(stderr, "BUG!! Unixbus::onActivated called after close\n");
        abort();
    }
    char buf[AFUNIX_MAX_PACKAGE_SIZE];
    int address;
    int r = afunix_recvfrom(d_fd, &buf, AFUNIX_MAX_PACKAGE_SIZE, MSG_DONTWAIT, &address);
    if (r < 1) {
        close();
        return;
    }
    onBusMessage(std::string(buf, r), address);
}


