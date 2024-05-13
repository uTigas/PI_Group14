#ifndef KEY_SYNC_H
#define KEY_SYNC_H


#include <unistd.h>
#include <uuid/uuid.h>

#include <string>
#include <stdexcept>
#include "json.hpp"
#include <thread>
#include "message_handler.h"
#include "netxpto_20200819.h"


using json = nlohmann::ordered_json;

namespace key_sync {

    using URI = std::string;
    using UUID = std::string;
    using KeyBuffer = typename std::vector<unsigned char>;

    const unsigned int SYMMETRIC = 0;
    const unsigned int OBLIVIOUS = 1;

    struct Metadata {
        unsigned int size;
        std::string buffer;
    };

    struct QoS {
        unsigned int key_type;
        unsigned int key_chunk_size;
    };

    enum Status : unsigned int {
        SUCCESSFUL = 0, PEER_NOT_CONNECTED = 1, GK_INSUFFICIENT_KEY_AVAILABLE = 2, GK_PEER_NOT_CONNECTED = 3,
        NO_QKD_CONNECTION_AVAILABLE = 4, OC_KSID_ALREADY_IN_USE = 5, TIMEOUT_ERROR = 6, OC_QOS_NOT_SATISFIABLE = 7,
        GK_METADATA_INSUFFICIENT_SIZE = 8
    };

    //devolve um json para que este seja posteriormente convertido num sinal
    json key_sync(const URI &source, const URI &destination, const Status status, unsigned int index,const QoS &qos);

}

#endif //ETSI_QKD_004_H
