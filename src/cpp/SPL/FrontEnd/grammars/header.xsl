<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   version="1.0"
   xmlns:xalan="http://xml.apache.org/xslt">

   <xsl:output indent="no" method="text"/>
   <xsl:strip-space elements="*"/>

   <xsl:template match="/">
      <xsl:call-template name="prolog"/>
      <xsl:call-template name="base-classes"/>
      <xsl:apply-templates select="/grammar/production[@gen='yes']" mode="derived-class"/>
      <xsl:call-template name="epilog"/>
   </xsl:template>

<xsl:variable name="lcalpha" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="ucalpha" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="production"><xsl:value-of select="@name"/><xsl:text>
</xsl:text></xsl:template>


<xsl:template name="prolog">
#ifndef SPL_PARSER_CONTEXTS_H
#define SPL_PARSER_CONTEXTS_H


#include &lt;SPL/FrontEnd/Generated/SPLParser.h&gt;
#include &lt;SPL/FrontEnd/TreeHelper.h&gt;
#include &lt;vector&gt;

namespace SPL
{

</xsl:template>

<xsl:template name="epilog">
} // namespace SPL
#endif // SPL_PARSER_CONTEXTS_H
</xsl:template>

<xsl:template name="base-classes">
class ParaphrasePusher
{
public:

    ParaphrasePusher(SPL::ParserContext &amp; parserState, int syntaxType) : _parserState(parserState)
    { parserState._paraphrase.push_back(syntaxType); }

   ~ParaphrasePusher()
    { _parserState._paraphrase.pop_back(); }

private:
    SPL::ParserContext &amp; _parserState;
};

/// The base class for the specific production context classes
class ContextBase
{
public:
    virtual ~ContextBase()  { _parserState.popContext(); }

    /// This is called at the end of a production to provide the "return" value.
    virtual void addChild(AstNode &amp; ast) { assert(false); }

    /// Set the production's state
    void operator&lt;&lt;(unsigned val) { _state = val; }

protected:
    ContextBase(SPL::ParserContext &amp; parserState, AstToken &amp; startTok)
       : _parserState(parserState), _startToken(startTok),
       	 _pool(SPL::AstPool::instance()), _previousContext(parserState.pushContext(*this)),
         _state(0) { }


    SPL::AstPool &amp; pool() const { return _pool; }
    SPL::ParserContext &amp; parseState() const { return _parserState; }
    SPL::ContextBase &amp; previousContext() const { assert(NULL != _previousContext); return *_previousContext; }
    AstNode &amp; errorNode();
    AstToken &amp; startToken() const { return _startToken; }
    unsigned state() const { return _state; }
private:
    SPL::ParserContext &amp; _parserState;
    AstToken &amp;           _startToken;
    SPL::AstPool &amp;       _pool;
    ContextBase *        _previousContext;
    unsigned             _state;
};
</xsl:template>


<xsl:template match="production" mode="class-name"><xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>Context</xsl:template>
<xsl:template match="production" mode="ctor">   <xsl:apply-templates select="." mode="class-name"/>(SPL::ParserContext &amp; parserState, pANTLR3_TOKEN_STREAM tokStream, AstToken &amp; startTok);
</xsl:template>
<xsl:template match="production" mode="dtor">   virtual ~<xsl:apply-templates select="." mode="class-name"/>();</xsl:template>

<xsl:template match="production" mode="members"><xsl:apply-templates select="child::*" mode="members"><xsl:with-param name="state" select="0"/></xsl:apply-templates></xsl:template>

<xsl:template match="sequence" mode="members">
   <xsl:param name="state"/>
   <xsl:apply-templates select="child::*[position()=1]" mode="members"><xsl:with-param name="state" select="$state"/></xsl:apply-templates>
</xsl:template>

<xsl:template match="terminal" mode="members">
<xsl:param name="state"/>
<xsl:apply-templates select="following-sibling::*[position()=1]" mode="members"><xsl:with-param name="state" select="$state"/></xsl:apply-templates>
</xsl:template>

<xsl:template match="group" mode="members">
<xsl:param name="state"/>
   <xsl:apply-templates select="." mode="type"/> _state<xsl:value-of select="$state"/>Var; // group<xsl:text>
</xsl:text><xsl:apply-templates select="following-sibling::*[position()=1]" mode="members"><xsl:with-param name="state" select="$state+1"/></xsl:apply-templates>
</xsl:template>

<xsl:template match="specialTerminal" mode="members"/>

<xsl:template match="nonTerminal" mode="members">
<xsl:param name="state"/>
   <xsl:apply-templates select="." mode="type"/> _state<xsl:value-of select="$state"/>Var; // <xsl:value-of select="name"/><xsl:text>
</xsl:text>
   <xsl:apply-templates select="following-sibling::*[position()=1]" mode="members"><xsl:with-param name="state" select="$state+1"/></xsl:apply-templates>
</xsl:template>

<xsl:template match="nonTerminal[@repetition='?']" mode="type">    AstNode *            </xsl:template>
<xsl:template match="nonTerminal[@repetition='*']" mode="type">    std::vector&lt;AstNode*&gt;</xsl:template>
<xsl:template match="nonTerminal" mode="type">    AstNode &amp;</xsl:template>

<xsl:template match="group[@repetition='?']" mode="type">    AstNode *            </xsl:template>
<xsl:template match="group[@repetition='*']" mode="type">    std::vector&lt;AstNode*&gt;</xsl:template>
<xsl:template match="group" mode="type">    AstNode &amp;</xsl:template>

<xsl:template match="production" mode="derived-class">
class <xsl:apply-templates select="." mode="class-name"/> : public ContextBase
{
public:

   <xsl:apply-templates select="." mode="ctor"/>
<xsl:apply-templates select="." mode="dtor"/><xsl:text>
</xsl:text>
   virtual addChild(AstNode &amp; ast);

private:
<xsl:apply-templates select="." mode="members"/>
};
</xsl:template>

</xsl:stylesheet>
