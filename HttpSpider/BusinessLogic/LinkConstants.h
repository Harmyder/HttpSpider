#pragma once

namespace BusinessLogic
{
    enum LinkState
    {
        STARTED,
        DOWNLOADING,
        FINISHED,
        CONNECT_TASK_FINISHED // Bad, bad ...
    };


#define ERROR_NO_ID        -1
#define ERROR_SUCCESS_ID    0
#define ERROR_RECEIVING_ID  1
#define ERROR_CONNECTING_ID 2

#define ERROR_SUCCESS_STRING  "Success"
#define ERROR_RECEIVING_STRING  "Receiving error"
#define ERROR_CONNECTING_STRING "Connecting error"
}