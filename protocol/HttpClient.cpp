#include "HttpClient.h"

#include "Util.h"
#include <iostream>

HttpClient::HttpClient()
{

}

/*

                if (mCurrentChunkSize > 0)
                {
                    if (std::getline(iss, line))
                    {
                        mCurrentChunk = line;
                        std::cout << "Body: "<< line << "Size: " << line.size() << std::endl;
                        mCurrentChunkSize -= line.size();
                    }
                }
                else
                {
                    mGetFinished = true;
                    mRequestSuccess = true;
                }

*/
bool HttpClient::ParseChunkSize(const std::string &data)
{
    bool success = false;
    std::istringstream iss(data);
    std::string line;
    if (std::getline(iss, line))
    {
        if (line.size() > 0)
        {
            if (line.back() == '\r')
            {
                line.pop_back(); // remove \r

                std::stringstream converter;
                converter << std::hex << line;

                if (converter.good())
                {
                    converter >> mCurrentChunkSize;
                    std::cout << "Chunked size: "<< mCurrentChunkSize << std::endl;
                    success = true;
                }

                uint32_t body_start = static_cast<uint32_t>(iss.tellg());
                if (body_start < data.length())
                {
                    mCurrentChunk = data.substr(body_start);
                }
            }
        }
    }
    return success;
}


bool HttpClient::Get(const std::string &host, const std::string &path, uint16_t port, std::string &response)
{
    request.method = "GET";
    request.protocol = "HTTP/1.1";
    request.query = path;
    request.headers["Host"] = host;
    request.headers["Content-type"] = "application/json";

    client.Initialize();
    if (client.Connect(host, port))
    {
        if (client.Send(proto.GenerateRequest(request)))
        {
            mFirstPacket = false;
            mGetFinished = false;

            std::string output;
            std::string remaining;
            mCurrentChunkSize = -1; // unknown

            do
            {
                if (client.RecvWithTimeout(output, 100*1024, 5000))
                {
                    if (!mFirstPacket)
                    {
                        mFirstPacket = true;
                        proto.ParseReplyHeaders(output, mReply);

                        if (mReply.body.size() == mReply.contentLength)
                        {
                            mGetFinished = true;
                            mRequestSuccess = true;
                        }
                        else
                        {
                            output = mReply.body;
                            mCurrentChunkSize = -1;
                        }
                    }

                    if (mReply.chunked)
                    {
                        mCurrentChunk += output;

                        bool moreData = true;
                        do
                        {
                            // Si on n'a pas d'information sur le chunk en cours, on parse:
                            if (mCurrentChunkSize == -1)
                            {
                                if (mCurrentChunk.size() > 3)
                                {
                                    // le prochain paquet doit être un début de chunk
                                    if (!ParseChunkSize(mCurrentChunk))
                                    {
                                        mGetFinished = true;
                                        mRequestSuccess = false;
                                        moreData = false;
                                    }
                                }
                                else
                                {
                                    moreData = false; // pas assez de données
                                }
                            }

                            std::cout << "\r\n------------------------------\r\nReste: "<< mCurrentChunkSize << "\r\nChunk: " << mCurrentChunk << "\r\n------------------------------\r\n" << std::endl;
                            if (mCurrentChunkSize == 0)
                            {
                                mGetFinished = true;
                                mRequestSuccess = true;
                                moreData = false;
                            }
                            else if (mCurrentChunk.size() >= static_cast<uint32_t>(mCurrentChunkSize))
                            {
                                // On a reçu normalement suffisamment de données (voire plus)
                                response.append(mCurrentChunk, 0, mCurrentChunkSize);
                                mCurrentChunk.erase(0, mCurrentChunkSize);

                                if (mCurrentChunk.size() >= 2)
                                {
                                    if ((mCurrentChunk[0] == '\r') && (mCurrentChunk[1] == '\n'))
                                    {
                                        mCurrentChunk.erase(0, 2);
                                        mCurrentChunkSize = -1;
                                    }
                                    else
                                    {
                                        // erreur
                                        mGetFinished = true;
                                        mRequestSuccess = false;
                                        moreData = false;
                                    }


//                                    std::cout << "OUTPUT: " << output.size() << std::endl;
                                    // on ne peut même pas décoder une taille, il manque des données
//                                    remaining = output;
                                }
                                else
                                {
                                    // il manque des données, il faut continuer à lire le socket
                                    moreData = false;
                                }
                            }
                            else
                            {
                                // il manque des données, il faut continuer à lire le socket
                                moreData = false;
                            }
                        } while (moreData);
                    }
                    else
                    {
                        response += mReply.body;
                        mGetFinished = true;
                    }
                }
                else
                {
                    mGetFinished = true;
                }
            }
            while (!mGetFinished);
        }
    }

    return mRequestSuccess;
}
