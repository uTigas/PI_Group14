#include "load_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_2024.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ETSI004_kps.h"
#include "etsi_qkd_004.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"

int main(int argc, char *argv[]){

    DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
    param.setInputParametersFileName("input_KPS.txt");
    param.readSystemInputParameters();

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Message request{"Server_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"Server_request_.sgn",10,hType,sWriteMode};
    Message response{"Server_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Server_response_.sgn",10,hType,sWriteMode};
    Binary key{"Server_key.sgn",1024,hType,sWriteMode};
    Binary key_type{"key_type.sgn",10,hType,sWriteMode};


    LoadAscii readKeys({&key_type},{&key});
    readKeys.setProvider(param.provider);
    readKeys.setAsciiFileNameTailNumber(param.cntFirstVal);

    if(param.fileType){
        readKeys.setAsciiFileNameExtension(".b64");
    }

    IPTunnel IPTunnelServer_Server{{},{&request_}};
    IPTunnelServer_Server.setLocalMachineIpAddress(param.kpsIpAddress);
    IPTunnelServer_Server.setRemoteMachineIpAddress(param.sthIpAddress);
    IPTunnelServer_Server.setLocalMachinePort(param.kpsPort);
    IPTunnelServer_Server.setVerboseMode(param.verboseMode);
    IPTunnelServer_Server.setTimeIntervalSeconds(10);

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Key_Provider", 0);
    MessageHandler MessageHandlerServerRX{ {&request_},{&request},dttRxTransmitter,FUNCTIONING_AS_RX};
    
    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"KMS_South", "Key_Provider"});
    MessageHandler MessageHandlerServerTX{ {&response},{&response_},FUNCTIONING_AS_TX,ittTxTransmitter};

    IPTunnel IPTunnelServer_Client{{&response_},{}};
    IPTunnelServer_Client.setLocalMachineIpAddress(param.kpsIpAddress);
    IPTunnelServer_Client.setRemoteMachineIpAddress(param.sthIpAddress);
    IPTunnelServer_Client.setRemoteMachinePort(param.sthPort);
    IPTunnelServer_Client.setVerboseMode(param.verboseMode);
    IPTunnelServer_Client.setTimeIntervalSeconds(10);
    

    ETSI004kps ETSI004{{&request, &key}, {&response, &key_type}};
    ETSI004.setNumKeys((unsigned int)param.numKeys);
    ETSI004.setVerboseMode(param.verboseMode);

    System System_
            {
                {
                &readKeys,
                &IPTunnelServer_Server,
                &ETSI004,
                &IPTunnelServer_Client,
                &MessageHandlerServerTX,
                &MessageHandlerServerRX,
                }
            };

    System_.run();
    System_.terminate();

    return 0;
}