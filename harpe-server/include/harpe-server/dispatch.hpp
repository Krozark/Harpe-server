#ifndef DISPATCH_HPP
#define DISPATCH_HPP

namespace ntw
{
    class SocketSerialized;

    /**
     * \brief Define the dispatch function
     * A simple switch on the id, and call the function
     * of this id
     */
    int dispatch(int id,SocketSerialized& request);
}

#endif
