
#include "RF182CConnector.h"

RF182CConnectorRef RF182CConnector::create(asio::io_service & io, string host, int32_t port)
{
	return RF182CConnectorRef(new RF182CConnector(io, host, port))->shared_from_this();
}

RF182CConnector::RF182CConnector(asio::io_service &io, string host, int32_t port) : mHost(host), mPort(port), mLog(false), mConnected(false), mWaitForCommandAnswer(false), mNumTranspondersInField(0), mCurrentTransponderData(""), mCurrentCommandId("")
{
	mClient = TcpClient::create(io);

	mClient->connectResolveEventHandler([] {});
	mClient->connectConnectEventHandler(&RF182CConnector::onConnect, this);
	mClient->connectErrorEventHandler(&RF182CConnector::onError, this);

	mFeedbackSoundPositiv = loadAudioFromAssets("Sounds/positiv_rfid_speichern.mp3");
	mFeedbackSoundNegativ = loadAudioFromAssets("Sounds/negativ_rfid_speichern.mp3");
}

void RF182CConnector::connect()
{
	if (mLog) CI_LOG_I("Connecting to: " + mHost + ":" + toString(mPort));
	mClient->connect(mHost, mPort);
}

void RF182CConnector::close()
{
	mSession->close();
}

void RF182CConnector::write(RF182CCommandStr commandStr)
{
	if (mWaitForCommandAnswer) return;

	if (mSession && mSession->getSocket()->is_open()) 
	{
		if (mLog) CI_LOG_I("Write: " + commandStr);
		mSession->write(TcpSession::stringToBuffer(commandStr));
		mWaitForCommandAnswer = true;
	}
	else 
	{
		connect();
	}
}

void RF182CConnector::read()
{
	if (mSession && mSession->getSocket()->is_open())
	{
		mSession->read();
	}
}

void RF182CConnector::onConnectResolve()
{
	if (mLog) CI_LOG_I("Endpoint resolved");
}

void RF182CConnector::onConnect(TcpSessionRef session)
{
	mSession = session;

	mSession->connectCloseEventHandler(&RF182CConnector::onClose, this);
	mSession->connectErrorEventHandler(&RF182CConnector::onError, this);
	mSession->connectReadCompleteEventHandler(&RF182CConnector::onReadComplete, this);
	mSession->connectReadEventHandler(&RF182CConnector::onRead, this);
	mSession->connectWriteEventHandler(&RF182CConnector::onWrite, this);

	mConnected = true;

	write(RF182CCommand::resetCommand());
}

void RF182CConnector::onClose()
{
	mConnected = false;
}

void RF182CConnector::onError(std::string err, size_t bytesTransferred)
{
	string text = "Error";
	if (!err.empty())  text += " : " + err;
	CI_LOG_E(text);
}

void RF182CConnector::onReadComplete()
{
	if (mLog) CI_LOG_I("Read complete");
}

void RF182CConnector::onRead(ci::BufferRef buffer)
{
	string response = TcpSession::bufferToString(buffer);

	if (mLog) CI_LOG_I("Response : " + response + "(" + toString(response.size()) + " bytes read)");

	if (!isStayAlive()) mSession->close();

	XMLTag tag = firstTag(response);
	if (tag.type != "" && tag.strLength > 0)
	{
		if (RF182CReply::fastCheck(response))
		{
			auto reply = RF182CReply(response);
			mWaitForCommandAnswer = false;

			auto fine = handleErrorCode(reply.getResultCode(), reply.getName());

			if (reply.getCommandType() == RF182CCommand::readTagData)
			{
				auto data = reply.getData();
				if (data != "") mCurrentTransponderData = data;
			}
			else if (reply.getCommandType() == RF182CCommand::writeTagData)
			{
				fine ? mFeedbackSoundPositiv->start() : mFeedbackSoundNegativ->start();
			}

		}
		else if (RF182CNotification::fastCheck(response))
		{
			auto notification = RF182CNotification(response);

			mNumTranspondersInField = notification.getTagCount();

			if (!mNumTranspondersInField) mCurrentTransponderData = "";

			mCurrentCommandId = notification.getId();
		}
		else if (RF182CAlarm::fastCheck(response))
		{
			auto alarm = RF182CAlarm(response);
			handleErrorCode(alarm.getErrorcode(), alarm.getName());
		}
	}
}

const bool RF182CConnector::handleErrorCode(const string &errorCode, const string &type)
{
	auto error = RF182CError::create(errorCode);
	auto fine = error->fine();
	if (!fine) CI_LOG_E(type + " with error-code: " + error->getCode() + " (\"" + error->what() + "\")");
	return fine;
}

void RF182CConnector::readTransponderData()
{
	write(RF182CCommand::readTagDataCommand(0, 2));
}

void RF182CConnector::writeTransponderData(const string &data)
{
	write(RF182CCommand::writeTagDataCommand(0, data));
}

XMLTag RF182CConnector::firstTag(string response)
{
	int index1 = response.find("<reply>");
	int index2 = response.find("<notification>");
	int index3 = response.find("<alarm>");

	if (index1 == -1 && index2 == -1 && index3 == -1) return XMLTag(); 
	if (index1 == -1) index1 = INT_MAX;
	if (index2 == -1) index2 = INT_MAX;
	if (index3 == -1) index3 = INT_MAX;

	string endTag = "</alarm>";
	XMLTag tag = XMLTag();
	tag.type = "alarm";
	tag.startIndex = index3;

	if (index1 < index2 && index1 < index3)
	{
		endTag = "</reply>";
		tag.type = "reply";
		tag.startIndex = index1;
	}
	else if (index2 < index3)
	{
		endTag = "</notification>";
		tag.type = "notification";
		tag.startIndex = index2;
	}

	int endIndex = response.find_last_of(endTag);
	if (endIndex == -1) return XMLTag();
	tag.strLength = (int) (endIndex - tag.startIndex + endTag.length());
	return tag;
}

void RF182CConnector::onWrite(size_t bytesTransferred)
{
	if (mLog) CI_LOG_I(toString(bytesTransferred) + " bytes written");
}

void RF182CConnector::setStayAlive(const bool &stayAlive)
{
	mStayAlive = stayAlive;
}

const bool RF182CConnector::isConnected()
{
	return mConnected;
}

const bool RF182CConnector::isStayAlive()
{
	return mStayAlive;
}
const size_t RF182CConnector::getNumTranspondersInField()
{
	return mNumTranspondersInField;
}

