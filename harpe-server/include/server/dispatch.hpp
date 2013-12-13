#ifndef DISPATCH_HPP
#define DISPATCH_HPP

namespace ntw
{
    class SocketSerialized;

    int dispatch(int id,SocketSerialized& request);
}

#endif
