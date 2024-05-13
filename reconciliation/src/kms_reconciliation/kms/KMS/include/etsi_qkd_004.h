

#ifndef ETSI_QKD_004_H
#define ETSI_QKD_004_H


#include <unistd.h>
#include <uuid/uuid.h>

#include <string>
#include <stdexcept>
#include "json.hpp"
#include <thread>
#include "message_handler.h"
#include "netxpto_20200819.h"


using json = nlohmann::ordered_json;

namespace etsi_qkd_004 {

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
        unsigned int max_bps;
        unsigned int min_bps;
        unsigned int jitter;
        unsigned int priority;
        unsigned int timeout;
        unsigned int ttl;
        std::string metadata_mimetype;
    };

    enum Status : unsigned int {
        SUCCESSFUL = 0, PEER_NOT_CONNECTED = 1, GK_INSUFFICIENT_KEY_AVAILABLE = 2, GK_PEER_NOT_CONNECTED = 3,
        NO_QKD_CONNECTION_AVAILABLE = 4, OC_KSID_ALREADY_IN_USE = 5, TIMEOUT_ERROR = 6, OC_QOS_NOT_SATISFIABLE = 7,
        GK_METADATA_INSUFFICIENT_SIZE = 8
    };

//função que deverá ser usada pelo client para fazer um open_connect 
//devolve um json para que este seja posteriormente convertido num sinal
    json open_connect(const URI &source, const URI &destination, const QoS &qos,
                const UUID &key_stream_id = "");

//função que deverá ser usada pelo servidor para responder ao open_connect
//devolve um json para que este seja posteriormente convertido num sinal
    json handle_open_connect(const UUID &key_stream_id, const QoS &qos, Status status);

//função que deverá ser usada pelo client para fazer um pedido get_key
//devolve um json para que este seja posteriormente convertido num sinal
    json get_key(const UUID &key_stream_id, unsigned int index, const Metadata &metadata = {});

//função que deverá ser usada pelo servidor para responder ao get_key
//devolve um json para que este seja posteriormente convertido num sinal

    json handle_get_key(const Status status, const KeyBuffer& key_buffer, unsigned int index, const Metadata &metadata = {});


//função que deverá ser usada pelo client para fazer um pedido close 
//devolve um json para que este seja posteriormente transformado num sinal 
    json close(const UUID &key_stream_id);
//função que deverá ser usada pelo servidor para responder ao close
//devolve um json para que este seja posteriormente transformado num sinal

    json handle_close(const Status status);
}

#endif //ETSI_QKD_004_H
