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

#ifndef SPL_RUNTIME_PE_OUTPUT_PORT_DELEGATE_H
#define SPL_RUNTIME_PE_OUTPUT_PORT_DELEGATE_H

#include <inttypes.h>
#include <stddef.h>

namespace SPL {
class PEImpl;
class Tuple;
class NativeByteBuffer;
class Punctuation;

class PEOutputPortTupleDelegate
{
  public:
    PEOutputPortTupleDelegate(PEImpl* pe, int32_t port)
      : pe_(pe)
      , port_(port)
    {}

    uint32_t operator()(Tuple const& tuple) const;

  private:
    PEImpl* pe_;
    int32_t port_;
};

class PEOutputPortNativeByteBufferDelegate
{
  public:
    PEOutputPortNativeByteBufferDelegate(PEImpl* pe, int32_t port)
      : pe_(pe)
      , port_(port)
    {}

    uint32_t operator()(NativeByteBuffer& buffer) const;

    // returns whether the PE output port has filters.
    // If true, then we dont want to send data as a buffer since the filtering
    // code cannot handle that.
    bool hasFilters();

  private:
    PEImpl* pe_;
    int32_t port_;
};

class PEOutputPortPunctDelegate
{
  public:
    PEOutputPortPunctDelegate(PEImpl* pe, int32_t port)
      : pe_(pe)
      , port_(port)
    {}

    uint32_t operator()(Punctuation const& punct) const;

  private:
    PEImpl* pe_;
    int32_t port_;
};

class PEOutputPortDelegates
{
  public:
    PEOutputPortDelegates()
      : tupleDel_(NULL, -1)
      , punctDel_(NULL, -1)
    {}
    PEOutputPortDelegates(PEOutputPortTupleDelegate t, PEOutputPortPunctDelegate p)
      : tupleDel_(t)
      , punctDel_(p)
    {}

    uint32_t operator()(Tuple const& tuple) const { return tupleDel_.operator()(tuple); }
    uint32_t operator()(Punctuation const& punct) const { return punctDel_.operator()(punct); }

  private:
    PEOutputPortTupleDelegate tupleDel_;
    PEOutputPortPunctDelegate punctDel_;
};
};

#endif /* SPL_RUNTIME_PE_OUTPUT_PORT_DELEGATE_H */
