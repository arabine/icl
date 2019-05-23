/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef COUCH_DB_H
#define COUCH_DB_H

#include "Http.h"
#include "JsonReader.h"
#include "JsonWriter.h"

class CouchDb : public JsonValue, public http::IEvent
{

public:
    CouchDb();

    // Raw CouchDB interface, get a document
    void Get(const std::string &url);

    // Work on a document, provide an id
    void Put(const std::string &url);
    void Put(const std::string &url, const std::string &body);

    // Post document, without id
    void Post(const std::string &url, const std::string &body);

    // Delete a document or a database
    void Delete(const std::string &url);

    bool Connect();
    void Close();
    bool IsSuccess();

    // Utility methods
    std::string GetRawData() { return mData; }

private:
    std::string mData;
    http::Connection mConn;
    std::string mLastError;

    // Http::IEvent interface
    virtual void ResponseBegin( const http::Response* r);
    virtual void ResponseData( const http::Response* r, const char* data, int n );
    virtual void ResponseComplete( const http::Response* r );
};

#endif // COUCH_DB_H

//=============================================================================
// End of file CouchDb.h
//=============================================================================
