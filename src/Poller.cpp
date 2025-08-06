#include <stdlib.h>

#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop* loop) : loop_(loop) {

}

bool Poller::hasChannel(Channel *channel) const
{
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

