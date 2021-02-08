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

#ifndef TRANS_OPERATORPORTLABEL_H_
#define TRANS_OPERATORPORTLABEL_H_

#include <UTILS/UTILSTypes.h>
#include <boost/lexical_cast.hpp>
#include <limits.h>

UTILS_NAMESPACE_BEGIN

/**
 * Represents a reference to an operator data port.  An OperatorPortLabel is a string
 * wrapper which incorporates a job id, operator index, and operator port index.
 */
class OperatorPortLabel
{
  public:
    /**
     * Default constructor creates an empty OperatorPortLabel.
     */
    OperatorPortLabel() { parseIds(); }

    /**
     * Constructs an instance from a job id, operator index, and operator port index
     */
    OperatorPortLabel(unsigned long long jobId,
                      unsigned long long operatorIndex,
                      unsigned long long operatorPortIndex)
      : label_(OperatorPortLabel::toString(jobId, operatorIndex, operatorPortIndex))
      , jobId_(jobId)
      , operatorIndex_(operatorIndex)
      , operatorPortIndex_(operatorPortIndex)
    {}

    /**
     * Constructs an instance from a string.
     */
    OperatorPortLabel(const std::string& s)
      : label_(s)
    {
        parseIds();
    }

    /**
     * Constructs an instance from a C string.
     */
    OperatorPortLabel(const char* s)
      : label_(s)
    {
        parseIds();
    }

    /**
     * Constructs OperatorPortLabel with copy of value of @a other.
     *  @param other source OperatorPortLabel
     */
    OperatorPortLabel(const OperatorPortLabel& other)
      : label_(other.label_)
      , jobId_(other.jobId_)
      , operatorIndex_(other.operatorIndex_)
      , operatorPortIndex_(other.operatorPortIndex_)
    {}

    ~OperatorPortLabel() {}

    /**
     * Assigns the value of @a rhs to this OperatorPortLabel.
     * @param rhs source OperatorPortLabel
     */
    OperatorPortLabel& operator=(const OperatorPortLabel& rhs)
    {
        if (this != &rhs) {
            label_ = rhs.label_;
            jobId_ = rhs.jobId_;
            operatorIndex_ = rhs.operatorIndex_;
            operatorPortIndex_ = rhs.operatorPortIndex_;
        }
        return *this;
    }

    /**
     * Assigns the value of @a s to this OperatorPortLabel.
     * @param s source string
     */
    OperatorPortLabel& operator=(const std::string& s)
    {
        if (label_ != s) {
            label_ = s;
            parseIds();
        }
        return *this;
    }

    /**
     * Copies contents of @a s into this OperatorPortLabel.
     * @param str source null-terminated string
     */
    OperatorPortLabel& operator=(const char* s)
    {
        if (label_ != s) {
            label_ = s;
            parseIds();
        }
        return *this;
    }

    /**
     * Checks whether this instance is empty.
     */
    bool empty() const { return label_.empty(); }

    /**
     * Returns a reference to the internal string representation.
     */
    const std::string& str() const { return label_; }

    unsigned long long getJobId() const { return jobId_; }
    unsigned long long getOperatorIndex() const { return operatorIndex_; }
    unsigned long long getOperatorPortIndex() const { return operatorPortIndex_; }
    bool isErrorJobId() const { return jobId_ == errorId_; }
    bool isErrorOperatorIndex() const { return operatorIndex_ == errorId_; }
    bool isErrorOperatorPortIndex() const { return operatorPortIndex_ == errorId_; }

    /**
     * Returns the @a string representation of the given jobId, operator index, and operator port
     * index
     */
    static const std::string toString(unsigned long long jobId,
                                      unsigned long long operatorIndex,
                                      unsigned long long operatorPortIndex);

    friend std::ostream& operator<<(std::ostream& o, const OperatorPortLabel& ref);

  private:
    std::string label_; ///< string label
    // parsed results
    mutable unsigned long long jobId_;
    mutable unsigned long long operatorIndex_;
    mutable unsigned long long operatorPortIndex_;

    /// value indicating a parsing error
    static const unsigned long long errorId_ = ULLONG_MAX;

    int parseIds() const;
    void setParseErrorIds() const { jobId_ = operatorIndex_ = operatorPortIndex_ = errorId_; }
};

/////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, const OperatorPortLabel& ref)
{
    o << ref.label_;
    return o;
}

inline const std::string OperatorPortLabel::toString(unsigned long long jobId,
                                                     unsigned long long operatorIndex,
                                                     unsigned long long operatorPortIndex)
{
    return (boost::lexical_cast<std::string>(jobId) + "." +
            boost::lexical_cast<std::string>(operatorIndex) + "." +
            boost::lexical_cast<std::string>(operatorPortIndex));
}

UTILS_NAMESPACE_END

#endif /* !TRANS_OPERATORPORTLABEL_H_ */
