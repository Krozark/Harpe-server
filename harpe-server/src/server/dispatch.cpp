#include <server/dispatch.hpp>
#include <server/functions.hpp>
#include <Socket/FuncWrapper.hpp>

namespace ntw
{
    int dispatch(int id,SocketSerialized& request)
    {
        int res= FuncWrapper::Status::st::wrong_id;
        switch(id)
        {
            case FUNCTION_ID::GET_VERSION:
            {
                res = ntw::FuncWrapper::srv::exec(getVersion,request);
            }break;
            default:
            break;
        }
        return res;
    }
}
