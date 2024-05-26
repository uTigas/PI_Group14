#include "etsi_qkd_004.h"



 json etsi_qkd_004::open_connect(const URI &source, const URI &destination, const QoS &qos, const UUID &key_stream_id) {
        // Request to json
        json open_connect_json = {
                {"command", "OPEN_CONNECT"},
                {"data",    {
                                    {"source", source},
                                    {"destination", destination},
                                    {"qos", {
                                                    {"key_type", qos.key_type},
                                                    {"key_chunk_size", qos.key_chunk_size},
                                                    {"max_bps", qos.max_bps},
                                                    {"min_bps", qos.min_bps},
                                                    {"jitter", qos.jitter},
                                                    {"priority", qos.priority},
                                                    {"timeout", qos.timeout},
                                                    {"ttl", qos.ttl},
                                            }
                                    }
                            }
                },
        };
        if (!qos.metadata_mimetype.empty()) open_connect_json["data"]["qos"]["metadata_mimetype"] = qos.metadata_mimetype;
        if (!key_stream_id.empty()) open_connect_json["data"]["key_stream_id"] = key_stream_id;

        return open_connect_json;

    }
//função que deverá ser usada pelo servidor para responder ao open_connect
//devolve um json para que este seja posteriormente convertido num sinal
    json etsi_qkd_004::handle_open_connect(const UUID &key_stream_id, const QoS &qos, Status status){

        json open_connect_response_json = {
                {"command", "OPEN_CONNECT_RESPONSE"},
                {"data", {
                            {"status", status},
                            {"key_stream_id", key_stream_id},
                            {"qos", {
                                                    {"key_type", qos.key_type},
                                                    {"key_chunk_size", qos.key_chunk_size},
                                                    {"max_bps", qos.max_bps},
                                                    {"min_bps", qos.min_bps},
                                                    {"jitter", qos.jitter},
                                                    {"priority", qos.priority},
                                                    {"timeout", qos.timeout},
                                                    {"ttl", qos.ttl},
                                    }
                            }

                        }
                    

                },
        };
        return open_connect_response_json;
    }

//função que deverá ser usada pelo client para fazer um pedido get_key
//devolve um json para que este seja posteriormente convertido num sinal
    json etsi_qkd_004::get_key(const UUID &key_stream_id, unsigned int index, const Metadata &metadata)  {
        // Request to json
        json get_key_json = {
                {"command", "GET_KEY"},
                {"data",    {
                                    {"key_stream_id", key_stream_id},
                                    {"index", index},
                            }
                },
        };
        if (metadata.size > 0) {
            json metadata_json = {
                    {"size",   metadata.size},
                    {"buffer", metadata.buffer},
            };
            get_key_json["data"]["metadata"] = metadata_json;
        }

        return get_key_json;

    }

//função que deverá ser usada pelo servidor para responder ao get_key
//devolve um json para que este seja posteriormente convertido num sinal

   json etsi_qkd_004::handle_get_key(const Status status, const KeyBuffer& key_buffer, unsigned int index, const Metadata &metadata) {
        //Verificar se as condições correspondem ao etsi 
        auto kb = key_buffer.empty() ? KeyBuffer{} : key_buffer;
        auto r_index = index;
        auto r_metadata = metadata.size > 0 ? Metadata{metadata.size, metadata.buffer} : Metadata{0, std::string("")};

        json get_key_response_json = {
            {"command", "GET_KEY_RESPONSE"},
            {"data", {
                        {"status", status},
                        {"key_buffer", kb},
                        {"index", r_index},
                        {"metadata", {
                                        {"size", r_metadata.size},
                                        {"buffer", r_metadata.buffer}
                                    }},
                }
            },
        };
        return get_key_response_json;
    }


//função que deverá ser usada pelo client para fazer um pedido close 
//devolve um json para que este seja posteriormente transformado num sinal 
    json etsi_qkd_004::close(const UUID &key_stream_id) {
        // Request to json
        json close_json = {
                {"command", "CLOSE"},
                {"data",    {{"key_stream_id", key_stream_id},}},
        };

        return close_json;
    }
//função que deverá ser usada pelo servidor para responder ao close
//devolve um json para que este seja posteriormente transformado num sinal

    json etsi_qkd_004::handle_close(const Status status){
        
        json close_response_json = {
                {"command", "CLOSE_RESPONSE"},
                {"data", {
                            {"status", status},
                        }
                },
        };
        return close_response_json;
    }