/** Copyright (C) 2021 CCS (UPM) - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Rafael J. Vicente <rafaelj.vicente@upm.es>
 */

#ifndef ETSI_QKD_004_H
#define ETSI_QKD_004_H

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <uuid/uuid.h>

#endif

#include <string>
#include <stdexcept>
#include "json.hpp"
#include <thread>


using json = nlohmann::ordered_json;

namespace etsi_qkd_004 {

    struct LKMSAddress {
        std::string ip;
        unsigned short port;
    };

    inline std::string send_data(const LKMSAddress &address, const std::string &msg) {
        #ifdef _WIN32
            WSADATA wsa;

            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
                throw std::runtime_error("\n ERROR: Winsock creation error \n");
        #endif

        int sock;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw std::runtime_error("\n ERROR: Socket creation error \n");

        // IP Check
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(address.port);
        if (inet_pton(AF_INET, address.ip.c_str(), &serv_addr.sin_addr) <= 0)
            throw std::runtime_error("\n ERROR: Invalid address / Address not supported \n");

        // Socket connect
        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            throw std::runtime_error("\n ERROR: Socket connection Failed \n");

        // Socket send
        send(sock, msg.c_str(), msg.size(), 0);

        // Socket receive
        char buffer[6024] = {0};
        auto valread = recv(sock, buffer, 6024, 0);

        #ifdef _WIN32
            closesocket(sock);
            WSACleanup();
        #else
            close(sock);
        #endif

        return std::string{buffer, static_cast<size_t>(valread)};
    }

    using URI = std::string;
    using UUID = std::string;
    using KeyBuffer = typename std::vector<unsigned char>;

    struct Metadata {
        unsigned int size;
        std::string buffer;
    };

    struct QoS {
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

    struct OpenConnectResponse {
        Status status;
        std::string key_stream_id;
        QoS qos;
    };

    struct GetKeyResponse {
        Status status;
        KeyBuffer key_buffer;
        unsigned int index;
        Metadata metadata;
    };

    struct CloseResponse {
        Status status;
    };

    inline OpenConnectResponse
    open_connect(const LKMSAddress &address, const URI &source, const URI &destination, const QoS &qos,
                 const UUID &key_stream_id = "") {
        // Request to json
        json open_connect_json = {
                {"command", "OPEN_CONNECT"},
                {"data",    {
                                    {"source", source},
                                    {"destination", destination},
                                    {"qos", {
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

        // Send and await for response
        auto response = send_data(address, open_connect_json.dump());

        // Response from json
        json response_json = json::parse(response);
        Status status(response_json["status"]);
        UUID r_key_stream_id = response_json["key_stream_id"];
        QoS r_qos = response_json["qos"].is_null() ? qos :
                    QoS {
                            response_json["qos"]["key_chunk_size"],
                            response_json["qos"]["max_bps"],
                            response_json["qos"]["min_bps"],
                            response_json["qos"]["jitter"],
                            response_json["qos"]["priority"],
                            response_json["qos"]["timeout"],
                            response_json["qos"]["ttl"],
                            response_json["qos"]["metadata_mimetype"],
                    };

        return OpenConnectResponse {status, r_key_stream_id, r_qos};
    }

    inline GetKeyResponse
    get_key(const LKMSAddress &address, const UUID &key_stream_id, unsigned int index, const Metadata &metadata = {}) {
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

        // Send and await for response
        auto response = send_data(address, get_key_json.dump());

        // Response from json
        json response_json = json::parse(response);

        Status status(response_json["status"]);

        json kb_json = response_json["key_buffer"];
        auto key_buffer = kb_json.is_null() ? KeyBuffer {} : KeyBuffer(kb_json.begin(), kb_json.end());

        json index_json = response_json["index"];
        auto r_index = index_json.is_null() ? 0 : (unsigned int) index_json;

        json r_metadata_json = response_json["metadata"];
        auto r_metadata = r_metadata_json.is_null() ? Metadata {0, std::string("")} :
                          Metadata {r_metadata_json["size"], r_metadata_json["buffer"]};

        return GetKeyResponse {status, key_buffer, r_index, r_metadata};
    }

    inline CloseResponse close(const LKMSAddress &address, const UUID &key_stream_id) {
        // Request to json
        json close_json = {
                {"command", "CLOSE"},
                {"data",    {{"key_stream_id", key_stream_id},}},
        };
        // Send and await for response
        auto response = send_data(address, close_json.dump());

        // Response from json
        json response_json = json::parse(response);
        Status status(response_json["status"]);

        return CloseResponse {status};
    }
}

#endif //ETSI_QKD_004_H
