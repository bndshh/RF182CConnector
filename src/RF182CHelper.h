
// Created by b_ndsh_h

#pragma once

#include "cinder/Xml.h"
#include "cinder/Log.h"

#include "RF182CError.h"

using namespace std;
using namespace ci;

typedef string RF182CCommandStr;

class RF182CCommand
{
public:
	RF182CCommand() {};
	
	enum Type
	{
		none,				// Command not defined
		comDevSetConfig,	// Optional: RF182C umprojektieren		
		reset,				// RF182C und Reader eines Kanals zurücksetzen und parametrieren		     
		writeTagData,		// Schreiben auf Adresse des Tags     		
		readTagData,		// Lesen von Adresse des Tags	
		initializeTag,		// Tag initialisieren		
		getReaderStatus,	// Status der angeschlossenen Reader		
		setAnt,				// Antenne Ein-Aus	
		heartbeat,			// Leitungsüberwachung	
		getTagStatus		// Status des Tag
	};

	static string mReaderStatus[7];

	static vector<Type> AllTypes();
	static vector<string> AllTypesString();

	static string hex4(const int &value);
	static string hex4(const string &value);

	static RF182CCommandStr resetCommand(const bool &resetLED = false);
	static RF182CCommandStr writeTagDataCommand(const int &address, const string &hexData);
	static RF182CCommandStr readTagDataCommand(const int &address, const int &dataLength);
	static RF182CCommandStr initializeTagCommand(const int &memorySize, const int &defaultValue);
	static RF182CCommandStr getReaderStatusCommand(const int &mode);
	static RF182CCommandStr heartbeatCommand();
	static RF182CCommandStr setAntCommand(const bool &on);

	virtual ~RF182CCommand() = default;

protected:
	static XmlTree create();
};

class RF182CAnswer
{
public:
	RF182CAnswer() : mValid(false), mLog(false) {};
	bool mValid;
	bool mLog;
	virtual const string toString() = 0;
	virtual void parse(const string &str) = 0;
	virtual ~RF182CAnswer() = default;
};

class RF182CReply : public RF182CAnswer
{
public:
	RF182CReply(string str = "");
	virtual const bool fine();
	virtual const string toString() override;
	virtual void parse(const string &str) override;
	virtual const string getName() { return NAME; };
	virtual const string getResultCode() { return mResultCode; };
	virtual const RF182CCommand::Type getCommandType() { return mCommandType; };
	virtual const string getData() { return mData; };
	const static bool fastCheck(string str) { return str.find(NAME) != string::npos; };
	virtual ~RF182CReply() = default;

protected:
	const static string NAME;
	string mResultCode;
	RF182CCommand::Type mCommandType;
	string mData;
};

class RF182CNotification : public RF182CAnswer
{
public:
	RF182CNotification(string str = "");
	virtual const string toString() override;
	virtual void parse(const string &str) override;
	virtual const string getName() { return NAME; };
	virtual const string getId() { return mId; };
	virtual const string getOrigin() { return mOrigin; };
	virtual const int getTagCount() { return mTagCount; };
	const static bool fastCheck(string str) { return str.find(NAME) != string::npos; };
	virtual ~RF182CNotification() = default;

protected:
	const static string NAME;
	string mId;
	string mOrigin;
	int mTagCount;
};

class RF182CAlarm : public RF182CAnswer
{
public:
	RF182CAlarm(string str = "");
	virtual const bool fine();
	virtual const string toString() override;
	virtual void parse(const string &str) override;
	virtual const string getName() { return NAME; };
	virtual const string getId() { return mId; };
	virtual const string getOrigin() { return mOrigin; };
	virtual const string getDeviceName() { return mDeviceName; };
	virtual const long getDeviceTime() { return mDeviceTime; };
	virtual const string getErrorcode() { return mErrorcode; };
	const static bool fastCheck(string str) { return str.find(NAME) != string::npos; };
	virtual ~RF182CAlarm() = default;

protected:
	const static string NAME;
	string mId;
	string mOrigin;
	string mDeviceName;
	long mDeviceTime;
	string mErrorcode;
};