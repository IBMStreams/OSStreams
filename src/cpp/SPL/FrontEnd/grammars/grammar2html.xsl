<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   version="1.0"
   xmlns:xalan="http://xml.apache.org/xslt">

   <xsl:output indent="no" method="text"/>
   <xsl:strip-space elements="*"/>

   <xsl:template match="/">
      <!--xsl:call-template name="header"/-->
      <xsl:apply-templates select="grammar"/>
      <!--xsl:call-template name="trailer"/-->
   </xsl:template>

<xsl:variable name="lcalpha" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="ucalpha" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="grammar">
   <xsl:apply-templates select="production"/>
   <xsl:apply-templates select="fragment"/>
</xsl:template>

<xsl:template match="production">
  <xsl:apply-templates select="." mode="production-name"/> : <xsl:apply-templates select="child::*" mode="rule"></xsl:apply-templates><xsl:text>

</xsl:text>
</xsl:template>

<xsl:template match="production" mode="production-name"><xsl:value-of select="./@name"/></xsl:template>

<xsl:template match="sequence" mode="rule">
  <xsl:apply-templates select="child::*" mode="rule"></xsl:apply-templates>
</xsl:template>


<xsl:template match="nonTerminal" mode="rule"><xsl:apply-templates select="name" mode="rule"/><xsl:apply-templates select="args" mode="rule"/><xsl:apply-templates select="@repetition"/></xsl:template>
<xsl:template match="name" mode="rule"><xsl:text> </xsl:text><xsl:value-of select="."/></xsl:template>
<xsl:template match="args" mode="rule">[<xsl:value-of select="."/>]</xsl:template>

<xsl:template match="terminal" mode="rule">'<xsl:value-of select="."/>'</xsl:template>
<xsl:template match="specialTerminal" mode="rule"><xsl:text> </xsl:text><xsl:value-of select="."/></xsl:template>
<xsl:template match="terminalRange" mode="rule"><xsl:apply-templates select="child::*[position()=1]" mode="rule"/>..<xsl:apply-templates select="child::*[position()=2]" mode="rule"/></xsl:template>

<xsl:template match="choice" mode="rule"><xsl:apply-templates select="child::*[position()=1]" mode="rule"/>
<xsl:for-each select="child::*[position()!=1]">|<xsl:apply-templates select="." mode="rule"/>
</xsl:for-each>
</xsl:template>

<xsl:template match="group" mode="rule"> (<xsl:apply-templates select="child::*" mode="rule"/>)<xsl:apply-templates select="@repetition"/> </xsl:template>
<xsl:template match="dot" mode="rule"> .<xsl:apply-templates select="@repetition"/> </xsl:template>
<xsl:template match="syntacticPredicate" mode="rule"> (<xsl:apply-templates select="child::*" mode="rule"/> )=></xsl:template>
<xsl:template match="semiKeyword" mode = "rule"> semiKeyword["<xsl:value-of select="."/>"]</xsl:template>
<xsl:template match="not" mode="rule">~<xsl:apply-templates select="child::*" mode="rule"/></xsl:template>

<xsl:template match="fragment" mode="production-name"><xsl:value-of select="./@name"/></xsl:template>
<xsl:template match="fragment">
fragment   <xsl:apply-templates select="." mode="production-name"/> : <xsl:apply-templates select="child::*" mode="rule"></xsl:apply-templates><xsl:text>
</xsl:text>
</xsl:template>

</xsl:stylesheet>
