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

#ifndef TRANS_PORTLABEL_H_
#define TRANS_PORTLABEL_H_

#include <UTILS/UTILSTypes.h>
#include <boost/lexical_cast.hpp>
#include <limits.h>

UTILS_NAMESPACE_BEGIN

/**
 * Represents a reference to a PE data port.  A PortLabel is a string
 * wrapper which incorporates a PE id and a Port id.
 */
class PortLabel
{
  public:
    /**
     * Default constructor creates an empty PortLabel.
     */
    PortLabel() { parseIds(); }

    /**
     * Constructs an instance from a PE id and a PE port id.
     */
    PortLabel(unsigned long long peId, unsigned long long portId, unsigned long long jobId)
      : label_(PortLabel::toString(peId, portId, jobId))
      , peId_(peId)
      , portId_(portId)
      , jobId_(jobId)
    {}

    /**
     * Constructs an instance from a string.
     */
    PortLabel(const std::string& s)
      : label_(s)
    {
        parseIds();
    }

    /**
     * Constructs an instance from a C string.
     */
    PortLabel(const char* s)
      : label_(s)
    {
        parseIds();
    }

    /**
     * Constructs PortLabel with copy of value of @a other.
     *  @param other source PortLabel
     */
    PortLabel(const PortLabel& other)
      : label_(other.label_)
      , peId_(other.peId_)
      , portId_(other.portId_)
      , jobId_(other.jobId_)
    {}

    ~PortLabel() {}

    /**
     * Assigns the value of @a rhs to this PortLabel.
     * @param rhs source PortLabel
     */
    PortLabel& operator=(const PortLabel& rhs)
    {
        if (this != &rhs) {
            label_ = rhs.label_;
            peId_ = rhs.peId_;
            portId_ = rhs.portId_;
            jobId_ = rhs.jobId_;
        }
        return *this;
    }

    /**
     * Assigns the value of @a s to this PortLabel.
     * @param s source string
     */
    PortLabel& operator=(const std::string& s)
    {
        if (label_ != s) {
            label_ = s;
            parseIds();
        }
        return *this;
    }

    /**
     * Copies contents of @a s into this PortLabel.
     * @param str source null-terminated string
     */
    PortLabel& operator=(const char* s)
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

    /**
     * Returns the PE id of this object.
     * The id has a valid value if this object was build from a string and
     * parsing was successful, or if built from ids.
     *
     * @see isErrorPEId  The client can use this method to check whether
     * the id has a valid value.
     */
    unsigned long long getPEId() const { return peId_; }

    /**
     * Returns the PE port id of this object.
     * The id has a valid value if this object was build from a string and
     * parsing was successful, or if built from ids.
     *
     * @see isErrorPortId  The client can use this method to check whether
     * the id has a valid value.
     */
    unsigned long long getPortId() const { return portId_; }

    /**
     * Returns the job port id of this object.
     * The id has a valid value if this object was build from a string and
     * parsing was successful, or if built from ids.
     *
     * @see isErrorJobId  The client can use this method to check whether
     * the id has a valid value.
     */
    unsigned long long getJobId() const { return jobId_; }

    /**
     * Checks if the PE id has a valid value.
     */
    bool isErrorPEId() const { return peId_ == errorId_; }

    /**
     * Checks if the PE port id has a valid value.
     */
    bool isErrorPortId() const { return portId_ == errorId_; }

    /**
     * Checks if the Job id has a valid value.
     */
    bool isErrorJobId() const { return jobId_ == errorId_; }

    /**
     * Returns the @a string representation of the given PE and port Ids.
     */
    static const std::string toString(unsigned long long peId,
                                      unsigned long long portId,
                                      unsigned long long jobId);

    friend std::ostream& operator<<(std::ostream& o, const PortLabel& ref);

  private:
    std::string label_; ///< string label
    // parsed results
    mutable unsigned long long peId_;   ///< PE id (can be built from label_)
    mutable unsigned long long portId_; ///< Port id (can be built from label_)
    mutable unsigned long long jobId_;  ///< Job id (can be built from label_)
    /// value indicating a parsing error
    static const unsigned long long errorId_ = ULLONG_MAX;

    /**
     * Parse the label_ field and store results into the PE and port Id
     * fields.  The label must have format "pe_id_number.port_id_number".
     * If parsing fails, both ids are assigned the errorId value.
     * @return 0 for success, EINVAL if parsing failed.
     */
    int parseIds() const;
    void setParseErrorIds() const { peId_ = portId_ = errorId_; }
};

/////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, const PortLabel& ref)
{
    o << ref.label_;
    return o;
}

inline const std::string PortLabel::toString(unsigned long long peId,
                                             unsigned long long portId,
                                             unsigned long long jobId)
{
    return (boost::lexical_cast<std::string>(peId) + "." +
            boost::lexical_cast<std::string>(portId) + "@" +
            boost::lexical_cast<std::string>(jobId));
}

UTILS_NAMESPACE_END

#endif /* !TRANS_PORTLABEL_H_ */
