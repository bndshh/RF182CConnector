
// Created by b_ndsh_h

#pragma once

#include "cinder/Log.h"

using namespace std;

typedef shared_ptr<class RF182CError> RF182CErrorRef;

class RF182CError : public enable_shared_from_this<RF182CError>
{
public:
	static RF182CErrorRef create(string errorCode);
	virtual const string getCode() { return mCode; };
	virtual const int getCodeInt() { return mCodeInt; };
	virtual const string what() { return mWhat; };
	virtual const bool fine() { return mCode == "0000"; };
	virtual ~RF182CError() = default;

protected:
	RF182CError(string errorCode);
	virtual string parse();
	string mCode;
	int mCodeInt;
	string mWhat;
};