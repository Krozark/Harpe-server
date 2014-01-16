#include <server/dispatch.hpp>

#include <Socket/FuncWrapper.hpp>

#include <server/functions.hpp>

namespace ntw
{
    int dispatch(int id,SocketSerialized& request)
    {
        int res= FuncWrapper::Status::st::wrong_id;
        std::cout<<"[dispatch] id:"<<id<<std::endl<<std::flush;
        switch(id)
        {
            case FUNCTION_ID::GET_VERSION:
            {
                res = ntw::FuncWrapper::srv::exec(getVersion,request);
            }break;
            case FUNCTION_ID::ANALYSE:
            {
                res = ntw::FuncWrapper::srv::exec(analyse,request);
            }break;
            case FUNCTION_ID::CLIENT_WAIT_FOR_WORK :
            {
                res = ntw::FuncWrapper::srv::exec(clientWaitForWork,request);
            }break;
            case FUNCTION_ID::SEND_PEPTIDE_RESULTS : 
            {
                int pk;
                request>>pk;
                sendPeptideResults(request,pk);
                res = request.getStatus();

            }break;
            default:
            break;
        }
        return res;
    }
}
