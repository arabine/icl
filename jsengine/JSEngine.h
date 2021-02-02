/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef JSENGINE_H
#define JSENGINE_H

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <functional>
#include <memory>

#include "Observer.h"
#include "duktape.h"
#include "IScriptEngine.h"

/*****************************************************************************/
class JSEngine : public IScriptEngine
{
public:
    typedef duk_context JSContext;
    typedef duk_ret_t JSRetCode;
    using FunctionPtr = std::add_pointer<JSRetCode(JSContext)>::type;

    JSEngine();
    ~JSEngine();

    // From IScriptEngine
    void Initialize();
    void RegisterPrinter(IScriptEngine::IPrinter *printer);
    bool EvaluateFile(const std::string &fileName);
    bool EvaluateString(const std::string &contents, std::string &output);
    Value Call(const std::string &function, const IScriptEngine::StringList &args);
    void Close();
    void RegisterFunction(const std::string &name, std::shared_ptr<IFunction> function);
    bool HasError();
    std::string GetLastError();
    void ClearError();
    void RegisterString(const std::string &name, const std::string &value);
    void SetModuleSearchPath(const std::string &path);

private:
    duk_context *mCtx;
    bool mValidContext;
    std::uint32_t mId; // Id of the current script context
    bool mHasError;
    std::string mLastError;

    void SetError(const std::string &error);
    void PrintTop() const;

};

#endif // JSENGINE_H

//=============================================================================
// End of file JSEngine.h
//=============================================================================
