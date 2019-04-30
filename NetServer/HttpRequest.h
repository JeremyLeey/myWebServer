#pragma once

#include "base/noncopyable.h"
#include <map>
#include <assert.h>
#include <stdio.h>


class HttpRequest{
public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
        kUnknown, kHttp10, kHttp11
    };
    HttpRequest()
     :  method_(kInvalid),
        version_(kUnknown)
    {
        // do nothing
    }

    void setVersion(Version v) {
        version_ = v;
    }
    Version getVersion() const {
        return version_;
    }

    bool setMethod(const char* start, const char* end) {
        assert(method_ == kInvalid);
        string m(start, end);
        if (m == "GET") {
            method_ = kGet;
        }
        else if (m == "POST") {
            method_ = kPost;
        }
        else if (m == "HEAD") {
            method_ = kHead;
        }
        else if (m == "PUT") {
            method_ = kPut;
        }
        else if (m == "DELETE") {
            method_ = kDelete;
        }
        else {
            method_ = kInvalid;
        }

        return method_ != kInvalid;
    }

    Method method() const {
        return method_;
    }

    const char* methodString() const {
        const char* result = "UNKNOW";
        switch(method_) {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void addHeader(const char* start, const char* colon, const char* end) {
        string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon)) ++colon;
        string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1])) {
            values.resize(value.size()-1);
        }
        headers_[field] = value;
    }


    
    string getHeader(const string& field) const {
        string result;
        std::map<string, string>::const iterator = headers_.find(field);
        if (it != headers.end()) {
            result = it->second;
        }
        return result;
    }
    const std::map<string, string>& headers() const {
        return headers_;
    }

private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    std::map<string, string> headers_;
};

