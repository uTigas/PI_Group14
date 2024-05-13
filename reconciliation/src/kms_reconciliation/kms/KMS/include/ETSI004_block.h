# define PROGRAM_ETSI004_BLOCK_H_

#include "netxpto_20200819.h"
#include "etsi_qkd_004.h"
#include "json.hpp"


class ETSI004Block : public Block {

public:

    ETSI004Block(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);

    static const unsigned int PUSH = 0;
    static const unsigned int PULL = 1;

    void setID(t_string Id){ID = Id;};
    t_string getID(){return ID;};

    void setMode(unsigned int Mode){mode = Mode;};
    unsigned int getMode(){return mode;};

    void setNumKeys(unsigned int NumKeys){num_keys = NumKeys;}
    unsigned int getNumKeys(){return num_keys;}

    // void setKeyWriteType(t_integer option) { keyWriteType = option;}
    // t_integer getKeyWriteType(void) { return keyWriteType;}

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
        this->key_stream_id = key_stream_id;
    }
    etsi_qkd_004::UUID getKeyStreamId() const{
        return key_stream_id;
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

private:
std::vector<t_message> receivedMessages = {};

//t_integer keyWriteType{ 0 }; // 0-ASCII , 1-B64

unsigned int mode{0};
t_string ID;
t_integer ready;
t_integer i = 0;

json msgJson;
json msgCommand;
json msgData;

unsigned int num_keys = 50;
unsigned int keyID_Rx = 0;
unsigned int keyID_Tx = 0;

etsi_qkd_004::URI source = "source";
etsi_qkd_004::URI destination = "destination";
etsi_qkd_004::QoS qos;
etsi_qkd_004::UUID key_stream_id = "12345678-1234-1234-1234-123456789012";
etsi_qkd_004::Metadata metadata_client = {32,"metadata about the key requested"};
etsi_qkd_004::Metadata metadata_server = {32,"metadata about the key sent"};

};