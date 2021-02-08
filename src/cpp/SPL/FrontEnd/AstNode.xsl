<?xml version="1.0"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">

  <xsl:param name="mode"/>
  <xsl:output indent="no" method="text"/>
  <xsl:strip-space elements="*"/>
  <xsl:variable name="lcalpha" select="'abcdefghijklmnopqrstuvwxyz'"/>
  <xsl:variable name="ucalpha" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>


  <xsl:template match="/">
    <xsl:choose>
      <xsl:when test="$mode = 'h'">
        <xsl:apply-templates select="AstNodes" mode="h"/>
      </xsl:when>
      <xsl:when test="$mode = 'cpp'">
        <xsl:apply-templates select="AstNodes" mode="cpp"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:message terminate="yes">Invalid mode</xsl:message>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<xsl:template name="open-namespace-spl">
namespace SPL
{
</xsl:template>

  <xsl:template name="close-namespace-spl">
} // namespace SPL
</xsl:template>

<xsl:template name="open-namespace-ast">
namespace AST
{</xsl:template>

  <xsl:template name="close-namespace-ast">

} // namespace AST
</xsl:template>

<xsl:template match="AstNodes" mode="h"><xsl:message>Generating AstNode header include</xsl:message>// This is a generated file.  Any changes made locally will be lost
  <xsl:call-template name="open-namespace-spl"/>
  <xsl:call-template name="declare-base-class"/>
  <xsl:call-template name="declare-generic-class"/>
  <xsl:apply-templates select="AstNode" mode="declare-classes"/>
  <xsl:call-template name="open-namespace-ast"/>
  <xsl:apply-templates select="AstNode[count(@noAstNamespace) = 0]" mode="declare-namespaces"/>
  <xsl:call-template name="close-namespace-ast"/>
  <xsl:call-template name="close-namespace-spl"/>
</xsl:template>

<xsl:template name="declare-base-class"><xsl:text>
class AstNodeBase
{
public:
    enum Kind {</xsl:text><xsl:apply-templates select="AstNode" mode="enum-decl"/><xsl:text>
               Generic};
    Kind getKind() const { return _kind; }

    virtual AstNodeBase &amp; clone() const = 0;
    AstNode &amp; astNode() const { return _astNode; }

    AstInformation &amp; astInfo() { return _astInfo; }
    const AstInformation &amp; astInfo() const { return _astInfo; }

    virtual ~AstNodeBase();

protected:

    AstNodeBase(Kind kind, AstNode &amp; astNode_);
    AstNodeBase(const AstNodeBase &amp; other);

private:
    Kind           _kind;
    AstNode &amp;      _astNode;
    AstInformation _astInfo;
};
</xsl:text>
</xsl:template>

<xsl:template name="define-base-class-methods"><xsl:text>
AstNodeBase::AstNodeBase(Kind kind, AstNode &amp; astNode_) : _kind(kind), _astNode(astNode_)
{
    astNode_.u = &amp;_astInfo;
    pANTLR3_COMMON_TREE pSuper = static_cast&lt;pANTLR3_COMMON_TREE&gt;(astNode_.super);
    pSuper-&gt;super = this;
}

AstNodeBase::AstNodeBase(const AstNodeBase &amp; other) : _kind(other._kind), _astNode(*static_cast&lt;AstNode*&gt;(other._astNode.dupNode(&amp;other._astNode))), _astInfo(other._astInfo)
{
    _astNode.u = &amp;_astInfo;
    pANTLR3_COMMON_TREE pSuper = static_cast&lt;pANTLR3_COMMON_TREE&gt;(_astNode.super);
    pSuper->super = this;
    MODEL("ASTNode(" &lt;&lt; (void*)&amp;_astNode &lt;&lt; ", " &lt;&lt; astType(_astNode) &lt;&lt; ", " &lt;&lt; astText(_astNode) &lt;&lt; ", " &lt;&lt; astLine(_astNode) &lt;&lt; ", " &lt;&lt; astColumn(_astNode) &lt;&lt; ")\n");
    if (NULL != _astInfo.getSymbol()) {
        SPLDBG(__PRETTY_FUNCTION__ &lt;&lt; ": AssociateSymbol: ast: " &lt;&lt; (void*)&amp;_astNode &lt;&lt; " sym: " &lt;&lt; (void*)_astInfo.getSymbol(), Debug::ShowSymbols);
        MODEL("AssociateSymbol(" &lt;&lt; (void*)&amp;_astNode &lt;&lt; ", " &lt;&lt; (void*)_astInfo.getSymbol() &lt;&lt; ")\n");
    }
    if (NULL != _astInfo.getExpression()) {
        SPLDBG(__PRETTY_FUNCTION__ &lt;&lt; ": AssociateExpression: ast: " &lt;&lt; (void*)&amp;_astNode &lt;&lt; " expr: " &lt;&lt; (void*)_astInfo.getExpression(), Debug::ShowSymbols);
        MODEL("AssociateExpression(" &lt;&lt; (void*)&amp;_astNode &lt;&lt; ", " &lt;&lt; (void*)_astInfo.getExpression() &lt;&lt; ")\n");
    }
}

AstNodeBase::~AstNodeBase()
{
}

</xsl:text>
</xsl:template>


<xsl:template name="declare-generic-class"><xsl:text>
class AstGenericNode : public AstNodeBase
{
public:
    AstGenericNode(AstNode &amp; ast) : AstNodeBase(AstNodeBase::Generic, ast) {}
    virtual AstNodeBase &amp; clone() const;
protected:
    AstGenericNode(const AstGenericNode &amp; other) : AstNodeBase(other) {}
private:
};
</xsl:text>
</xsl:template>

<xsl:template name="define-generic-class-methods"><xsl:text>
AstNodeBase &amp; AstGenericNode::clone() const
{
    AstGenericNode * newNode =  new AstGenericNode(*this);
    return *newNode;
}
</xsl:text>
</xsl:template>

<xsl:template match="AstNode" mode="declare-classes"><xsl:text>
class </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text> : public AstNodeBase
{
public:
    </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(AstNode &amp; ast) : AstNodeBase(AstNodeBase::</xsl:text><xsl:apply-templates select="." mode="enum-name"/><xsl:text>, ast) {}
    virtual AstNodeBase &amp; clone() const;
protected:
    </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(const </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text> &amp; other) : AstNodeBase(other) {}
};
</xsl:text>
</xsl:template>

<xsl:template match="AstNode" mode="define-class-methods"><xsl:text>
AstNodeBase &amp; </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>::clone() const
{
    </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text> * newNode =  new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(*this);
    return *newNode;
}
</xsl:text>
</xsl:template>

<xsl:template match="AstNode" mode="declare-namespaces"><xsl:text>

namespace </xsl:text><xsl:value-of select="@name"/><xsl:text>
{
</xsl:text>
    <xsl:apply-templates select="." mode="push-public"/>
    <xsl:apply-templates select="." mode="declare-creates"/>
    <xsl:text>    bool is(AstNode &amp; ast);
</xsl:text>
    <xsl:apply-templates select="./Child" mode="declare-accessor"/>
    <xsl:apply-templates select="./Child" mode="declare-count"/>
    <xsl:apply-templates select="./Child" mode="declare-has"/>
    <xsl:apply-templates select="./Child" mode="declare-setter"/>
    <xsl:apply-templates select="." mode="pop-public"/>
<xsl:text>} // namespace </xsl:text><xsl:value-of select="@name"/><xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="AstNode[@public='true']" mode="push-public">
<xsl:text>PUSH_DLL_PUBLIC
</xsl:text>
</xsl:template>

<xsl:template match="AstNode[@public='true']" mode="pop-public">
<xsl:text>POP_DLL_PUBLIC
</xsl:text>
</xsl:template>

<xsl:template match="AstNode" mode="declare-creates">
  <!-- declare the full create method -->
  <xsl:apply-templates select="." mode="declare-creates-with-start"/>
  <xsl:apply-templates select="." mode="declare-creates-without-start"/>
  <xsl:apply-templates select="." mode="declare-text-only-create"/>
</xsl:template>

<xsl:template match="AstNode" mode="declare-creates-with-start">
  <xsl:if test="not(@start='false')">
     <xsl:text>    AstNode &amp; create(AstToken &amp; start</xsl:text>
     <xsl:if test="count(Child)>0">
       <xsl:text>, </xsl:text>
     </xsl:if>
     <xsl:apply-templates select="./Child" mode="param"/>
     <xsl:text>);</xsl:text>
     <!-- If there is an optional param, declare a create method with start token and all but optional kid -->
     <xsl:if test="count(Child[count(@optional)>0])>0">
       <xsl:text>
    AstNode &amp; create(AstToken &amp; start</xsl:text>
       <xsl:if test="count(Child)>1"><xsl:text>, </xsl:text>
         <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
       </xsl:if>
       <xsl:text>);</xsl:text>
    </xsl:if>
    <xsl:text>
</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="declare-creates-without-start">
  <xsl:if test="@noStart='true'">
    <xsl:text>    AstNode &amp; create(</xsl:text>
    <xsl:apply-templates select="./Child" mode="param"/>
    <xsl:text>);</xsl:text>
    <!-- If there is an optional param, declare a create method with all but optional kid -->
      <xsl:if test="count(Child[count(@optional)>0])>0">
      <xsl:text>
    AstNode &amp; create(</xsl:text>
      <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
      <xsl:text>);</xsl:text>
    </xsl:if>
    <xsl:text>
</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="declare-text-only-create">
<xsl:if test="@text='true'">
<xsl:text>
    AstNode &amp; create(const std::string &amp; text</xsl:text>
  <xsl:if test="count(Child)>count(Child[@optional])">
    <xsl:text>, </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
  <xsl:text>);
</xsl:text>
</xsl:if>
</xsl:template>

<!--xsl:template match="AstNode" mode="declare-other-create">
    AstNode &amp; create(AstToken &amp; start<xsl:if test="count(Child)>count(Child[@optional])">, </xsl:if><xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>);</xsl:template-->

<xsl:template match="AstNode" mode="define-namespaces">
<xsl:text>

namespace </xsl:text><xsl:value-of select="@name"/><xsl:text>
{
</xsl:text>
  <xsl:apply-templates select="." mode="define-creates"/>
  <xsl:apply-templates select="." mode="define-is"/>
  <xsl:apply-templates select="Child" mode="define-accessor"/>
  <xsl:apply-templates select="Child" mode="define-count"/>
  <xsl:apply-templates select="Child" mode="define-has"/>
  <xsl:apply-templates select="Child" mode="define-setter"/>
<xsl:text>
} // namespace </xsl:text><xsl:value-of select="@name"/>
</xsl:template>


<xsl:template match="AstNode" mode="define-is">
<xsl:text>
bool is(AstNode &amp; ast)
{
    return AstNodeBase::</xsl:text><xsl:apply-templates select="." mode="enum-name"/><xsl:text> == wrapper(ast).getKind();
}
</xsl:text>
</xsl:template>

<!-- Define main and any optional create methods -->
<xsl:template match="AstNode" mode="define-creates">
  <xsl:apply-templates select="." mode="define-creates-with-start"/>
  <xsl:apply-templates select="." mode="define-creates-without-start"/>
  <xsl:apply-templates select="." mode="define-text-only-create"/>
</xsl:template>

<!-- Define the create method for this node -->
<xsl:template match="AstNode" mode="define-creates-with-start">
  <xsl:if test="not(@start='false')">
    <xsl:text>
AstNode &amp; create(AstToken &amp; start</xsl:text>
    <xsl:if test="count(Child)>0">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="./Child" mode="param"/>
    <xsl:text>)
{
    AstNode &amp; result = createAst(start</xsl:text>
    <xsl:if test="not(@useStartToken='true')">
      <xsl:apply-templates select="." mode="token-type-arg-with-comma"/>
      <xsl:apply-templates select="." mode="spelling-arg"/>
    </xsl:if>
    <xsl:apply-templates select="Child" mode="arg"/>
    <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
</xsl:text>
    <!-- If there is an optional child, define the create with start and without the optional child -->
    <xsl:if test="count(Child[count(@optional)>0])>0">
      <xsl:text>
AstNode &amp; create(AstToken &amp; start</xsl:text>
      <xsl:if test="count(Child)>1">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
      <xsl:text>)
{
    AstNode &amp; result = createAst(start</xsl:text>
    <xsl:if test="not(@useStartToken='true')">
      <xsl:apply-templates select="." mode="token-type-arg-with-comma"/>
      <xsl:apply-templates select="." mode="spelling-arg"/>
    </xsl:if>
    <xsl:apply-templates select="Child[not(@optional) or @optional='false']" mode="arg"/>
    <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
</xsl:text>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="define-creates-without-start">
  <xsl:if test="@noStart='true'">
    <xsl:text>
AstNode &amp; create(</xsl:text>
    <xsl:apply-templates select="./Child" mode="param"/>
    <xsl:text>)
{
    AstNode &amp; result = createAst(</xsl:text>
         <xsl:apply-templates select="." mode="token-type-arg"/>
         <xsl:apply-templates select="." mode="spelling-arg"/>
         <xsl:apply-templates select="Child" mode="arg"/>
      <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
  </xsl:text>
    <!-- If there is an optional child, define the create without the optional child -->
    <xsl:if test="count(Child[count(@optional)>0])>0">
<xsl:text>
AstNode &amp; create(</xsl:text>
    <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
    <xsl:text>)
{
    AstNode &amp; result = createAst(</xsl:text>
         <xsl:apply-templates select="." mode="token-type-arg"/>
         <xsl:apply-templates select="." mode="spelling-arg"/>
         <xsl:apply-templates select="Child[not(@optional) or @optional='false']" mode="arg"/>
      <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
</xsl:text>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="define-text-only-create">
  <xsl:if test="@text='true'">
    <xsl:text>
AstNode &amp; create(const string &amp; text</xsl:text>
    <xsl:apply-templates select="./Child" mode="param"/>
    <xsl:text>)
{
    AstNode &amp; result = createAst(</xsl:text>
    <xsl:apply-templates select="." mode="token-type-arg"/>
    <!--xsl:if test="count(@tokenType) != 0">
      <xsl:text>, </xsl:text>
    </xsl:if -->
    <xsl:text>, text</xsl:text>
    <!--xsl:apply-templates select="." mode="spelling-arg"/-->
    <xsl:apply-templates select="Child" mode="arg"/>
    <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
  </xsl:text>
</xsl:if>
</xsl:template>

<!--xsl:template match="AstNode" mode="define-other-create">
  <xsl:text>
AstNode &amp; create(AstToken &amp; start</xsl:text>
  <xsl:if test="count(Child[not(@optional)or @optional='false']) > 0">
    <xsl:text>, </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="./Child[not(@optional) or @optional='false']" mode="param"/>
  <xsl:text>)
{
    AstNode &amp; result = createAst(start</xsl:text>
  <xsl:apply-templates select="." mode="token-type-arg-with-comma"/>
  <xsl:apply-templates select="." mode="spelling-arg"/>
  <xsl:apply-templates select="Child[not(@optional) or @optional='false']" mode="arg"/>
  <xsl:text>);
    assert(result.u == NULL);
    (void) new </xsl:text><xsl:apply-templates select="." mode="class-name"/><xsl:text>(result);
    assert(result.u != NULL);
    return result;
}
</xsl:text>
</xsl:template-->

<xsl:template match="AstNode" mode="token-type-arg-with-comma">
  <xsl:if test="count(@tokenType) != 0">
    <xsl:text>, </xsl:text>
    <xsl:apply-templates select="." mode="token-type-arg"/>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="token-type-arg">
  <xsl:if test="count(@tokenType) != 0">
    <xsl:value-of select="@tokenType"/>
  </xsl:if>
</xsl:template>

<xsl:template match="AstNode" mode="spelling-arg">
  <xsl:choose>
    <xsl:when test="@spelling">
      <xsl:text>, "</xsl:text>
      <xsl:value-of select="@spelling"/>
      <xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="@tokenType">
        <xsl:text>, "</xsl:text>
        <xsl:value-of select="@tokenType"/>
        <xsl:text>"</xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="param">
  <xsl:if test="position() != 1">
    <xsl:text>, </xsl:text>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>std::vector&lt;AstNode*&gt; &amp; children</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>AstNode &amp; </xsl:text><xsl:value-of select="@name"/><xsl:text>_</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="arg">
  <xsl:text>, </xsl:text>
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>children</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@name"/><xsl:text>_</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="declare-accessor">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>    AstNode &amp; </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>(AstNode &amp; ast, int child);
</xsl:text>
    </xsl:when>
  <xsl:otherwise>
    <xsl:text>    AstNode &amp; </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>(AstNode &amp; ast);
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="define-accessor">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
<xsl:text>
AstNode &amp; </xsl:text><xsl:value-of select="@name"/><xsl:text>(AstNode &amp; ast, int child)
{
    assert(</xsl:text><xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
    // The variable length child is at position </xsl:text><xsl:value-of select="position()-1"/>
      <xsl:choose>
        <xsl:when test="position() &lt; 2">
<xsl:text>
    return astChild(ast, child);</xsl:text>
        </xsl:when>
        <xsl:otherwise>
<xsl:text>
    return astChild(ast, child + </xsl:text><xsl:value-of select="position()-1"/><xsl:text>);</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
<xsl:text>
}
</xsl:text>
    </xsl:when>
    <xsl:otherwise>
<xsl:text>
AstNode &amp; </xsl:text><xsl:value-of select="@name"/><xsl:text>(AstNode &amp; ast)
{
    assert(</xsl:text><xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
</xsl:text>
  <xsl:if test="@optional">
    <xsl:text>    assert(</xsl:text><xsl:value-of select="position()-1"/><xsl:text> &lt; astSize(ast));
</xsl:text>
  </xsl:if>
  <xsl:text>    return astChild(ast, </xsl:text><xsl:value-of select="position()-1"/><xsl:text>);
}
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template match="Child" mode="declare-setter">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>    void set</xsl:text>
      <xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>
      <xsl:text>(AstNode &amp; ast, int child, AstNode &amp; newChild);
</xsl:text>
    </xsl:when>
  <xsl:otherwise>
    <xsl:text>    void set</xsl:text>
    <xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>
    <xsl:text>(AstNode &amp; ast, AstNode &amp; newChild);
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="define-setter">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
<xsl:text>
void set</xsl:text><xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/><xsl:text>(AstNode &amp; ast, int child, AstNode &amp; newChild)
{
    assert(</xsl:text><xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
    // The variable length child is at position </xsl:text><xsl:value-of select="position()-1"/>
      <xsl:choose>
        <xsl:when test="position() &lt; 2"></xsl:when>
        <xsl:otherwise>
<xsl:text>
    child = child + </xsl:text><xsl:value-of select="position()-1"/><xsl:text>;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
<xsl:text>
    assert(0 &lt;= child &amp;&amp; child &lt; astSize(ast));
    AstPool::instance()->setChild(ast, child, newChild);
    assert(&amp;newChild == &amp;astChild(ast, child));</xsl:text>
<xsl:text>
}
</xsl:text>
    </xsl:when>
    <xsl:otherwise>
<xsl:text>
void set</xsl:text><xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/><xsl:text>(AstNode &amp; ast, AstNode &amp; newChild)
{
    assert(</xsl:text><xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
</xsl:text>
  <xsl:if test="@optional">
    <xsl:text>    assert(</xsl:text><xsl:value-of select="position()-1"/><xsl:text> &lt; astSize(ast));
</xsl:text>
  </xsl:if>
  <xsl:text>    int child = </xsl:text><xsl:value-of select="position()-1"/><xsl:text>;
    assert(0 &lt;= child &amp;&amp; child &lt; astSize(ast));
    AstPool::instance()->setChild(ast, child, newChild);
    assert(&amp;newChild == &amp;astChild(ast, child));
}
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template match="Child" mode="declare-count">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>    int </xsl:text><xsl:value-of select="@name"/><xsl:text>Count(AstNode &amp; ast);
</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="define-count">
  <xsl:choose>
    <xsl:when test="@repeat = 'true'">
      <xsl:text>
int </xsl:text><xsl:value-of select="@name"/><xsl:text>Count(AstNode &amp; ast)
{
    assert</xsl:text>(<xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
    // The variable length child is at position </xsl:text><xsl:value-of select="position()-1"/>
      <xsl:choose>
        <xsl:when test="position() &lt; 2"><xsl:text>
    return astSize(ast);</xsl:text>
        </xsl:when>
        <xsl:otherwise>
<xsl:text>
    return astSize(ast) - </xsl:text><xsl:value-of select="position()-1"/><xsl:text>;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
<xsl:text>
}
</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template match="Child" mode="declare-has">
  <xsl:if test="@optional">
    <xsl:text>    bool has_</xsl:text><xsl:value-of select="@name"/><xsl:text>(AstNode &amp; ast);
</xsl:text>
</xsl:if>
</xsl:template>

<xsl:template match="Child" mode="define-has">
  <xsl:if test="@optional">
<xsl:text>
bool has_</xsl:text><xsl:value-of select="@name"/><xsl:text>(AstNode &amp; ast)
{
    assert(</xsl:text><xsl:value-of select="../@tokenType"/><xsl:text> == ast.getType(&amp;ast));
    return </xsl:text><xsl:value-of select="position()-1"/><xsl:text> &lt; astSize(ast);
}
</xsl:text>
</xsl:if>
</xsl:template>

<xsl:template match="AstNodes" mode="cpp"><xsl:message>Generating AstNode cpp include</xsl:message>// This is a generated file.  Any changes made locally will be lost
  <xsl:call-template name="open-namespace-spl"/>
  <xsl:call-template name="define-base-class-methods"/>
  <xsl:call-template name="define-generic-class-methods"/>
  <xsl:apply-templates select="AstNode" mode="define-class-methods"/>
  <xsl:call-template name="open-namespace-ast"/>
  <xsl:apply-templates select="AstNode[count(@noAstNamespace) = 0]" mode="define-namespaces"/>
  <xsl:call-template name="close-namespace-ast"/>
  <xsl:call-template name="close-namespace-spl"/>
</xsl:template>

<xsl:template match="AstNode" mode="class-name">
  <xsl:text>Ast</xsl:text>
  <xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>
</xsl:template>

<xsl:template match="AstNode" mode="enum-name">
  <xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>
</xsl:template>

<xsl:template match="AstNode" mode="enum-decl">
  <xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/>
  <xsl:value-of select="substring(@name, 2)"/><xsl:text>,</xsl:text>
  <xsl:if test="position() != last()"><xsl:text>
               </xsl:text>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
