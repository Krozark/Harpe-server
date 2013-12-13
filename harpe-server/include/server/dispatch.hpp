#ifndef DISPATCH_HPP
#define DISPATCH_HPP

namespace ntw
{
    class SocketSerialized;

    void dispatch(int id,SocketSerialized& request);
}

#endif
