#include "dv_qkd_message_processor_common_20200819.h"

namespace MessageProcessors {
	/*
	t_message_type getMessageType(const t_message& msg) {
		char mType = msg.messageType[0];
		if (mType == t_message_type::BasisReconciliationMsg) return t_message_type::BasisReconciliationMsg;
		if (mType == t_message_type::ErrorCorrectionMsg) return t_message_type::ErrorCorrectionMsg;
		if (mType == t_message_type::ErrorCorrectionPermutationsMsg) return t_message_type::ErrorCorrectionPermutationsMsg;
		if (mType == t_message_type::PrivacyAmplificationSeedsMsg) return t_message_type::PrivacyAmplificationSeedsMsg;
		if (mType == t_message_type::ParameterEstimationSeedAndBitsMsg) return t_message_type::ParameterEstimationSeedAndBitsMsg;
		if (mType == t_message_type::ToggleBERChangeMsg) return t_message_type::ToggleBERChangeMsg;
		return (t_message_type)0;
	}


	t_message_data getMessageData(const t_message & msg, t_message_data_length dataLength) {
		t_message_type mType = getMessageType(msg);
		std::string mDataString = msg.messageData;
		std::vector <int> mDataVector;
		std::string num{ "" };

		if (t_message_type::ErrorCorrectionPermutationsMsg == mType || t_message_type::PrivacyAmplificationSeedsMsg == mType) {
			for (int k = 0; k < dataLength; k++) {
				std::string data;
				data = mDataString.at(k);
				if (data == "#") {
					mDataVector.push_back(stoi(num));
					num = "";
				}
				else {
					num.append(data);
				}
			}
		}
		else if (t_message_type::ParameterEstimationSeedAndBitsMsg == mType) {
			bool gotSeed = false;

			for (int k = 0; k < dataLength; k++) {
				std::string data;
				data = mDataString.at(k);
				if (!gotSeed) {
					if (data == "#") {
						mDataVector.push_back(stoi(num));
						num = "";
						gotSeed = true;
					}
					else {
						num.append(data);
					}
				}
				else {
					char datac = mDataString.at(k);
					if (datac == '1') {
						mDataVector.push_back(1);
					}
					else if (datac == '0') {
						mDataVector.push_back(0);
					}
					else {
						std::cout << "Error during parameter estimation message reception.\n";
					}
				}
			}
		}
		else {
			for (int k = 0; k < dataLength; k++) {
				char data = mDataString.at(k);

				if (data == '1') {
					mDataVector.push_back(1);
				}
				else if (data == '0') {
					mDataVector.push_back(0);
				}
				else if (data == '2') {
					mDataVector.push_back(2);
				}
				else if (data == '3') {
					mDataVector.push_back(3);
				}
				else if (data == '4') {
					mDataVector.push_back(3);
				}
				else if (data == '5') {
					mDataVector.push_back(5);
				}
				else if (data == '-') {
					char aux_data = mDataString.at((unsigned _int64)k + 1);
					if (aux_data == '2') {
						mDataVector.push_back(-2);
						k++;
					}
					else if (aux_data == '1') {
						mDataVector.push_back(-1);
						k++;
					}
				}

			}
		}
		return mDataVector;
	}
	
	t_message_data_length getMessageDataLength(const t_message & msg) {

		if ((msg.messageDataLength).size() > 0)
			return stoi(msg.messageDataLength);
		else
			return 0;
	}
	*/
	bool sendToMsgProc(std::vector <t_integer>& data, Signal& signalToMsg, bool& started) {
		int space = signalToMsg.space();
		if (space <= 0) return false;

		if (!started) signalToMsg.bufferPut((t_real)data.size());

		if (space <= 0) return false;
		started = true;

		int ready = (int)data.size();
		space = signalToMsg.space();
		int process = std::min(ready, space);

		for (auto k = 0; k < process; ++k) {
			signalToMsg.bufferPut((t_real)data[k]);
		}
		data.erase(data.begin(), data.begin() + process);

		if (data.empty()) {
			started = false;
			return true;
		}
		else {
			return false;
		}
	}

	bool recvFromMsgProc(std::vector <t_integer>& data, Signal& signalToMsg, bool& started, int &numberofValuesToRecv) {
		t_integer ready = signalToMsg.ready();

		t_integer startIdx{ 0 };
		t_real dataIn;
		bool finishedReceiving{ false };
		t_real nValues;
		if (!started) {
			signalToMsg.bufferGet(&nValues);
			numberofValuesToRecv = (int)nValues;
			started = true;
			finishedReceiving = false;
			startIdx = 1;
		}

		for (auto k = startIdx; k < ready; ++k) {
			signalToMsg.bufferGet(&dataIn);
			data.push_back((int)dataIn);
		}
		
		if ((int)data.size() == numberofValuesToRecv) {
			finishedReceiving = true;
			started = false;
		}
		
		return finishedReceiving;
	}
	/*
	std::string generateMessageData(const t_message_data &msgDataVal, const t_message_type &msgType) {
		std::string msgDataString = "";
		int start = 0;

		if (msgType == t_message_type::ParameterEstimationSeedAndBitsMsg) {
			msgDataString.append(std::to_string(msgDataVal[0]));
			msgDataString.append("#");
			start = 1;
			for (unsigned int k = start; k < msgDataVal.size(); ++k) {
				msgDataString.append(std::to_string(msgDataVal[k]));
			}
		}
		else if ((msgType == t_message_type::PrivacyAmplificationSeedsMsg)
			|| (msgType == t_message_type::ErrorCorrectionPermutationsMsg)) {
			for (unsigned int k = 0; k < msgDataVal.size(); ++k) {
				msgDataString.append(std::to_string(msgDataVal[k]));
				msgDataString.append("#");
			}
		}
		else {
			for (unsigned int k = 0; k < msgDataVal.size(); ++k) {
				msgDataString.append(std::to_string(msgDataVal[k]));
			}
		}
		
		return msgDataString;
	}
	*/
}

