
// Created by b_ndsh_h

#pragma once

#include "CinderAsio.h"
#include "TcpClient.h"
#include "RF182CHelper.h"

using namespace ci;
using namespace std;

class XMLTag
{
public:
	XMLTag() : startIndex(0), strLength(0), type("") {};
	int startIndex;
	int strLength;
	string type;
	bool equals(const XMLTag &rhs) { return type == rhs.type || strLength == rhs.strLength || startIndex == startIndex; };
};

typedef shared_ptr<class RF182CConnector> RF182CConnectorRef;

class RF182CConnector : public enable_shared_from_this<RF182CConnector>
{
public:
	static RF182CConnectorRef create(asio::io_service &io, string host, int32_t port);
	virtual void connect();
	virtual void close();
	virtual void write(RF182CCommandStr commandStr);
	virtual void read();
	virtual void readTransponderData();
	virtual const string getTransponderData() { return mCurrentTransponderData; };
	virtual const string getCurrentCommandId() { return mCurrentCommandId; };
	virtual void writeTransponderData(const string &data);
	virtual void setStayAlive(const bool &stayAlive = true);
	virtual const bool isConnected();
	virtual const bool isStayAlive();
	virtual const size_t getNumTranspondersInField();
	virtual ~RF182CConnector() = default;

protected:
	RF182CConnector(asio::io_service &io, string host, int32_t port);

	virtual void onConnectResolve();
	virtual void onConnect(TcpSessionRef session);
	virtual void onClose();
	virtual void onError(std::string err, size_t bytesTransferred);
	virtual void onReadComplete();
	virtual void onRead(ci::BufferRef buffer);
	virtual void onWrite(size_t bytesTransferred);
	virtual const bool handleErrorCode(const string &errorCode, const string &type);
	virtual void setPositiveCallback(const std::function<void()> callback) { mPositiveFeedback = callback; };
	virtual void setNegativeCallback(const std::function<void()> callback) { mNegativeFeedback = callback; };

	bool mLog;
	bool mConnected;
	bool mStayAlive;
	bool mWaitForCommandAnswer;
	string	mHost;
	int32_t	mPort;

	size_t mNumTranspondersInField;
	string mCurrentTransponderData;
	string mCurrentCommandId;

	TcpClientRef mClient;
	TcpSessionRef mSession;

	XMLTag firstTag(string response);

	std::function<void()> mPositiveFeedback;
	std::function<void()> mNegativeFeedback;
};

