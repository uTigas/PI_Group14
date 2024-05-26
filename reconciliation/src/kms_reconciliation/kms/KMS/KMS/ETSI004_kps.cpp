#include "ETSI004_kps.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

void ETSI004kps::initialize(void){

}

bool ETSI004kps::runBlock(void){

    bool alive = true;
    if (getTerminated()) return false;

    ready = inputSignals[0]->ready();
    if(ready){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        receivedMessages.push_back(msgReceived);
        msgJson = json::parse(msgReceived.getMessageData());
        msgCommand = msgJson["command"];
        msgData = msgJson["data"];
    } else if ( msgCommand != "GET_KEY" ){
        msgCommand = nullptr;
    }

    if (msgCommand == "OPEN_CONNECT"){
        if(getVerboseMode()){
            std::cout << "RECEIVED OPEN_CONNECT" << std::endl;
        }

        // Check key_stream_id
        if (!msgData.contains("key_stream_id")) {

            // generate random UUID
            boost::uuids::random_generator generator;
            boost::uuids::uuid uuid = generator();
            etsi_qkd_004::UUID key_stream_ID = boost::uuids::to_string(uuid);

            // verify if already exists and generate another UUID if so
            while(Sessions.find(key_stream_ID) != Sessions.end()){
                uuid = generator();
                key_stream_ID = boost::uuids::to_string(uuid);
            }

            KSID = key_stream_ID;
            SessionInfo info;
            info.index = 0;
            Sessions[KSID] = info;
            
        } else {
            etsi_qkd_004::UUID key_stream_ID = msgData["key_stream_id"];
            if (Sessions.find(key_stream_ID) == Sessions.end()) {
                KSID = key_stream_ID;
                SessionInfo info;
                info.index = 0;
                Sessions[KSID] = info;
            } else { status = etsi_qkd_004::OC_KSID_ALREADY_IN_USE;}
        }

        unsigned int key_type = msgData["qos"]["key_type"];
        unsigned int key_chunk_size = msgData["qos"]["key_chunk_size"];
        unsigned int max_bps = msgData["qos"]["max_bps"];
        unsigned int min_bps = msgData["qos"]["min_bps"];
        unsigned int jitter = msgData["qos"]["jitter"];
        unsigned int priority = msgData["qos"]["priority"];
        unsigned int timeout = msgData["qos"]["timeout"];
        unsigned int ttl = msgData["qos"]["ttl"];
        std::string metadata_mimetype = msgData["qos"]["metadata_mimetype"];
        Sessions[KSID].key_chunk_size = key_chunk_size;
        Sessions[KSID].key_type = key_type;
        setQoS(key_type, key_chunk_size, max_bps, min_bps, jitter, priority, timeout, ttl, metadata_mimetype);

        // signal with key_type
        if (key_type == 0){
            outputSignals[1]->bufferPut((t_binary)0);
        } else if (key_type == 1){ outputSignals[1]->bufferPut((t_binary)1);}
        
        t_message msgSend;
        t_string msgDataSend = etsi_qkd_004::handle_open_connect(KSID,qos,status).dump();
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);

    } else if (msgCommand == "GET_KEY" && !(cnt % sampleP)){
        if(getVerboseMode()){
            std::cout << "RECEIVED GET_KEY" << std::endl;
        }

        KSID = msgData["key_stream_id"];
        
        if(inputSignals[1]->ready() && Sessions[KSID].index < num_keys){
            etsi_qkd_004::KeyBuffer keyBuffer;
            for(auto k = 0; k < Sessions[KSID].key_chunk_size; k++){
                t_binary kval{0};
                inputSignals[1]->bufferGet(&kval);
                keyBuffer.push_back(kval);
            }

            t_string msgDataSend;
            if(!keyBuffer.empty()){
                status = etsi_qkd_004::SUCCESSFUL;
                msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,Sessions[KSID].index++,metadata_server).dump();
            } else { 
                status = etsi_qkd_004::GK_INSUFFICIENT_KEY_AVAILABLE;
                msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,Sessions[KSID].index,metadata_server).dump();
            }

            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
        }
    } else if (msgCommand == "CLOSE"){
        if(getVerboseMode()){
            std::cout << "RECEIVED CLOSE" << std::endl;
        }

        KSID = msgData["key_stream_id"];

        // send response
        t_message msgSend;
        t_string msgDataSend = etsi_qkd_004::handle_close(status).dump();
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);

        if(getVerboseMode()){
            for(const auto& msg : receivedMessages){
                std::cout << "MSG: " << i << " - " << msg << std::endl;
                i++;
            }
        }
        setTerminated(true);
    }
    cnt++;
    return true;
}