<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   version="1.0"
   xmlns:a="http://www.ibm.com/xmlns/prod/streams/spl/annotations"
   xmlns:xalan="http://xml.apache.org/xslt">

   <xsl:output indent="no" method="text"/>
   <xsl:strip-space elements="*"/>

   <xsl:template match="/">
      <xsl:apply-templates select="a:annotations"/>
   </xsl:template>

   <xsl:template match="a:annotations">{
    SupportedAnnotation * a;<xsl:apply-templates select="a:annotation[count(@internal) = 0 or @internal='false']"/>
}
</xsl:template>

   <xsl:template match="a:annotation">

    a = annotationPool.construct("<xsl:value-of select="./@name"/>", <xsl:choose><xsl:when test="@maxOccurs = 'unbounded'"><xsl:text>-1</xsl:text></xsl:when><xsl:otherwise><xsl:value-of select="@maxOccurs"/></xsl:otherwise></xsl:choose>, <xsl:choose><xsl:when test="@standaloneSupported"><xsl:value-of select="./@standaloneSupported"/></xsl:when><xsl:otherwise><xsl:text>false</xsl:text></xsl:otherwise></xsl:choose>);
    <xsl:apply-templates select="a:key"/>_supportedAnnotations.insert(make_pair("<xsl:value-of select="./@name"/>", a));</xsl:template>

    <xsl:template match="a:key">
        <xsl:choose>
            <xsl:when test='./@submissionTimeValueAllowed'>a->addKey("<xsl:value-of select="./@name"/>", <xsl:value-of select="./@required"/>, <xsl:value-of select="./@submissionTimeValueAllowed"/>);
    </xsl:when>
            <xsl:otherwise>a->addKey("<xsl:value-of select="./@name"/>", <xsl:value-of select="./@required"/>, false);
    </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>
