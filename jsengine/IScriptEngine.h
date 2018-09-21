/*=============================================================================
 * TarotClub - IScriptEngine.h
 *=============================================================================
 * Agnostic script engine interface
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef I_SCRIPT_ENGINE_H
#define I_SCRIPT_ENGINE_H

#include <string>
#include <vector>
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
    virtual void RegisterFunction(const std::string &name, IScriptEngine::IFunction *function) = 0;
    virtual bool HasError() = 0;
    virtual std::string GetLastError() = 0;
    virtual void ClearError() = 0;
};

#endif // I_SCRIPT_ENGINE_H

//=============================================================================
// End of file IScriptEngine.h
//=============================================================================
