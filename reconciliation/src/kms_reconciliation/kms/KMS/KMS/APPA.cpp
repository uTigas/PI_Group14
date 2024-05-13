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

int main(){
    DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
    param.setInputParametersFileName("input_APPA.txt");
    param.readSystemInputParameters();

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Message request{"appA_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"appA_request_.sgn",10,hType,sWriteMode};
    Message response{"appA_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"appA_response_.sgn",10,hType,sWriteMode};
    Binary key{"appA_key.sgn",1024,hType,sWriteMode};
    Message index{"appA_index.sgn",5,hType,sWriteMode};

    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("APP_A", 0);
    MessageHandler MessageHandlerRX{{&response_},{&response}, dttRxTransmitter, FUNCTIONING_AS_RX};
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"KMS_North", "APP_A"});
    MessageHandler MessageHandlerTX{{&request},{&request_}, FUNCTIONING_AS_TX, ittTxTransmitter};

    IPTunnel IPTunnel_Client{{&request_},{}};
    IPTunnel_Client.setLocalMachineIpAddress(param.txIpAddress);
    IPTunnel_Client.setRemoteMachineIpAddress(param.rxIpAddress);
    IPTunnel_Client.setRemoteMachinePort(param.rxReceivingPort);
    IPTunnel_Client.setVerboseMode(param.verboseMode);

    IPTunnel IPTunnel_Server{{},{&response_}};
    IPTunnel_Server.setLocalMachineIpAddress(param.txIpAddress);
    IPTunnel_Server.setRemoteMachineIpAddress(param.rxIpAddress);
    IPTunnel_Server.setLocalMachinePort(param.txReceivingPort);
    IPTunnel_Server.setVerboseMode(param.verboseMode);

    ETSI004Block ETSI004{{&response},{&request, &key, &index}};
    ETSI004.setID("Tx");
    ETSI004.setMode(ETSI004Block::PULL);
    ETSI004.setSource(param.etsiSource);
    ETSI004.setDestination(param.etsiDest);
    ETSI004.setQoS((unsigned int) param.keyType, (unsigned int) param.keyChunkSize, (unsigned int) param.maxBps, (unsigned int) param.minBps, (unsigned int) param.jitter, (unsigned int) param.priority, (unsigned int) param.timeout, (unsigned int) param.ttl, param.metaMimetype );
    ETSI004.setNumKeys((unsigned int) param.numKeys);
    ETSI004.setVerboseMode(param.verboseMode);

    System System_
            {
                {
                &ETSI004,
                &IPTunnel_Client,
                &IPTunnel_Server,
                &MessageHandlerTX,
                &MessageHandlerRX
                }
            };

    System_.run();
    System_.terminate();

    return 0;
}