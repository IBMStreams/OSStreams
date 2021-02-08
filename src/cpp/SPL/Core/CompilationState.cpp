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

#include <SPL/Core/CompilationState.h>

using namespace std;
using namespace SPL;

#include <SPL/Runtime/Utility/Singleton.t>

CompilationState::State::State()
  : _stringizeGetChannel(false)
  , _stringizeGetLocalChannel(false)
  , _stringizeGetAllChannels(false)
  , _stringizeGetMaxChannels(false)
  , _stringizeGetLocalMaxChannels(false)
  , _stringizeGetAllMaxChannels(false)
  , _evaluateUDPIntrinsicsAs64BitLiteral(false)
  , _inParallelRegion(false)
  , _allowByChannel(false)
  , _loc(NULL)
{}

CompilationState::State::State(const CompilationState::State& other)
  : _stringizeGetChannel(other._stringizeGetChannel)
  , _stringizeGetLocalChannel(other._stringizeGetLocalChannel)
  , _stringizeGetAllChannels(other._stringizeGetAllChannels)
  , _stringizeGetMaxChannels(other._stringizeGetMaxChannels)
  , _stringizeGetLocalMaxChannels(other._stringizeGetLocalMaxChannels)
  , _stringizeGetAllMaxChannels(other._stringizeGetAllMaxChannels)
  , _evaluateUDPIntrinsicsAs64BitLiteral(other._evaluateUDPIntrinsicsAs64BitLiteral)
  , _inParallelRegion(other._inParallelRegion)
  , _allowByChannel(other._allowByChannel)
  , _loc(other._loc)
{}

CompilationState::CompilationState()
  : Singleton<CompilationState>(*this)
  , _currentState(new CompilationState::State())
{}

CompilationState::~CompilationState()
{
    delete _currentState;
}

void CompilationState::push()
{
    _stateStack.push(_currentState);
    _currentState = new CompilationState::State(*_currentState);
}

void CompilationState::pop()
{
    assert(_stateStack.size() > 0);
    delete _currentState;
    _currentState = _stateStack.top();
    _stateStack.pop();
}

bool CompilationState::isToolkitUsed(const Toolkit& toolkit) const
{
    if (_usedToolkits.find(&toolkit) != _usedToolkits.end())
        return true;
    return false;
}

void CompilationState::setToolkitUsed(const Toolkit& toolkit)
{
    _usedToolkits.insert(&toolkit);
}
