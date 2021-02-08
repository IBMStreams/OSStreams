/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPL_COMPILER_STATE_H
#define SPL_COMPILER_STATE_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>

#include <cassert>
#include <stack>

namespace SPL {

class SourceLocation;
class Toolkit;

class CompilationState : public Singleton<CompilationState>
{
    struct State
    {

        State();
        State(const State& state);

        /************ NOTE: Don't forget to update the ctor and copy ctor when adding members **************/
        bool _stringizeGetChannel;
        bool _stringizeGetLocalChannel;
        bool _stringizeGetAllChannels;
        bool _stringizeGetMaxChannels;
        bool _stringizeGetLocalMaxChannels;
        bool _stringizeGetAllMaxChannels;
        bool _evaluateUDPIntrinsicsAs64BitLiteral;
        bool _inParallelRegion;
        bool _allowByChannel;
        const SourceLocation* _loc;
    };

  public:
    CompilationState();
    ~CompilationState();

    /// Pushes the current state and places a new, identical, state on the top of the state stack
    void push();

    /// Pops one state off the state stack
    void pop();

    /// Determine if the UDP related intrinsic functions should be stringized
    /// @return Returns true if this function should be stringized, otherwise false
    bool stringizeGetChannel() const { return _currentState->_stringizeGetChannel; }
    bool stringizeGetLocalChannel() const { return _currentState->_stringizeGetLocalChannel; }
    bool stringizeGetAllChannels() const { return _currentState->_stringizeGetAllChannels; }
    bool stringizeGetMaxChannels() const { return _currentState->_stringizeGetMaxChannels; }
    bool stringizeGetLocalMaxChannels() const
    {
        return _currentState->_stringizeGetLocalMaxChannels;
    }
    bool stringizeGetAllMaxChannels() const { return _currentState->_stringizeGetAllMaxChannels; }

    /// Enable stringization of the UDP related intrinsic function
    void setStringizeGetChannel() { _currentState->_stringizeGetChannel = true; }
    void setStringizeGetLocalChannel() { _currentState->_stringizeGetLocalChannel = true; }
    void setStringizeGetAllChannels() { _currentState->_stringizeGetAllChannels = true; }
    void setStringizeGetMaxChannels() { _currentState->_stringizeGetMaxChannels = true; }
    void setStringizeGetLocalMaxChannels() { _currentState->_stringizeGetLocalMaxChannels = true; }
    void setStringizeGetAllMaxChannels() { _currentState->_stringizeGetAllMaxChannels = true; }

    bool allowByChannel() const { return _currentState->_allowByChannel; }
    void setAllowByChannel() { _currentState->_allowByChannel = true; }

    void setEvaluateUDPIntrinsicsAs64BitLiteral() const
    {
        _currentState->_evaluateUDPIntrinsicsAs64BitLiteral = true;
    }
    bool evaluateUDPIntrinsicsAs64BitLiteral() const
    {
        return _currentState->_evaluateUDPIntrinsicsAs64BitLiteral;
    }

    bool isInParallelRegion() const { return _currentState->_inParallelRegion; }
    void setIsInParallelRegion() { _currentState->_inParallelRegion = true; }

    void setSourceLocation(const SourceLocation& loc) { _currentState->_loc = &loc; }
    const SourceLocation& sourceLocation() const
    {
        assert(NULL != _currentState->_loc);
        return *_currentState->_loc;
    }

    /// Determine if the given toolkit is used in the compilation
    bool isToolkitUsed(const Toolkit& toolkit) const;
    void setToolkitUsed(const Toolkit& toolkit);

    const std::tr1::unordered_set<const Toolkit*>& usedToolkits() const { return _usedToolkits; }

  private:
    std::stack<State*> _stateStack;
    State* _currentState;
    std::tr1::unordered_set<const Toolkit*> _usedToolkits;
};

} // namespace SPL

#endif
