#include "key_sync.h"

json key_sync::key_sync(const URI &source, const URI &destination, const Status status, unsigned int index,const QoS &qos){
    json key_sync_json ={
        {"command", "KEY_SYNC"},
        {"data", {
            {"source", source},
            {"destination", destination},
            {"qos", {
                {"key_type", qos.key_type},
                {"key_chunk_size", qos.key_chunk_size},
            }
        },
        },
        },
        {"status", status},
        {"index", index},

    };
    return key_sync_json;
}