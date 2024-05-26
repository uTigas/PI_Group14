#include "load_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_2024.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ETSI004_block.h"
#include "etsi_qkd_004.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"
#include "key_sync.h"
#include "KeySync_Block.h"

Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

namespace SOUTH {

    Message request{"south_request.sgn",1000,hType,sWriteMode};
    HandlerMessage request_{"south_request_.sgn",1000,hType,sWriteMode};
    Message response{"south_response.sgn",1000,hType,sWriteMode};
    HandlerMessage response_{"south_response_.sgn",1000,hType,sWriteMode};
    Binary key{"south_key.sgn",1024,hType,sWriteMode};
    Message index{"south_index.sgn",5,hType,sWriteMode};

    SaveAscii saveKeys{{&key, &index},{}};
    DestinationTranslationTable dttRxTransmitter;
    MessageHandler MessageHandlerRX{{&response_},{&response}};
    InputTranslationTable ittTxTransmitter;
    MessageHandler MessageHandlerTX{{&request},{&request_}};
    IPTunnel IPTunnel_Client{{&request_},{}};
    IPTunnel IPTunnel_Server{{},{&response_}};
    ETSI004Block ETSI004{{&response},{&request, &key, &index}};

    void setup(){

        DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
        param.setInputParametersFileName("input_south.txt");
        param.readSystemInputParameters();

        saveKeys.setFile_type(param.fileType);
        saveKeys.setAsciiFolderName("../saved_keys");
        saveKeys.setAsciiFileName(param.keyType ? "obl_keys" : "sym_keys");
        saveKeys.setAsciiFileNameTailNumber("0");
        saveKeys.setAsciiFileNameTailNumberModulos(0);
        saveKeys.setInsertId(true);
        if(param.fileType) saveKeys.setAsciiFileNameExtension("b64");

        dttRxTransmitter.add("KMS_South", 0);
        MessageHandlerRX = MessageHandler{{&response_},{&response}, dttRxTransmitter, FUNCTIONING_AS_RX};
        ittTxTransmitter.add(0, {"Key_Provider", "KMS_South"});
        MessageHandlerTX = MessageHandler{{&request},{&request_}, FUNCTIONING_AS_TX, ittTxTransmitter};

        IPTunnel_Client.setLocalMachineIpAddress(param.txIpAddress);
        IPTunnel_Client.setRemoteMachineIpAddress(param.rxIpAddress);
        IPTunnel_Client.setRemoteMachinePort(param.rxReceivingPort);
        IPTunnel_Client.setVerboseMode(param.verboseMode);

        IPTunnel_Server.setLocalMachineIpAddress(param.txIpAddress);
        IPTunnel_Server.setRemoteMachineIpAddress(param.rxIpAddress);
        IPTunnel_Server.setLocalMachinePort(param.txReceivingPort);
        IPTunnel_Server.setVerboseMode(param.verboseMode);

        ETSI004.setID("Tx");
        ETSI004.setMode(ETSI004Block::PUSH);
        ETSI004.setSource(param.etsiSource);
        ETSI004.setDestination(param.etsiDest);
        ETSI004.setQoS((unsigned int) param.keyType, (unsigned int) param.keyChunkSize, (unsigned int) param.maxBps, (unsigned int) param.minBps, (unsigned int) param.jitter, (unsigned int) param.priority, (unsigned int) param.timeout, (unsigned int) param.ttl, param.metaMimetype );
        ETSI004.setNumKeys((unsigned int) param.numKeys);
        ETSI004.setVerboseMode(param.verboseMode);
    }
}

namespace NORTH {

    Message request{"north_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"north_request_.sgn",10,hType,sWriteMode};
    Message response{"north_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"north_response_.sgn",10,hType,sWriteMode};
    Binary key{"north_key.sgn",1024,hType,sWriteMode};
    Binary key_type{"north_key_type.sgn",5,hType,sWriteMode};

    LoadAscii readKeys({&key_type},{&key}); // precisa de ser alterado para ter em conta nome dos ficheiros criados pelo kms e index, etc..
    IPTunnel IPTunnel_Server{{},{&request_}};
    DestinationTranslationTable dttRxTransmitter;
    MessageHandler MessageHandlerRX{{&request_},{&request}};
    InputTranslationTable ittTxTransmitter;
    MessageHandler MessageHandlerTX{{&response},{&response_}};
    IPTunnel IPTunnel_Client{{&response_},{}};
    ETSI004Block ETSI004{{&request, &key},{&response, &key_type}};

    void setup() {

        DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
        param.setInputParametersFileName("input_north.txt");
        param.readSystemInputParameters();

        readKeys.setAsciiFileNameTailNumber("0");
        if(param.fileType) readKeys.setAsciiFileNameExtension(".b64");

        IPTunnel_Server.setLocalMachineIpAddress(param.rxIpAddress);
        IPTunnel_Server.setRemoteMachineIpAddress(param.txIpAddress);
        IPTunnel_Server.setLocalMachinePort(param.rxReceivingPort);
        IPTunnel_Server.setVerboseMode(param.verboseMode);

        IPTunnel_Client.setLocalMachineIpAddress(param.rxIpAddress);
        IPTunnel_Client.setRemoteMachineIpAddress(param.txIpAddress);
        IPTunnel_Client.setRemoteMachinePort(param.txReceivingPort);
        IPTunnel_Client.setVerboseMode(param.verboseMode);

        dttRxTransmitter.add("KMS_North", 0);
        MessageHandlerRX = MessageHandler{{&request_},{&request}, dttRxTransmitter, FUNCTIONING_AS_RX};
        ittTxTransmitter.add(0, {"APP_A", "KMS_North"});
        MessageHandlerTX = MessageHandler{{&response},{&response_}, FUNCTIONING_AS_TX, ittTxTransmitter };

        ETSI004.setID("Rx");
        ETSI004.setMode(ETSI004Block::PULL);
        ETSI004.setNumKeys( (unsigned int) param.numKeys);
        ETSI004.setVerboseMode(param.verboseMode);
    }
}

namespace KeySync {

    Message request{"sync_request.sgn",1000,hType,sWriteMode};
    HandlerMessage request_{"sync_request_.sgn",1000,hType,sWriteMode};
    Message response{"sync_response.sgn",1000,hType,sWriteMode};
    HandlerMessage response_{"sync_response_.sgn",1000,hType,sWriteMode};

    DestinationTranslationTable dttRxTransmitter;
    MessageHandler MessageHandlerRX{{&response_},{&response}};
    InputTranslationTable ittTxTransmitter;
    MessageHandler MessageHandlerTX{{&request},{&request_}};
    IPTunnel IPTunnel_Client{{&request_},{}};
    IPTunnel IPTunnel_Server{{},{&response_}};
    KeySyncBlock KeySync{{&response,&SOUTH::index},{&request}};

    void setup(){

        dttRxTransmitter.add("Key_Sync", 0);
        MessageHandlerRX = MessageHandler{{&response_},{&response}, dttRxTransmitter, FUNCTIONING_AS_RX};
        ittTxTransmitter.add(0, {"KMS_Peer", "Key_Sync"});
        MessageHandlerTX = MessageHandler{{&request},{&request_}, FUNCTIONING_AS_TX, ittTxTransmitter};

        IPTunnel_Client.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client.setRemoteMachinePort(54005);
        IPTunnel_Client.setVerboseMode(true);

        IPTunnel_Server.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Server.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Server.setLocalMachinePort(54006);
        IPTunnel_Server.setVerboseMode(true);

    }
}

int main(){

    SOUTH::setup();
    NORTH::setup();
    KeySync::setup();

    System System_
            {
                {
                &SOUTH::saveKeys,
                &SOUTH::ETSI004,
                &SOUTH::IPTunnel_Client,
                &SOUTH::IPTunnel_Server,
                &SOUTH::MessageHandlerTX,
                &SOUTH::MessageHandlerRX,
                &KeySync::KeySync,
                &KeySync::IPTunnel_Client,
                &KeySync::IPTunnel_Server,
                &KeySync::MessageHandlerTX,
                &KeySync::MessageHandlerRX,
                &NORTH::readKeys,
                &NORTH::IPTunnel_Server,
                &NORTH::ETSI004,
                &NORTH::IPTunnel_Client,
                &NORTH::MessageHandlerTX,
                &NORTH::MessageHandlerRX,
                }
            };
    
    System_.run();
    System_.terminate();

    return 0;
}