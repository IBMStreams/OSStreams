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

#ifndef SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_FACTORY_H
#define SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_FACTORY_H

namespace SPL {
template<class T, class G>
class TumblingWindowSummarizerFactory
{
  public:
    typedef T TupleType;     //!< tuple type
    typedef G PartitionType; //!< partition type

    virtual ~TumblingWindowSummarizerFactory() {}
    virtual TumblingWindowSummarizer<TupleType, PartitionType>* createSummarizer(Operator&) = 0;

    template<class M>
    static TumblingWindowSummarizerFactory<T, G>* createFactory(M* dummy);
};

template<class T, class G, class M>
class BasicTumblingWindowSummarizerFactory : public TumblingWindowSummarizerFactory<T, G>
{
  public:
    typedef T TupleType;      //!< tuple type
    typedef G PartitionType;  //!< partition type
    typedef M SummarizerType; //!< summarizer type

    TumblingWindowSummarizer<TupleType, PartitionType>* createSummarizer(Operator& oper)
    {
        return new SummarizerType(oper);
    }
};

template<class T, class G>
template<class M>
TumblingWindowSummarizerFactory<T, G>* TumblingWindowSummarizerFactory<T, G>::createFactory(
  M* dummy)
{
    return new BasicTumblingWindowSummarizerFactory<T, G, M>();
}
};

#endif /*SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_FACTORY_H */
