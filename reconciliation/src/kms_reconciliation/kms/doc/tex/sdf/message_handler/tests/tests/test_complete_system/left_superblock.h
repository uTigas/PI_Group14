// Import of standard or provided libraries
#include <memory>

// Import of local libraries
#include "dummy_block.h"
#include "message_handler.h"

class LeftSuperBlock : SuperBlock {
   public:
    LeftSuperBlock(std::initializer_list<Signal*> rightToLeft,
                   std::initializer_list<Signal*> leftToRight)
        : SuperBlock(leftToRight, rightToLeft){};

    void initialize(void) {
        /* Initialize Destination Translation Table for Tx 
            translationTableForTx.add("parameter_estimation_left", 0);   // exitPort0 - stay in left superblock
            translationTableForTx.add("privacy_amplification_left", 0);  // exitPort0 - stay in left superblock
            translationTableForTx.add("error_correction_left", 0);       // exitPort0 - stay in left superblock
            translationTableForTx.add("frame_synchronization_left", 0);  // exitPort0 - stay in left superblock
            translationTableForTx.add("parameter_estimation_right", 1);   // exitPort1 - go to right superblock
            translationTableForTx.add("privacy_amplification_right", 1);  // exitPort1 - go to right superblock
            translationTableForTx.add("error_correction_right", 1);       // exitPort1 - go to right superblock
            translationTableForTx.add("frame_synchronization_right", 1);  // exitPort1 - go to right superblock
            */
        /* Initialize Destination Translation Table for Rx 
            translationTableForRx.add("parameter_estimation_left", 0);   // exitPort0 - stay in left superblock
            translationTableForRx.add("privacy_amplification_left", 1);  // exitPort1 - stay in left superblock
            translationTableForRx.add("error_correction_left", 2);       // exitPort2 - stay in left superblock
            translationTableForRx.add("frame_synchronization_left", 3);  // exitPort3 - stay in left superblock
            */

        // Setup System Block
        setSuperBlockSystem({
            &parameterEstimationBlock,
            &PrivacyAmplificationBlock,
            &ErrorCorrectionBlock,
            &FrameSyncBlock,
            &txMessageHandler,
            &rxMessageHandler,
        });

        //setLogValue(logValue);
        setLogFileName(logFileName);
        setSignalsFolderName(signalsFolderName);

        /* ################# Initialize Dummy Blocks ################# */
        for (uint32_t i = 0; i < dummyBlocks.size(); i++) {
            dummyBlocks[i]->initialize();
        }

        /* ################# Initialize Message Handlers ################# */
        txMessageHandler.initialize();
        rxMessageHandler.initialize();
    };

    bool runBlock(void) {

        /* ################# Run Dummy Blocks ################# */
        for (uint32_t i = 0; i < dummyBlocks.size(); i++) {
            dummyBlocks[i]->setMessagesToGenerate(numberOfMessagesToGenerate);
            dummyBlocks[i]->runBlock();
        }

        /* ################# Run Message Handlers ################# */
        txMessageHandler.runBlock();
        rxMessageHandler.runBlock();

        return true;
    };

    void sendSignalToTerminate(void) {
        /* ################# Send Signal to Terminate Dummy Blocks ################# */
        for (uint32_t i = 0; i < dummyBlocks.size(); i++) {
            dummyBlocks[i]->setMessagesToGenerate(0);
        }
        setNumberOfMessagesToGenerate(0);
    }

    void terminate(void) {
        /* ################# Terminate the Dummy Blocks ################# */
        for (uint32_t i = 0; i < dummyBlocks.size(); i++) {
            dummyBlocks[i]->terminate();
        }

        /* ################# Terminate the Message Handlers ################# */
        txMessageHandler.terminate();
        rxMessageHandler.terminate();
    };

    void setNumberOfMessagesToGenerate(unsigned int number) {
        numberOfMessagesToGenerate = number;
    };

    unsigned int getNumberOfMessagesToGenerate(void) {
        return numberOfMessagesToGenerate;
    };

   private:
    unsigned long int signalBuffSize = 10;
    unsigned int numberOfMessagesToGenerate = 1;
    std::string signalsFolderName{"signals/LeftSuperBlock"};
    std::string logFileName{"LeftSuperBlock.txt"};
    std::vector<std::string> allBlocksList{
        "parameter_estimation_left",  "privacy_amplification_left",
        "error_correction_left",      "frame_synchronization_left",
        "parameter_estimation_right", "privacy_amplification_right",
        "error_correction_right",     "frame_synchronization_right"};
    std::vector<DummyBlock*> dummyBlocks{
        &parameterEstimationBlock,
        &PrivacyAmplificationBlock,
        &ErrorCorrectionBlock,
        &FrameSyncBlock,
    };

    /* ################# Declaration of Translations for Destination Table ################# */
    std::map<t_string, t_integer> mapTx{
        {"parameter_estimation_left",
         0},  // exitPort0 - stay in left superblock
        {"privacy_amplification_left",
         0},                           // exitPort0 - stay in left superblock
        {"error_correction_left", 0},  // exitPort0 - stay in left superblock
        {"frame_synchronization_left",
         0},  // exitPort0 - stay in left superblock
        {"parameter_estimation_right",
         1},  // exitPort1 - go to right superblock
        {"privacy_amplification_right",
         1},                            // exitPort1 - go to right superblock
        {"error_correction_right", 1},  // exitPort1 - go to right superblock
        {"frame_synchronization_right",
         1}  // exitPort1 - go to right superblock
    };
    std::map<t_string, t_integer> mapRx{
        {"parameter_estimation_left",
         0},  // exitPort0 - stay in left superblock
        {"privacy_amplification_left",
         1},                           // exitPort1 - stay in left superblock
        {"error_correction_left", 2},  // exitPort2 - stay in left superblock
        {"frame_synchronization_left",
         3}  // exitPort3 - stay in left superblock
    };
    // Create translation table
    DestinationTranslationTable translationTableForTx{mapTx};
    DestinationTranslationTable translationTableForRx{mapRx};

    /* ################# Declaration of Signals ################# */
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // Signals arriving to Tx Message Handler
    HandlerMessage parameterEstimationTx{"parameterEstimationTx.sgn",
                                         signalBuffSize, hType, sWriteMode};
    HandlerMessage PrivacyAmplificationTx{"PrivacyAmplificationTx.sgn",
                                          signalBuffSize, hType, sWriteMode};
    HandlerMessage ErrorCorrectionTx{"ErrorCorrectionTx.sgn", signalBuffSize,
                                     hType, sWriteMode};
    HandlerMessage FrameSyncTx{"FrameSyncTx.sgn", signalBuffSize, hType,
                               sWriteMode};

    // Inter-Message Handler signal (Connects to Tx and Rx)
    HandlerMessage MessageToMessageHandler{"MessageToMessageHandler.sgn",
                                           signalBuffSize, hType, sWriteMode};

    // Signals departing from Rx Message Handler
    HandlerMessage parameterEstimationRx{"parameterEstimationRx.sgn",
                                         signalBuffSize, hType, sWriteMode};
    HandlerMessage PrivacyAmplificationRx{"PrivacyAmplificationRx.sgn",
                                          signalBuffSize, hType, sWriteMode};
    HandlerMessage ErrorCorrectionRx{"ErrorCorrectionRx.sgn", signalBuffSize,
                                     hType, sWriteMode};
    HandlerMessage FrameSyncRx{"FrameSyncRx.sgn", signalBuffSize, hType,
                               sWriteMode};

    /* ################# Declaration of Blocks ################# */
    DummyBlock parameterEstimationBlock{{&parameterEstimationRx},
                                        {&parameterEstimationTx},
                                        "parameter_estimation_left",
                                        allBlocksList};
    DummyBlock PrivacyAmplificationBlock{{&PrivacyAmplificationRx},
                                         {&PrivacyAmplificationTx},
                                         "privacy_amplification_left",
                                         allBlocksList};
    DummyBlock ErrorCorrectionBlock{{&ErrorCorrectionRx},
                                    {&ErrorCorrectionTx},
                                    "error_correction_left",
                                    allBlocksList};
    DummyBlock FrameSyncBlock{{&FrameSyncRx},
                              {&FrameSyncTx},
                              "frame_synchronization_left",
                              allBlocksList};

    // Input and Output Signals declared from left to right
    MessageHandler txMessageHandler{
        {&parameterEstimationTx, &PrivacyAmplificationTx, &ErrorCorrectionTx,
         &FrameSyncTx},
        {&MessageToMessageHandler, outputSignals[0]},
        translationTableForTx};
    MessageHandler rxMessageHandler{
        {&MessageToMessageHandler, inputSignals[0]},
        {&parameterEstimationRx, &PrivacyAmplificationRx, &ErrorCorrectionRx,
         &FrameSyncRx},
        translationTableForRx};
};
