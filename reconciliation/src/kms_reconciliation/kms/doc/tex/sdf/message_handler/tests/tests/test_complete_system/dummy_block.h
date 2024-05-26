#pragma once

// Import of local libraries
#include "netxpto_20200819.h"

class DummyBlock : public Block {
   public:
    DummyBlock(std::initializer_list<Signal*> InputSig,
               std::initializer_list<Signal*> OutputSig, std::string name,
               std::vector<std::string> possibleBlocks)
        : Block(InputSig, OutputSig),
          blockName(name),
          possibleBlocks(
              std::make_shared<std::vector<std::string>>(possibleBlocks)),
          listOfOpenIds(std::make_shared<std::vector<std::uint32_t>>()){};

    void initialize(void){};

    bool runBlock(void) {
        // Process Incoming Messages
        processIncomingMessages();

        // Create new Message
        generateMessages(messagesToGenerate);

        return true;
    };

    void terminate(void) {
        // Check if there were any lost messages Ids
        int lostMessages = listOfOpenIds->size();

        if (lostMessages > 0) {
            std::cout << "Block " << blockName
                      << " terminated the simulation with " << lostMessages
                      << " lost messages\n";
            std::cout << "\tLost Ids: [ ";
            for (auto it = listOfOpenIds->begin(); it != listOfOpenIds->end();
                 it++) {
                std::cout << *it << "; ";
            }
            std::cout << "]" << std::endl;
        } else {
            std::cout << "Block " << blockName
                      << " executed simulation successfully, no lost messages"
                      << std::endl;
        }
    };

    uint getMessagesToGenerate() { return messagesToGenerate; }

    void setMessagesToGenerate(uint number) { messagesToGenerate = number; }

    std::string getBlockName() { return blockName; }

   private:
    //! Get a random Destination block from possible block's list
    std::string getRandomBlock() {
        std::random_device rd;  // Obtain a random seed from the hardware
        std::mt19937 gen(
            rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(
            0, possibleBlocks->size() - 1);  // Define the range of indices

        int randomIndex = dis(gen);  // Generate a random index within the range
        return (*possibleBlocks)
            [randomIndex];  // Return the value at the random index
    };

    std::string getDataFieldValue(t_handler_message& msg, t_string field) {
        // Find the position of "DestinationBlock:"
        size_t position = msg.getMessageData().find(field);

        if (position != std::string::npos) {
            // Extract the substring after "DestinationBlock:"
            std::string value =
                msg.getMessageData().substr(position + (field + ":").size());

            // Find the position of '|' (delimiter)
            size_t endPosition = value.find('|');

            if (endPosition != std::string::npos) {
                // Extract the substring until '|'
                value = value.substr(0, endPosition);
            }

            return value;
        }

        return "";  // Return an empty string if "DestinationBlock:" is not found
    }

    /*!
            Every new message needs to be checked if the values are correct and 
            send a response to the sender. A message is only processed if the Output
            Signal is not full
        */
    void processIncomingMessages(void) {
        auto inputS = inputSignals[0];
        auto outputS = outputSignals[0];
        // Run until the Input Signal is empty or the output Signal is full
        while (inputS->ready() > 0 && outputS->space() > 0) {
            t_handler_message msg;
            inputS->bufferGet(&msg);

            /* Check if the message is valid */
            if (getDataFieldValue(msg, "DestinationBlock") !=
                    msg.getDestinationBlock() ||
                msg.getDestinationBlock() != blockName) {
                // Log error (uses the t_handler_message operator<< to transform the message in ostream)
                std::cout << "Message received by " + blockName +
                                 ", with invalid DestinationBlock: "
                          << msg << std::endl;
                return;
            }

            /* Check if the message was a response (type of 200 is response)*/
            bool elementExists =
                (std::find(listOfOpenIds->begin(), listOfOpenIds->end(),
                           msg.getSequentialId()) != listOfOpenIds->end());
            if (msg.getMessageType() == "200" && elementExists) {
                // Remove from list of open ids
                listOfOpenIds->erase(
                    std::remove(listOfOpenIds->begin(), listOfOpenIds->end(),
                                msg.getSequentialId()),
                    listOfOpenIds->end());
            } else {
                /* If the message was a request, send a response */
                t_string destinationBlock = msg.getSourceBlock();
                t_string sourceBlock = blockName,
                         messageType = "200";   // 200 is response
                uint8_t sourcePort = 9;         // dummy value
                uint8_t priority = lastId % 5;  // Rotate priority
                t_string messageData =
                    "|DestinationBlock:" + destinationBlock + "|";

                t_handler_message responseMsg;
                responseMsg.setHeader(destinationBlock, sourceBlock, priority,
                                      msg.getSequentialId(), messageType,
                                      sourcePort,
                                      std::to_string(messageData.length()));
                responseMsg.setMessageData(messageData);

                outputS->bufferPut(responseMsg);
            }
        }
    };

    /*!
            Generate new messages based on the *numberOfMessages* argument or until
            the Output Signal is full
        */
    void generateMessages(uint numberOfMessages) {
        auto outputS = outputSignals[0];
        for (uint i = 0; (i < numberOfMessages) && (outputS->space() > 0);
             i++) {
            // Choose a random destination Block of possible Blocks
            t_string destinationBlock = getRandomBlock();
            t_string sourceBlock = blockName,
                     messageType = "100";  // 100 is request

            // Maybe add the sourcePort
            t_string messageData =
                "|DestinationBlock:" + destinationBlock + "|";

            uint8_t sourcePort = 9;         // dummy value
            uint8_t priority = lastId % 6;  // Rotate priority

            t_handler_message msg;
            msg.setHeader(destinationBlock, sourceBlock, priority, lastId,
                          messageType, sourcePort,
                          std::to_string(messageData.length()));
            msg.setMessageData(messageData);

            outputS->bufferPut(msg);

            // Add id to pending requests
            listOfOpenIds->push_back(lastId++);
        }
    };

    uint32_t lastId = 1;
    uint messagesToGenerate = 1;
    std::string blockName = "";

    // List of possible blocks to send data
    std::shared_ptr<std::vector<std::string>> possibleBlocks;

    // Open Messages
    std::shared_ptr<std::vector<uint32_t>> listOfOpenIds;
};
