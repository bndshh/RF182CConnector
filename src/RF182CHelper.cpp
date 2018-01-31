
// Created by b_ndsh_h

#include "RF182CHelper.h"

#include <iomanip>

vector<RF182CCommand::Type> RF182CCommand::AllTypes()
{
	std::vector<Type> allTypes;

	allTypes.push_back(none);
	allTypes.push_back(comDevSetConfig);
	allTypes.push_back(reset);
	allTypes.push_back(writeTagData);
	allTypes.push_back(readTagData);
	allTypes.push_back(initializeTag);
	allTypes.push_back(getReaderStatus);
	allTypes.push_back(setAnt);
	allTypes.push_back(heartbeat);
	allTypes.push_back(getTagStatus);

	return allTypes;
}

vector<string> RF182CCommand::AllTypesString()
{
	std::vector<string> allTypes;

	allTypes.push_back("none");
	allTypes.push_back("comDevSetConfig");
	allTypes.push_back("reset");
	allTypes.push_back("writeTagData");
	allTypes.push_back("readTagData");
	allTypes.push_back("initializeTag");
	allTypes.push_back("getReaderStatus");
	allTypes.push_back("setAnt");
	allTypes.push_back("heartbeat");
	allTypes.push_back("getTagStatus");

	return allTypes;
}

string RF182CCommand::hex4(const int &value)
{
	stringstream stream;
	stream << uppercase << hex << setw(4) << setfill('0') << value;
	return stream.str();
}

string RF182CCommand::hex4(const string &value)
{
	stringstream stream;
	stream << uppercase << setw(4) << setfill('0') << value;
	return stream.str();
}

XmlTree RF182CCommand::create()
{
	auto xml = XmlTree::createDoc();
	return XmlTree("command", "");
}

string RF182CCommand::mReaderStatus[7] =
{
	"01", // = Reader - Status
	"02", // = MOBY U(SLG - Diagnose I, Funktionsaufrufe)
	"03", // = MOBY U(SLG - Diagnose II, Fehlermeldungen)
	"04", // = MOBY U(SLG - Diagnose III, identifizierte MDS)
	"05", // = MOBY U(SLG - Diagnose IV, Kommunikationsgüte)
	"06", // = RF300 Reader - Diganose
	"07", // = RF600 Reader - Diagnose
};

RF182CCommandStr RF182CCommand::resetCommand(const bool &resetLED)
{
	auto xml = create();
	auto node = XmlTree(AllTypesString().at(Type::reset), "");

	string param = "00";				// standby - Not used by RF300
	param += "25";						// Param == 001 (000 == Keine ANW-Kontroll, 001 == keine MDS-Steuerung; ANW-Kontrolle über Firmware) 0 0101 (0101 == 5 -> RF300 Without Multitag)
	param += resetLED ? "02" : "00";    // 00 == ERR-LED am SLG nicht zurücksetzen || 02 == ERR-LED am SLG zurücksetzen
	param += "00";						// distance_limiting - Not used by RF300
	param += "0001";					// Number of Allowed Tags == No Mulittag 
	param += "00";					    // field_ON_control  - Not used by RF300 (00hex)
	param += "01";						// field_ON_time // herstellerunabhängiger tag

	node.push_back(XmlTree("param", param));

	xml.push_back(node);
	return toString(xml);
}

RF182CCommandStr RF182CCommand::writeTagDataCommand(const int &address, const string &hexData)
{
	if (address < 0 || address > SHRT_MAX) return string();
	if (hexData.length() < 0 || hexData.length() > 4) return string();

	auto xml = create();

	auto node = XmlTree(AllTypesString().at(Type::writeTagData), "");
	node.push_back(XmlTree("startAddress", hex4(address)));
	node.push_back(XmlTree("data", hex4(hexData)));
	xml.push_back(node);

	return toString(xml);
}

RF182CCommandStr RF182CCommand::readTagDataCommand(const int &address, const int &dataLength)
{
	if (address < 0 || address > SHRT_MAX) return string();
	if (dataLength < 0 || dataLength > 255) return string();

	auto xml = create();

	auto node = XmlTree(AllTypesString().at(Type::readTagData), "");
	node.push_back(XmlTree("startAddress", hex4(address)));
	node.push_back(XmlTree("dataLength", hex4(dataLength)));
	xml.push_back(node);

	return toString(xml);
}

RF182CCommandStr RF182CCommand::initializeTagCommand(const int &memorySize, const int &defaultValue)
{
	if (memorySize < 0 || memorySize > 65535) return string();
	if (defaultValue < 0 || defaultValue > 255) return string();

	auto xml = create();

	auto node = XmlTree(AllTypesString().at(Type::initializeTag), "");
	node.push_back(XmlTree("memorySize", hex4(memorySize)));
	node.push_back(XmlTree("defaultValue", hex4(defaultValue)));
	xml.push_back(node);

	return toString(xml);
}

RF182CCommandStr RF182CCommand::getReaderStatusCommand(const int &mode)
{
	if (mode < 0 || mode > 7 - 1) return string();

	auto xml = create();
	auto node = XmlTree(AllTypesString().at(Type::getReaderStatus), "");
	node.push_back(XmlTree("mode", mReaderStatus[mode]));
	xml.push_back(node);
	return toString(xml);
}

RF182CCommandStr RF182CCommand::heartbeatCommand()
{
	auto xml = create();
	xml.push_back(XmlTree(AllTypesString().at(Type::heartbeat), ""));
	return toString(xml);
}

RF182CCommandStr RF182CCommand::setAntCommand(const bool &on)
{	
	auto xml = create();
	auto node = XmlTree(AllTypesString().at(Type::setAnt), "");
	node.push_back(XmlTree("mode", on ? "01" : "02"));
	xml.push_back(node);
	return toString(xml);
}

const string RF182CReply::NAME = "reply";

RF182CReply::RF182CReply(string str) : RF182CAnswer(), mResultCode(""), mCommandType(RF182CCommand::Type::none), mData("")
{
	if (str != "") parse(str);
}

const bool RF182CReply::fine()
{
	return mResultCode == "0000";
}

const string RF182CReply::toString()
{
	return "RF182CReply : { valid : " + ci::toString(mValid) + ", resultCode : " + mResultCode + ", command : " + RF182CCommand::AllTypesString().at(mCommandType) + ", data : " + mData + " }";
}

void RF182CReply::parse(const string &str)
{
	auto xml = XmlTree(str);
	XmlTree node = xml.getChild(NAME);

	if (node.getTag() == NAME)
	{
		for each(auto &child in node.getChildren())
		{
			string tag = child->getTag();
			string value = child->getValue();

			if (tag == "resultCode")
			{
				mResultCode = value;
			}
			else
			{
				for each(auto type in RF182CCommand::AllTypes())
				{
					if (RF182CCommand::AllTypesString().at(type) == tag)
					{
						mCommandType = type;

						try
						{
							mData = child->getChild("returnValue").getChild("data").getValue();
						}
						catch (const XmlTree::ExcChildNotFound &exc)
						{
							if (mLog) CI_LOG_W(exc.what());
						}

						break;
					}
				}
			}
		}

		mValid = mResultCode != "" && mCommandType != RF182CCommand::Type::none;
	}
}

const string RF182CNotification::NAME = "notification";

RF182CNotification::RF182CNotification(string str) : RF182CAnswer(), mId(""), mOrigin(""), mTagCount(-1)
{
	if (str != "") parse(str);
}

const string RF182CNotification::toString()
{
	return "RF182CNotification : { valid : " + ci::toString(mValid) + ", id : " + mId + ", origin : " + mOrigin + ", tagCount : " + ci::toString(mTagCount) + " }";
}

void RF182CNotification::parse(const string &str)
{
	auto xml = XmlTree(str);
	XmlTree node = xml.getChild(NAME);

	if (node.getTag() == NAME)
	{
		for each(auto &child in node.getChildren())
		{
			string tag = child->getTag();
			string value = child->getValue();

			if (tag == "id")
			{
				mId = value;
			}
			else if (tag == "origin")
			{
				mOrigin = value;
			}
			else if (tag == "tagPresent")
			{
				try
				{
					unsigned int x;
					std::stringstream ss;
					ss << std::hex << child->getChild("tagCount").getValue();
					ss >> x;

					mTagCount = static_cast<int>(x);
				}
				catch (const XmlTree::ExcChildNotFound &exc)
				{
					if (mLog) CI_LOG_W(exc.what());
				}
			}
		}

		mValid = mId != "" && mOrigin != "" && (mTagCount > 0);
	}
}

const string RF182CAlarm::NAME = "alarm";

RF182CAlarm::RF182CAlarm(string str) : RF182CAnswer(), mId(""), mOrigin(""), mDeviceName(""), mDeviceTime(-1), mErrorcode("")
{
	if (str != "") parse(str);
}

const bool RF182CAlarm::fine()
{
	return mErrorcode == "0000";
}

const string RF182CAlarm::toString()
{
	return "RF182CAlarm : { valid : " + ci::toString(mValid) + ", id : " + mId + ", origin : " + mOrigin + ", deviceName : " + mDeviceName + ", deviceTime : " + ci::toString(mDeviceTime) + ", errorcode : " + mErrorcode + " }";
}

void RF182CAlarm::parse(const string & str)
{
	auto xml = XmlTree(str);
	XmlTree node = xml.getChild(NAME);

	if (node.getTag() == NAME)
	{
		for each(auto &child in node.getChildren())
		{
			string tag = child->getTag();
			string value = child->getValue();

			if (tag == "id")
			{
				mId = value;
			}
			else if (tag == "origin")
			{
				mOrigin = value;
			}
			else if (tag == "deviceName")
			{
				mDeviceName = value;
			}
			else if (tag == "deviceTime")
			{
				unsigned int x;
				std::stringstream ss;
				ss << std::hex << value;
				ss >> x;

				mDeviceTime = static_cast<int>(x);
			}
			else if (tag == "content")
			{
				try
				{
					mErrorcode = child->getChild("code").getValue();
				}
				catch (const XmlTree::ExcChildNotFound &exc)
				{
					if (mLog) CI_LOG_W(exc.what());
				}
			}
		}

		mValid = mId != "" && mOrigin != "" && mDeviceName != "" && (mDeviceTime > 0) && mErrorcode != "";
	}
}