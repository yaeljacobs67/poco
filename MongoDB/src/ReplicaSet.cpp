//
// ReplicaSet.cpp
//
// $Id$
//
// Library: MongoDB
// Package: MongoDB
// Module:  ReplicaSet
//
// Implementation of the ReplicaSet class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
#include "Poco/MongoDB/ReplicaSet.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/MongoDB/ResponseMessage.h"

namespace Poco
{
namespace MongoDB
{

ReplicaSet::ReplicaSet(const std::vector<Net::SocketAddress> &addresses) : _addresses(addresses)
{
}

ReplicaSet::~ReplicaSet()
{
}


Connection::Ptr ReplicaSet::findMaster()
{
	Connection::Ptr master;

	for(std::vector<Net::SocketAddress>::iterator it = _addresses.begin(); it != _addresses.end(); ++it)
	{
		master = isMaster(*it);
		if ( ! master.isNull() )
		{
			break;
		}
	}

	return master;
}

Connection::Ptr ReplicaSet::isMaster(const Net::SocketAddress& address)
{
	Connection::Ptr conn = new Connection();

	try
	{
		conn->connect(address);

		QueryRequest request("admin.$cmd");
		request.numberToReturn(1);
		request.query().add("isMaster", 1);

		ResponseMessage response;
		conn->sendRequest(request, response);

		if ( response.documents().size() > 0 )
		{
			Document::Ptr doc = response.documents()[0];
			if ( doc->get<bool>("ismaster") )
			{
				return conn;
			}
			else if ( doc->exists("primary") )
			{
				return isMaster(Net::SocketAddress(doc->get<std::string>("primary")));
			}
		}
	}
	catch(...)
	{
		conn = NULL;
	}
	
	return NULL; 
}


}} // Poco::MongoDB
