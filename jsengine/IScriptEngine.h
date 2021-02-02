/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef I_SCRIPT_ENGINE_H
#define I_SCRIPT_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include "Value.h"

/*****************************************************************************/
class IScriptEngine
{
public:
    class IPrinter
    {
    public:
        virtual ~IPrinter() {}

        virtual void Print(const std::string &msg) = 0;
    };

    class IFunction
    {
    public:
        virtual ~IFunction() {}

        virtual bool Execute(const std::vector<Value> &val, Value &ret) = 0;
    };

    typedef std::vector<std::string> StringList;

    virtual ~IScriptEngine() {}

    virtual void Initialize() = 0;
    virtual void RegisterPrinter(IScriptEngine::IPrinter *printer) = 0;
    virtual bool EvaluateFile(const std::string &fileName) = 0;
    virtual bool EvaluateString(const std::string &contents, std::string &output) = 0;
    virtual Value Call(const std::string &function, const StringList &args) = 0;
    virtual void Close() = 0;
    virtual void RegisterFunction(const std::string &name, std::shared_ptr<IScriptEngine::IFunction> function) = 0;
    virtual bool HasError() = 0;
    virtual std::string GetLastError() = 0;
    virtual void ClearError() = 0;
    virtual void RegisterString(const std::string &name, const std::string &value) = 0;
    virtual void SetModuleSearchPath(const std::string &path) = 0;
};

#endif // I_SCRIPT_ENGINE_H

//=============================================================================
// End of file IScriptEngine.h
//=============================================================================
