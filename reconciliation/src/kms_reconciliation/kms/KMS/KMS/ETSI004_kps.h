# define PROGRAM_ETSI004_KPS_H_

#include "netxpto_20200819.h"
#include "etsi_qkd_004.h"
#include "json.hpp"
#include <unordered_map>

class ETSI004kps : public Block {

public:

    ETSI004kps(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);

    void setQoS(unsigned int key_type = 0, unsigned int key_chunk_size = 0, unsigned int max_bps = 0, unsigned int min_bps = 0, unsigned int jitter = 0, unsigned int priority = 0, unsigned int timeout = 0, unsigned int ttl = 0, std::string metadata_mimetype = "JSON"){        
        this->qos.key_type = key_type;
        this->qos.key_chunk_size = key_chunk_size;
        this->qos.max_bps = max_bps;
        this->qos.min_bps = min_bps;
        this->qos.jitter = jitter;
        this->qos.priority = priority;
        this->qos.timeout = timeout;
        this->qos.ttl = ttl;
        this->qos.metadata_mimetype = metadata_mimetype;
    }
    etsi_qkd_004::QoS getQoS() const{
        return qos;
    }

    void setKeyStreamId(const etsi_qkd_004::UUID& key_stream_id){
        this->KSID = key_stream_id;
    }
    etsi_qkd_004::UUID getKeyStreamId() const{
        return KSID;
    }

    void setMetadataClient(const etsi_qkd_004::Metadata& metadata, unsigned int size, std::string buffer){
        this->metadata_client = metadata;
        this->metadata_client.size = size;
        this->metadata_client.buffer = buffer;
    }
    etsi_qkd_004::Metadata getMetadataClient() const{
        return metadata_client;
    }

    void setMetadataServer(const etsi_qkd_004::Metadata& metadata, unsigned int size, std::string buffer){
        this->metadata_server= metadata;
        this->metadata_server.size = size;
        this->metadata_server.buffer = buffer;
    }    
    etsi_qkd_004::Metadata getMetadataServer() const{
        return metadata_server;
    }

    void setSource(etsi_qkd_004::URI source){
        this->source = source;
    }
    etsi_qkd_004::URI getSource(){
        return source;
    }

    void setDestination(etsi_qkd_004::URI destination){
        this->destination = destination;
    }
    etsi_qkd_004::URI getDestination(){
        return destination;
    }

    void setNumKeys(unsigned int NumKeys){num_keys = NumKeys;}
    unsigned int getNumKeys(){return num_keys;}

private:

    struct SessionInfo {
        unsigned int index;
        unsigned int key_type;
        unsigned int key_chunk_size;
    };

    std::vector<t_message> receivedMessages = {};
    std::unordered_map<etsi_qkd_004::UUID, SessionInfo> Sessions;

    t_integer sampleP{ 10 };
    t_integer cnt{0};
    t_integer ready;
    t_integer i{0};

    json msgJson;
    json msgCommand;
    json msgData;

    unsigned int num_keys{0};
    etsi_qkd_004::URI source = "source";
    etsi_qkd_004::URI destination = "destination";
    etsi_qkd_004::QoS qos;
    etsi_qkd_004::UUID KSID = "";
    etsi_qkd_004::Metadata metadata_client = {32,"metadata about the key requested"};
    etsi_qkd_004::Metadata metadata_server = {32,"metadata about the key sent"};
    etsi_qkd_004::Status status{ etsi_qkd_004::SUCCESSFUL };

};