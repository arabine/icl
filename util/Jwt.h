#ifndef JWT_H
#define JWT_H

#include "ShaTwo.h"
#include "Base64Util.h"
#include "JsonValue.h"
#include "JsonReader.h"
#include "Util.h"
#include "Log.h"
#include "HttpProtocol.h"
#include <string>
#include <regex>

class Jwt
{

public:
    static std::string GenerateJWT(const std::string &payload, const std::string &key)
    {
        std::string header = R"({"alg":"HS256","typ":"JWT"})";

      //  mSessionSecret = "coucou";
        // Compute signature
       // TLogInfo("[HTTP] Header: " + header);
       // TLogInfo("[HTTP] Payload: " + payload);
        std::string input_signature = Base64::Encode(header) + '.' + Base64::Encode(payload);

      //  TLogInfo("[HTTP] Input signature: " + input_signature);
        std::string hmac = Base64::Encode(hmac_compute(key, input_signature));
        //TLogInfo("[HTTP] HMAC-SHA256: " + );

        return input_signature + '.' + hmac;
    }

    static bool CheckJWT(const std::string &header, const std::string &payload, const std::string &hash, const std::string &key, JsonValue &json)
    {
        bool success = false;

        // First, verify signature
        std::string input_signature = header + '.' + payload;

        std::string hmac = Base64::Encode(hmac_compute(key, input_signature));

        if (hmac == hash)
        {
            // Now, check expiration
            std::string decoded = Base64::Decode(payload);
            JsonReader reader;
            if (reader.ParseString(json, decoded))
            {
                if (json.IsObject())
                {
                    uint32_t now = Util::CurrentTimeStamp();
                    uint32_t iat = static_cast<uint32_t>(json.GetObj().GetValue("iat").GetInteger());
                    uint32_t exp = static_cast<uint32_t>(json.GetObj().GetValue("exp").GetInteger());

                    if (iat == exp)
                    {
                        // Authentication is valid forever
                        success = true;
                    }
                    else if (now < exp)
                    {
                        TLogInfo("[JWT] Authentication still valid for: " + std::to_string(exp - now) + " seconds");
                        success = true;
                    }
                }
                else
                {
                    TLogError("[JWT] Not an object");
                }
            }
            else
            {
                TLogError("[JWT] Json parse failure");
            }
        }
        else
        {
            TLogError("[JWT] Bad signature");
        }

        return success;
    }

    static bool CheckRequest(const HttpRequest &request, const std::string &key)
    {
        bool success = false;

        // Must be a token
        std::map<std::string, std::string>::const_iterator auth = request.headers.find("authorization");
        if (auth != request.headers.end() )
        {
            // extract token
            TLogInfo("[HTTP] Authorization: " + auth->second);
            std::string token = Match(auth->second, "Bearer (.*)");

            success = CheckJwtStringToken(token, key);
        }

        return success;
    }

    static bool CheckJwtStringToken(const std::string &token, const std::string &key)
    {
        bool valid = false;
        std::vector<std::string> parts = Util::Split(token, ".");

        if (parts.size() == 3)
        {
            JsonValue json;
            valid = CheckJWT(parts[0], parts[1], parts[2], key, json);
        }
        return valid;
    }

    static std::string Match(const std::string &msg, const std::string &patternString)
    {
        std::regex pattern(patternString);
        std::smatch matcher;
        std::string subMatch;

        std::regex_search(msg, matcher, pattern);

        if (matcher.size() == 2)
        {
            // Extracted value is located at index 1
            subMatch = matcher[1].str();
        }
        return subMatch; // empty string if not found
    }

};

#endif // JWT_H
