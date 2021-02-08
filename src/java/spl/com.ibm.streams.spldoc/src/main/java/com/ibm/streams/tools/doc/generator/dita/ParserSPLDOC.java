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

package com.ibm.streams.tools.doc.generator.dita;

import com.ibm.streams.tools.doc.messages.Messages;
import com.ibm.streams.tools.doc.utilities.Logger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Parser for the SPLDOC markup in the description of an SPL artifact or its properties.
 *
 * <p>IMPORTANT NOTE: The StreamsStudio IDE uses the parser code for displaying hover text from
 * SPLDOC markup text, including SPLDOC annotations. Therefore the IDE code includes a copy of this
 * file, and any changes must be made there also. In order to allow this use, the parser is designed
 * to be independent of other SPLDOC classes.
 */
public class ParserSPLDOC {

  private static final Logger logger = Logger.getInstance();

  // Enumeration for node types of SPLDOC elements.
  public enum SPLDOCElementType {
    Root,
    NewPage,
    Section,
    Title,
    Paragraph,
    Text,
    CodeBlock,
    OList,
    OListItem,
    UList,
    UListItem,
    Bold,
    Italics,
    Monospace,
    Link,
    LinkURL,
    LinkLabel,
    Image,
    ImageURL,
    ImageLabel,
    Table,
    TableGroup,
    TableRows,
    TableRow,
    TableEntry,
    Error
  };

  // A node in the SPLDOC syntax tree. Represents the structure of the SPLDOC markup in a
  // description.
  public class SPLDOCElementNode {
    SPLDOCElementNode parent = null;
    SPLDOCElementType type;
    String text = null;
    int lineOffset;
    List<SPLDOCElementNode> children = new ArrayList<SPLDOCElementNode>();

    SPLDOCElementNode(SPLDOCElementType type, ParseState state) {
      this.type = type;
      this.lineOffset = state.lineOffset;
    }

    SPLDOCElementNode addChild(SPLDOCElementNode child) {
      child.parent = this;
      children.add(child);
      return child;
    }

    SPLDOCElementNode newChild(SPLDOCElementType type, ParseState state) {
      SPLDOCElementNode child = new SPLDOCElementNode(type, state);
      addChild(child);
      return child;
    }

    SPLDOCElementNode newChild(SPLDOCElementType type, String text, ParseState state) {
      SPLDOCElementNode child = newChild(type, state);
      child.text = text;
      return child;
    }

    SPLDOCElementNode newChild(SPLDOCErrorType errorType, ParseState state) {
      SPLDOCElementNode child = new SPLDOCErrorNode(errorType, state);
      addChild(child);
      return child;
    }

    SPLDOCElementNode newChild(SPLDOCErrorType errorType, String text, ParseState state) {
      SPLDOCElementNode child = newChild(errorType, state);
      child.text = text;
      return child;
    }

    public List<SPLDOCElementNode> getChildren() {
      return children;
    }

    public List<SPLDOCElementNode> getChildren(SPLDOCElementType type) {
      List<SPLDOCElementNode> childOfType = new ArrayList<SPLDOCElementNode>();
      for (SPLDOCElementNode node : children) {
        if (node.type == type) {
          childOfType.add(node);
        }
      }
      return childOfType;
    }

    public boolean isEmpty() {
      return getChildren().isEmpty();
    }

    public SPLDOCElementNode getParent() {
      return parent;
    }

    public SPLDOCElementNode getAncestor(SPLDOCElementType type) {
      SPLDOCElementNode node = parent;
      while (node != null && node.type != type) {
        node = node.parent;
      }
      return node;
    }
    // Print for testing and debugging.
    public void print() {
      System.out.println();
      print(this, 0);
    }

    private void print(SPLDOCElementNode node, int level) {
      for (int ix = 0; ix < level; ix++) {
        System.out.print(" ");
      }
      node.printNode();
      System.out.println();
      for (SPLDOCElementNode child : node.children) {
        print(child, level + 1);
      }
    }

    protected void printNode() {
      System.out.print(this.type + ", (" + (this.text != null ? this.text : "") + ")");
    }
  }

  // Enumeration for the types of error in SPLDOC markup text that are detected by the parser.
  public enum SPLDOCErrorType {
    InternalError,
    InvalidInlineMarkupError,
    NoTitleError,
    ReservedInlineError,
    ReservedNestedInlineError,
    ReservedNewlineError,
    ReservedTitleError,
    TableEndError,
    TableSyntaxError,
    TableEOLSyntaxError,
    TableTooFewColumnsError,
    TableTooManyColumnsError,
    TableMixedColumnBoundaryError,
    TableNoColsRowBoundaryError,
    TableShortRowBoundaryError
  };

  // A node in the SPLDOC syntax tree signifying an error in the SPLDOC markup text.
  public class SPLDOCErrorNode extends SPLDOCElementNode {
    SPLDOCErrorType errorType;

    SPLDOCErrorNode(SPLDOCErrorType errorType, ParseState state) {
      super(SPLDOCElementType.Error, state);
      this.errorType = errorType;
    }

    // Print for testing and debugging.
    @Override
    protected void printNode() {
      super.printNode();
      System.out.print(", " + this.errorType.toString());
    }
  }

  // A node in the SPLDOC syntax tree for a table.
  public class SPLDOCTableNode extends SPLDOCElementNode {
    public boolean hasFirstColumnRowHeader = false;

    SPLDOCTableNode(ParseState state) {
      super(SPLDOCElementType.Table, state);
    }

    // Print for testing and debugging.
    @Override
    protected void printNode() {
      super.printNode();
      if (this.hasFirstColumnRowHeader) {
        System.out.print(", has row header");
      }
    }
  }

  // A node in the SPLDOC syntax tree for a group of rows in a table
  // that comprise either a column header or the body of a table.
  public class SPLDOCTableRowsNode extends SPLDOCElementNode {
    public boolean isColumnHeader = false;

    SPLDOCTableRowsNode(ParseState state) {
      super(SPLDOCElementType.TableRows, state);
    }

    // Print for testing and debugging.
    @Override
    protected void printNode() {
      super.printNode();
      if (this.isColumnHeader) {
        System.out.print(", column header");
      }
    }
  }

  // Enumeration for the horizontal alignment of text in a table column entry.
  public enum SPLDOCHorizontalAlignment {
    Left,
    Right,
    Center,
    Default
  };

  // Enumeration for the vertical alignment of text in a table column entry.
  public enum SPLDOCVerticalAlignment {
    Top,
    Bottom,
    Middle,
    Default
  };

  // A node in the SPLDOC syntax tree for a column entry in a table.
  public class SPLDOCTableEntryNode extends SPLDOCElementNode {
    public SPLDOCHorizontalAlignment hAlign = SPLDOCHorizontalAlignment.Default;
    public SPLDOCVerticalAlignment vAlign = SPLDOCVerticalAlignment.Default;
    public int moreRows = 0; // Additional rows, if spanning rows.
    public int colStart; // Starting column (0 = first column).
    public int colEnd; // Ending column (for column spanning).

    SPLDOCTableEntryNode(ParseState state) {
      super(SPLDOCElementType.TableEntry, state);
      this.colStart = state.tableState.colNum;
      this.colEnd = state.tableState.colNum;
    }

    // Print for testing and debugging.
    @Override
    protected void printNode() {
      super.printNode();
      System.out.print(
          ", cols="
              + colStart
              + ":"
              + colEnd
              + ", morerows:"
              + moreRows
              + ", H:"
              + hAlign
              + ", V:"
              + vAlign);
    }
  }

  // Recognized annotation tag names in SPLDOC markup text that have targets
  // (such as the name of a parameter), as well as descriptions.
  private static final List<String> AnnotationTagsWithTargets =
      Arrays.asList(new String[] {"param", "throws"});

  // Enumeration for the types of error in SPLDOC annotations that are detected by the parser.
  public enum SPLDOCAnnotationErrorType {
    NoTagNameError,
    NoTargetError
  };

  // An annotation extracted from SPLDOC markup text.
  // Annotations take the form:
  //   @<name> [<target>] [<description>]
  public class SPLDOCAnnotation {
    String name = ""; // Name of annotation tag.
    String target = null; // Target of annotation, for recognized annotation tags with targets.
    String description = null; // Optional description containing SPLDOC markup text.
    int lineOffset; // Offset of line containing annotation tag in source SPLDOC markup text.
    private SPLDOCAnnotationErrorType errorType = null;

    SPLDOCAnnotation(int lineOffset) {
      this.lineOffset = lineOffset;
    }

    public boolean hasError() {
      return (errorType != null);
    }

    public SPLDOCAnnotationErrorType getErrorType() {
      return errorType;
    }
  }

  // Defines the text to be used for a particular type of SPLDOC markup.
  class SPLDOCMarkup {
    private SPLDOCElementType type;
    String markup_open = null;
    String markup_close = null;
    private int length = -1; // Length of string that defines this type of markup.
    private int end = -1; // Position at end of substring that equals() markup.
    private Pattern markupPattern = null;

    SPLDOCMarkup(String markup, SPLDOCElementType type) {
      this.markup_open = markup;
      this.markup_close = markup;
      // Length open is assumed to match length close.
      this.length = markup_open.length();
      this.type = type;
    }

    SPLDOCMarkup(String markup_open, String markup_close, SPLDOCElementType type) {
      this.markup_open = markup_open;
      this.markup_close = markup_close;
      // Length open is assumed to match length close.
      this.length = markup_open.length();
      this.type = type;
    }

    SPLDOCMarkup(SPLDOCElementType type, String regex) {
      this.markupPattern = Pattern.compile(regex);
      this.type = type;
    }

    SPLDOCMarkup(SPLDOCElementType type, String regex, int flags) {
      this.markupPattern = Pattern.compile(regex, flags);
      this.type = type;
    }

    boolean equals(String s, int position) {
      boolean res;
      if (markupPattern != null) {
        // Restrict match to current line only (including new line).
        int ix = s.indexOf('\n', position);
        s = s.substring(position, ix >= 0 ? ix + 1 : s.length());
        Matcher matcher = markupPattern.matcher(s);
        res = matcher.lookingAt();
        end = (res ? position + matcher.end() : -1);
        return res;
      }
      res = s.startsWith(markup_open, position);
      end = (res ? position + markup_open.length() : -1);
      return res;
    }

    boolean equals(String s) {
      return equals(s, 0);
    }
  }

  // Markup that affects the appearance of the text and is in line with other text (in-line markup).
  // These ASCII characters are out of the range of the Unicode surrogate characters that could
  // occur in localized
  // strings.
  private static final SPLDOCMarkup Bold =
      (new ParserSPLDOC()).new SPLDOCMarkup("**", SPLDOCElementType.Bold);
  private static final SPLDOCMarkup Italics =
      (new ParserSPLDOC()).new SPLDOCMarkup("*", SPLDOCElementType.Italics);
  private static final SPLDOCMarkup Monospace =
      (new ParserSPLDOC()).new SPLDOCMarkup("`", SPLDOCElementType.Monospace);
  static final SPLDOCMarkup Link =
      (new ParserSPLDOC()).new SPLDOCMarkup("[", "]", SPLDOCElementType.Link);
  static final SPLDOCMarkup LinkLabel =
      (new ParserSPLDOC()).new SPLDOCMarkup("|", SPLDOCElementType.LinkLabel);
  static final SPLDOCMarkup Image =
      (new ParserSPLDOC()).new SPLDOCMarkup("{", "}", SPLDOCElementType.Image);
  static final SPLDOCMarkup ImageLabel =
      (new ParserSPLDOC()).new SPLDOCMarkup("|", SPLDOCElementType.ImageLabel);

  // Markup that controls the structure of the document and starts on a new line (new-line markup).
  // New page, formatted as: "+ <title>", "++ <title>", etc, with one "+" per nesting level.
  private static final SPLDOCMarkup NewPage =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(SPLDOCElementType.NewPage, "\\s*\\++\\s", Pattern.DOTALL);
  // Section heading, formatted as "# <title>".
  private static final SPLDOCMarkup Section =
      (new ParserSPLDOC()).new SPLDOCMarkup(SPLDOCElementType.Section, "\\s*#\\s", Pattern.DOTALL);
  // Paragraph, formatted as a blank line, followed by text.
  private static final SPLDOCMarkup Paragraph =
      (new ParserSPLDOC()).new SPLDOCMarkup(SPLDOCElementType.Paragraph, "\\s*$");
  // Code block, formatted as 4 or more spaces, or 1 or more tabs, followed by text.
  private static final SPLDOCMarkup CodeBlock =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(SPLDOCElementType.CodeBlock, "( {4,}|\\t{1,})", Pattern.DOTALL);
  // Ordered list, formatted as "   <number>. <text>".
  private static final SPLDOCMarkup OListItem =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(SPLDOCElementType.OListItem, "\\s*\\d+\\.\\s", Pattern.DOTALL);
  // Unnumbered list, formatted as "   * <text>".
  private static final SPLDOCMarkup UListItem =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(SPLDOCElementType.UListItem, "\\s*\\*\\s", Pattern.DOTALL);
  // Annotation, terminates any previous description, formatted as "@<name> [<target>]
  // [<description>]".
  private static final SPLDOCMarkup Annotation =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*@", Pattern.DOTALL);
  // Comment, a line of text that will not be included in the output, formatted as "! <comment
  // text>".
  private static final SPLDOCMarkup Comment =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*!\\s+", Pattern.DOTALL);
  // Table title, formatted as "--- <title>".
  private static final SPLDOCMarkup TableTitle =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*\\-\\-\\-\\s+\\S", Pattern.DOTALL);
  // Table border to start a table, formatted as "---", or "|---" with optional ":".
  private static final SPLDOCMarkup TableStart =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(null, "\\s*(\\|[\\-:][\\-:][\\-:]|\\-\\-\\-)", Pattern.DOTALL);
  // Table border to end a table, formatted as "---".
  private static final SPLDOCMarkup TableEnd =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*\\-\\-\\-", Pattern.DOTALL);
  // Table column header boundary, formatted as "|===".
  private static final SPLDOCMarkup TableColumnHeaderBoundary =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*\\|[\\=:][\\=:][\\=:]", Pattern.DOTALL);
  // Table row content, formatted as "|" or ":", followed by row content or a row boundary.
  private static final SPLDOCMarkup TableRowContentStart =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*[\\|:]", Pattern.DOTALL);
  // The end of table row content, formatted as "|" at the end of the line.
  private static final SPLDOCMarkup TableRowContentEnd =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\|\\s*$", Pattern.DOTALL);
  // Table column boundary that starts the current line of an entry.
  private static final SPLDOCMarkup TableEntryStart =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "[\\|\\+:]\\s+\\S", Pattern.DOTALL);
  // Table column boundary that ends the current line of an entry.
  // The column boundary must be followed by a row boundary, another entry, or the end of row
  // content.
  private static final SPLDOCMarkup TableEntryEnd =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(
          null, "\\s+([\\|\\+][:\\-][:\\-]\\-|[\\|\\+:]\\s|\\|\\s*$)", Pattern.DOTALL);
  // Table column boundary that starts a row boundary between entries.
  private static final SPLDOCMarkup TableRowBoundary =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "[\\|\\+][\\-:][\\-:][\\-:]", Pattern.DOTALL);

  // Characters reserved for possible future markup.
  // Reserved for in-line markup.
  private static final SPLDOCMarkup ReservedInline =
      (new ParserSPLDOC())
      .new SPLDOCMarkup(null, "(\\*\\*\\*\\*|``|\\[\\[|\\{\\{)", Pattern.DOTALL);
  // Reserved for new-line markup (note this reserves "!!", "## ", etc).
  private static final SPLDOCMarkup ReservedNewline =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*([!%=\\^\\$\\-]|#+\\s)", Pattern.DOTALL);
  // Reserved for markup that precedes a section or new page title, eg to specify a name to be used
  // in links.
  private static final SPLDOCMarkup ReservedTitle =
      (new ParserSPLDOC()).new SPLDOCMarkup(null, "\\s*[!@#%=\\^\\$\\-\\+]", Pattern.DOTALL);

  // Indentation used to define a code block.
  private static final String CodeBlockSpaceIndent = "    ";
  private static final String CodeBlockTabIndent = "\t";

  // Defines the current parsing state.
  private class ParseState {
    private String doc;
    private int length;
    private int startIndent = 0;
    private int pos = 0;
    private int lineOffset;
    private int fontLevel = 0;
    private int listLevel = 0;
    private int pageLevel = 0;
    private boolean inTitle = false;
    private TableState tableState = null;

    private ParseState(String doc, int lineOffset) {
      this.doc = doc;
      this.lineOffset = lineOffset;
      length = doc.length();
      // Move to the first line of text, if any, and set the starting indentation.
      skipCommentsAndBlankLines(this, -1);
      startIndent = getIndent(doc, pos);
    }

    private ParseState(String doc) {
      this(doc, 0);
    }
  }

  // Parsing states when parsing a table.
  private enum TableParseState {
    Start,
    TitleFetched,
    DefiningColumns,
    FetchingRows,
    End
  };

  // Defines the current state when parsing a table.
  private class TableState {
    private TableParseState state = TableParseState.Start;
    private SPLDOCTableNode tableNode = null;
    private boolean hasColumnHeader = false;
    private SPLDOCElementNode group = null;
    private SPLDOCTableRowsNode rows = null;
    private SPLDOCElementNode row = null;
    private TableRowState rowState = null;
    private ArrayList<TableColState> colStates = null;
    private TableColState colState = null;
    private int colNum = -1;
  }

  // Defines the state of the current line of row content when parsing a table.
  private class TableRowState {
    private boolean newRow = false;
    private boolean rowError = false;
  }

  // Defines the current state for a column when parsing a table.
  private class TableColState {
    private SPLDOCHorizontalAlignment hAlign = SPLDOCHorizontalAlignment.Default;
    private SPLDOCTableEntryNode entry = null;
    private StringBuilder doc = null;
    private int lineOffset = 0;
  }

  // Output SPLDOC syntax tree.
  private static SPLDOCElementNode root;

  // Text that is inserted into the output to identify a markup usage error.
  private static final String errorText = /*"!!!!!!ERROR:!!!!!!"*/
      logger.getMessage(Messages.MSG_ERROR);

  /**
   * Return true if the given character is whitespace, otherwise false. As written this is expected
   * to support localization.
   */
  public static boolean isWhitespace(char c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
      return true;
    }
    return false;
  }

  /**
   * Return the position of the next whitespace character in the given string, or the end of the
   * string, whichever comes first, starting from the given position.
   */
  private static int getNextWhitespaceOrEnd(String s, int pos) {
    while (pos < s.length()) {
      if (isWhitespace(s.charAt(pos))) {
        return pos;
      }
      pos++;
    }
    return pos;
  }

  /**
   * Return the position of the next non-whitespace character in the given string, starting from the
   * given position, or -1, if none.
   */
  private static int getNextNonWhitespace(String s, int pos) {
    while (pos < s.length()) {
      if (!isWhitespace(s.charAt(pos))) {
        return pos;
      }
      pos++;
    }
    return -1;
  }

  /**
   * Return the position of the first trailing whitespace character at the end of the current line,
   * or the new line character, in the given string, or the end of the string, whichever comes
   * first, starting from the given position.
   */
  private static int getEndOfLineWhitespace(String s, int pos) {
    int whitePos = -1;
    while (pos < s.length()) {
      char c = s.charAt(pos);
      if (c == '\n') {
        break;
      }
      if (isWhitespace(c)) {
        if (whitePos == -1) {
          whitePos = pos;
        }
      } else {
        if (whitePos != -1) {
          whitePos = -1;
        }
      }
      pos++;
    }
    if (whitePos == -1) {
      whitePos = pos;
    }
    return whitePos;
  }

  /**
   * Return the number of characters used to indent a line of text in the given string, where the
   * line starts at the given position.
   */
  private static int getIndent(String s, int pos) {
    int indent = 0;
    while (pos < s.length()) {
      if (isWhitespace(s.charAt(pos)) && !(s.charAt(pos) == '\n')) {
        indent++;
      } else {
        return indent;
      }
      pos++;
    }
    return indent;
  }

  /** Return the number of characters used to indent a line of text. */
  private static int getIndent(String line) {
    return getIndent(line, 0);
  }

  /**
   * Return the given indentation, adjusted so that it is not less than the starting indentation.
   *
   * <p>This is necessary to make the processing of SPLDOC descriptions in XML model files
   * consistent with the processing of SPLDOC descriptions in SPL files, since the SPLDOC model
   * builder that processes SPL files strips off any leading whitespace characters up to the
   * starting indentation.
   */
  private static int adjustIndent(int indent, ParseState state) {
    return (indent < state.startIndent ? state.startIndent : indent);
  }

  /**
   * Return the remainder of the current line, starting at the given position, including the newline
   * character, if present.
   */
  private static String getLine(int position, ParseState state) {
    int ix = state.doc.indexOf('\n', position);
    return state.doc.substring(position, ix < 0 ? state.length : ix + 1);
  }

  /**
   * Return the entire current line containing the given position, including the ending newline
   * character, if present.
   */
  private static String getWholeLine(int position, ParseState state) {
    int start = position;
    while (start > 0 && state.doc.charAt(start - 1) != '\n') {
      start--;
    }
    int end = state.doc.indexOf('\n', position);
    return state.doc.substring(start, end < 0 ? state.length : end + 1);
  }

  /** Skip the remainder of the current line, starting at the current position. */
  private static void skipLine(ParseState state) {
    while (state.pos < state.length) {
      if (state.doc.charAt(state.pos) == '\n') {
        state.pos++;
        state.lineOffset++;
        return;
      }
      state.pos++;
    }
  }

  /**
   * Skip any comment lines in the input, starting at the current position, which must be at the
   * beginning of the next line of text. Caller must set the base indent negative if the next line
   * of non-comment text cannot belong to a code block.
   */
  private static void skipComments(ParseState state, int baseIndent) {
    while (state.pos < state.length) {
      String doc_line = getLine(state.pos, state);
      if (baseIndent >= 0
          && getIndent(doc_line) > baseIndent
          && CodeBlock.equals(doc_line.substring(baseIndent))) {
        // Next line is part of a code block.
        return;
      }
      if (!Comment.equals(doc_line)) {
        // Next line is not a comment.
        return;
      }
      // Next line is a comment.
      state.pos += doc_line.length();
      state.lineOffset++;
    }
  }

  /**
   * Skip any comment and blank lines in the input, starting at the current position, which must be
   * at the beginning of the next line of text. Caller must set the base indent negative if the next
   * line of non-comment text cannot belong to a code block.
   */
  private static void skipCommentsAndBlankLines(ParseState state, int baseIndent) {
    while (state.pos < state.length) {
      skipComments(state, baseIndent);
      if (state.pos >= state.length) {
        return;
      }
      String doc_line = getLine(state.pos, state);
      if (!Paragraph.equals(doc_line)) {
        return;
      }
      state.pos += doc_line.length();
      state.lineOffset++;
    }
  }

  /**
   * Return true if the next line of input that starts at the given position, indicates the end of
   * the current type of text processing, otherwise false.
   */
  private static boolean isEndOfCurrentText(int position, ParseState state, int baseIndent) {
    String doc_line = getLine(position, state);
    int indent = getIndent(doc_line);
    // Test for the following conditions.
    return ( // End of a title being parsed.
    state.inTitle
        ||
        // New-line markup.
        Paragraph.equals(doc_line)
        || (indent > baseIndent && CodeBlock.equals(doc_line.substring(baseIndent)))
        || NewPage.equals(doc_line)
        || Section.equals(doc_line)
        || OListItem.equals(doc_line)
        || UListItem.equals(doc_line)
        || TableTitle.equals(doc_line)
        || TableStart.equals(doc_line)
        || (ReservedNewline.equals(doc_line) && !Comment.equals(doc_line))
        || Annotation.equals(doc_line)
        ||
        // End of a current list.
        (state.listLevel > 0 && indent <= baseIndent));
  }

  /**
   * If the next characters are the opening characters for font markup, return the markup type,
   * otherwise return null. The font markup characters must be followed by a non-whitespace
   * character.
   */
  private static SPLDOCMarkup getFontMarkup(ParseState state) {
    SPLDOCMarkup type = null;
    if (Bold.equals(state.doc, state.pos)) {
      type = Bold;
    } else if (Italics.equals(state.doc, state.pos)) {
      type = Italics;
    } else if (Monospace.equals(state.doc, state.pos)) {
      type = Monospace;
    }
    if (type != null) {
      int nextPos = state.pos + type.length;
      if (nextPos < state.length && !isWhitespace(state.doc.charAt(nextPos))) {
        return type;
      }
    }
    return null;
  }

  /**
   * If the next characters are the opening characters for link or image markup, return the markup
   * type, otherwise return null.
   */
  private static SPLDOCMarkup getLinkOrImageMarkup(ParseState state) {
    if (state.inTitle) {
      return null;
    }
    if (Link.equals(state.doc, state.pos)) {
      return Link;
    } else if (Image.equals(state.doc, state.pos)) {
      return Image;
    }
    return null;
  }

  /**
   * If the next character is escaped, return the escaped character and any consecutive escaped
   * characters on the same line, and advance the document position. If an escape is at the end of a
   * line, ignore the newline and continue reading the next line.
   */
  private static String parseEscapes(ParseState state) {
    String escaped_chars = "";
    while (state.pos < state.length) {
      if (state.doc.charAt(state.pos) == '\\') {
        if (++state.pos == state.length) {
          break;
        }
        if (state.doc.charAt(state.pos) != '\n') {
          escaped_chars += state.doc.charAt(state.pos);
        } else {
          state.lineOffset++;
        }
        state.pos++;
      } else {
        break;
      }
    }
    return escaped_chars;
  }

  /** Add a text node containing the given block of text to the given node. */
  private static void addTextNode(SPLDOCElementNode node, String plain_text, ParseState state) {
    if (plain_text.length() > 0) {
      node.newChild(SPLDOCElementType.Text, plain_text, state);
    }
  }

  /**
   * Process an input line of code block text as follows: - the indentation used to define a line of
   * code block is removed - to allow a code block line to start with "@", without being processed
   * as an annotation, translate an escape sequence for the initial "@" or "\" character.
   */
  private static String processCodeBlockLine(String codeline, int baseIndent) {
    // Remove indentation.
    if (codeline.length() > baseIndent) {
      if (codeline.substring(baseIndent).startsWith(CodeBlockSpaceIndent)) {
        codeline = codeline.substring(baseIndent + CodeBlockSpaceIndent.length());
      } else if (codeline.substring(baseIndent).startsWith(CodeBlockTabIndent)) {
        codeline = codeline.substring(baseIndent + CodeBlockTabIndent.length());
      }
    }
    // Translate an escaped initial character.
    int pos = getNextNonWhitespace(codeline, 0);
    if (pos >= 0
        && codeline.charAt(pos) == '\\'
        && pos < codeline.length()
        && (codeline.charAt(pos + 1) == '@' || codeline.charAt(pos + 1) == '\\')) {
      codeline = codeline.substring(0, pos) + codeline.substring(pos + 1, codeline.length());
    }
    return codeline;
  }

  /**
   * Return the end position of the code block that starts at the current position. The input code
   * block can contain blank lines that are part of the code block, and comment lines that are not.
   */
  private static int endOfCodeBlock(ParseState state, int baseIndent) {
    // Save original position.
    int startPos = state.pos;
    int startLineOffset = state.lineOffset;
    // Find end of code block.
    int endPos = startPos;
    while (state.pos < state.length) {
      skipCommentsAndBlankLines(state, baseIndent);
      if (state.pos >= state.length) {
        break;
      }
      String doc_line = getLine(state.pos, state);
      int nextLinePos = state.pos + doc_line.length();
      if (getIndent(doc_line) > baseIndent && CodeBlock.equals(doc_line.substring(baseIndent))) {
        // This is a code block line.
        endPos = nextLinePos;
      } else {
        // The is the end of the code block and any included blank lines.
        break;
      }
      state.pos = nextLinePos;
      state.lineOffset++;
    }
    state.pos = startPos;
    state.lineOffset = startLineOffset;
    return endPos;
  }

  /**
   * Parse a code block. A text node containing the resulting text is added to the given code block
   * node.
   */
  private static void parseCodeBlock(
      SPLDOCElementNode codeblock_node, ParseState state, int baseIndent) {
    StringBuilder codeline = new StringBuilder();
    int endPos = endOfCodeBlock(state, baseIndent);
    while (state.pos < endPos) {
      skipComments(state, baseIndent);
      if (state.pos >= endPos) {
        break;
      }
      String doc_line = getLine(state.pos, state);
      codeline.append(processCodeBlockLine(doc_line, baseIndent));
      state.pos += doc_line.length();
      state.lineOffset++;
    }
    // Remove any trailing new line (note: this interface is also used by the IDE).
    addTextNode(
        codeblock_node,
        codeline.substring(
            0, codeline.length() - (codeline.charAt(codeline.length() - 1) == '\n' ? 1 : 0)),
        state);
  }

  /**
   * Parse the input text for a title, and add the title to the given node. The title must exist, be
   * on a single line, and not start with characters reserved for future markup.
   */
  private static void parseTitle(SPLDOCElementNode current_node, ParseState state, int baseIndent) {
    // Find the start of the title.
    while (state.pos < state.length
        && isWhitespace(state.doc.charAt(state.pos))
        && state.doc.charAt(state.pos) != '\n') {
      state.pos++;
    }
    SPLDOCElementNode title_node = current_node.newChild(SPLDOCElementType.Title, state);
    String doc_line = getLine(state.pos, state);
    boolean error = false;
    if (state.doc.charAt(state.pos) == '\n') {
      // There is no title.
      current_node.newChild(SPLDOCErrorType.NoTitleError, state);
      error = true;
    } else if (ReservedTitle.equals(doc_line)) {
      // The title starts with characters reserved for future markup.
      current_node.newChild(SPLDOCErrorType.ReservedTitleError, doc_line.trim(), state);
      error = true;
    }
    if (error) {
      // Make the title an error string, plus the faulty title.
      title_node.newChild(SPLDOCElementType.Text, errorText + doc_line.trim(), state);
      state.pos += doc_line.length();
      state.lineOffset++;
      return;
    }
    state.inTitle = true;
    parseText(title_node, state, baseIndent);
    state.inTitle = false;
  }

  /**
   * Report the given error type at the current position in a line of table markup, and advance to
   * the next line.
   */
  private static void tableError(
      SPLDOCErrorType errorType, SPLDOCElementNode current_node, ParseState state) {
    // Report the error, and add an error string into the output.
    String doc_line = getLine(state.pos, state);
    current_node.newChild(SPLDOCElementType.Text, errorText + doc_line, state);
    // Report the remaining text, or the whole line if at the end of the line.
    int len = doc_line.length();
    doc_line = doc_line.trim();
    if (doc_line.isEmpty()) {
      doc_line = getWholeLine(state.pos, state).trim();
    }
    current_node.newChild(errorType, doc_line, state);
    state.pos += len;
    state.lineOffset++;
  }

  /**
   * Report a syntax error at the current position in a line of table markup, and advance to the
   * next line.
   */
  private static void tableSyntaxError(SPLDOCElementNode current_node, ParseState state) {
    if (getEndOfLineWhitespace(state.doc, state.pos) == state.pos) {
      tableError(SPLDOCErrorType.TableEOLSyntaxError, current_node, state);
    } else {
      tableError(SPLDOCErrorType.TableSyntaxError, current_node, state);
    }
  }

  /**
   * Report the given error type at the current position in a line of table markup for a row, and
   * advance to the next line.
   */
  private static void tableRowError(
      SPLDOCErrorType errorType, SPLDOCElementNode current_node, ParseState state) {
    tableError(errorType, current_node, state);
    state.tableState.rowState.rowError = true;
  }

  /**
   * Report a syntax error at the current position in a line of table markup for a row, and advance
   * to the next line.
   */
  private static void tableRowSyntaxError(SPLDOCElementNode current_node, ParseState state) {
    tableSyntaxError(current_node, state);
    state.tableState.rowState.rowError = true;
  }

  /** Add a row to a table. */
  private static void addRow(ParseState state) {
    state.tableState.row = state.tableState.rows.newChild(SPLDOCElementType.TableRow, state);
  }

  /** Add a column to a table. */
  private static void addColumn(ParseState state) {
    if (state.tableState.colStates == null) {
      state.tableState.colStates = new ArrayList<TableColState>();
    }
    state.tableState.colState = (new ParserSPLDOC()).new TableColState();
    state.tableState.colStates.add(state.tableState.colState);
    state.tableState.colNum++;
    addColEntry(state);
  }

  /** Add a column to a table where the current column entry spans the new column. */
  private static void addColumnWithSpan(ParseState state) {
    state.tableState.colStates.add((new ParserSPLDOC()).new TableColState());
    state.tableState.colState.entry.colEnd++;
    state.tableState.colNum++;
  }

  /** Return the number of columns in a table. */
  private static int getNumCols(ParseState state) {
    return state.tableState.colStates.size();
  }

  /** Prepare to move to the first column in a table. */
  private static void resetColumn(ParseState state) {
    state.tableState.colNum = -1;
  }

  /**
   * Move to the next column in a table.
   *
   * <p>Returns true if successful, or false if there are no more columns.
   */
  private static boolean nextColumn(ParseState state) {
    if (state.tableState.colNum + 1 >= getNumCols(state)) {
      return false;
    }
    state.tableState.colNum++;
    if (state.tableState.colNum == 0
        || state.tableState.colState.entry == null
        || state.tableState.colNum > state.tableState.colState.entry.colEnd) {
      state.tableState.colState = state.tableState.colStates.get(state.tableState.colNum);
    }
    return true;
  }

  /**
   * Move to the next column in a table where the current column entry spans the next column.
   *
   * <p>Returns true if successful, or false if there are no more columns.
   */
  private static boolean nextColumnWithSpan(ParseState state) {
    if (state.tableState.colNum + 1 >= getNumCols(state)) {
      return false;
    }
    state.tableState.colNum++;
    state.tableState.colState.entry.colEnd++;
    return true;
  }

  /** Return true if the next column in a table has an entry, otherwise false. */
  private static boolean nextColumnHasEntry(ParseState state) {
    if (state.tableState.colNum + 1 >= getNumCols(state)
        || state.tableState.colStates.get(state.tableState.colNum + 1).entry == null) {
      return false;
    }
    return true;
  }

  /**
   * Move to the next column entry in a table.
   *
   * <p>Returns true if successful, or false if there are no more entries.
   */
  private static boolean nextColEntry(ParseState state) {
    int colNum = state.tableState.colNum;
    if (colNum >= 0) {
      colNum = state.tableState.colState.entry.colEnd + 1;
    } else {
      colNum = 0;
    }
    if (colNum >= getNumCols(state)) {
      return false;
    }
    state.tableState.colNum = colNum;
    state.tableState.colState = state.tableState.colStates.get(colNum);
    return true;
  }

  /** Add a new entry to the current row and column in a table. */
  private static void addColEntry(ParseState state) {
    state.tableState.colState.entry = (new ParserSPLDOC()).new SPLDOCTableEntryNode(state);
    state.tableState.row.addChild(state.tableState.colState.entry);
    state.tableState.colState.doc = null;
  }

  /** End the entry that starts in the current column in a table, if any, due to a row boundary. */
  private static void endColEntry(ParseState state) {
    // Parse the text for the entry, if any, and add into the syntax tree.
    TableColState colState = state.tableState.colStates.get(state.tableState.colNum);
    if (colState.doc != null) {
      parse(
          colState.entry,
          (new ParserSPLDOC()).new ParseState(colState.doc.toString(), colState.lineOffset));
    }
    colState.entry = null;
    colState.doc = null;
  }

  /** Extend any column entries that span a new row. */
  private static void addRowSpans(ParseState state) {
    if (!state.tableState.rowState.newRow) {
      // A new row was not started.
      return;
    }
    resetColumn(state);
    while (nextColEntry(state)) {
      if (state.tableState.colState.doc != null) {
        // A column entry that was started in a previous row.
        state.tableState.colState.entry.moreRows++;
      }
    }
  }

  /**
   * Given alignment markup at the current position in a column boundary, set the vertical alignment
   * for the current column entry in a table.
   *
   * <p>Returns true if successful, otherwise false;
   */
  private static boolean setRowAlign(ParseState state) {
    if (state.tableState.colState.doc == null) {
      // The alignment character is in the first line for this row.
      state.tableState.colState.entry.vAlign = SPLDOCVerticalAlignment.Top;
    } else if (state.tableState.colState.entry.vAlign == SPLDOCVerticalAlignment.Default) {
      // This is the first alignment character for this row and not in the first line.
      state.tableState.colState.entry.vAlign = SPLDOCVerticalAlignment.Bottom;
    } else if (state.tableState.colState.entry.vAlign == SPLDOCVerticalAlignment.Top) {
      // This is the second alignment character for this row and not in the first line.
      state.tableState.colState.entry.vAlign = SPLDOCVerticalAlignment.Middle;
    } else {
      // Too many alignment characters.
      return false;
    }
    return true;
  }

  /**
   * Given alignment markup at the current position in a row boundary, set the horizontal alignment
   * for the current column entry in a table.
   *
   * <p>Returns true if successful, otherwise false;
   */
  private static boolean setColAlign(ParseState state) {
    if (state.doc.charAt(state.pos - 1) == '|' || state.doc.charAt(state.pos - 1) == '+') {
      // The alignment character is in the first position for this column.
      state.tableState.colState.entry.hAlign = SPLDOCHorizontalAlignment.Left;
    } else if (state.tableState.colState.entry.hAlign == SPLDOCHorizontalAlignment.Default) {
      // This is the first alignment character for this column and not at first position.
      state.tableState.colState.entry.hAlign = SPLDOCHorizontalAlignment.Right;
    } else if (state.tableState.colState.entry.hAlign == SPLDOCHorizontalAlignment.Left) {
      // This is the second alignment character for this column and not at first position.
      state.tableState.colState.entry.hAlign = SPLDOCHorizontalAlignment.Center;
    } else {
      // Too many alignment characters.
      return false;
    }
    return true;
  }

  /**
   * Set the default alignments for every row and column entry in a table.
   *
   * <p>If the vertical alignment for an entry has been specified by markup, set the new default for
   * the row to that alignment, otherwise set the alignment for the entry to the current default for
   * the row.
   *
   * <p>If the horizontal alignment for an entry has been specified by markup, set the new default
   * for the starting column to that alignment, otherwise set the alignment for the entry to the
   * current default for the column.
   */
  private static void setDefaultAlignments(ParseState state) {
    for (SPLDOCElementNode group :
        state.tableState.tableNode.getChildren(SPLDOCElementType.TableGroup)) {
      for (SPLDOCElementNode rows : group.getChildren(SPLDOCElementType.TableRows)) {
        for (SPLDOCElementNode row : rows.getChildren(SPLDOCElementType.TableRow)) {
          SPLDOCVerticalAlignment vAlign = SPLDOCVerticalAlignment.Default;
          for (SPLDOCElementNode node : row.children) {
            SPLDOCTableEntryNode entry = (SPLDOCTableEntryNode) node;
            if (entry.vAlign != SPLDOCVerticalAlignment.Default) {
              vAlign = entry.vAlign;
            } else if (vAlign != SPLDOCVerticalAlignment.Default) {
              entry.vAlign = vAlign;
            }
            TableColState colState = state.tableState.colStates.get(entry.colStart);
            if (entry.hAlign != SPLDOCHorizontalAlignment.Default) {
              colState.hAlign = entry.hAlign;
            } else if (colState.hAlign != SPLDOCHorizontalAlignment.Default) {
              entry.hAlign = colState.hAlign;
            }
          }
        }
      }
    }
  }

  /** Parse the title for a table. */
  private static void parseTableTitle(SPLDOCElementNode current_node, ParseState state) {
    if (state.tableState.state == TableParseState.Start) {
      // Advance position past the opening markup and parse the title.
      state.pos += 3;
      parseTitle(state.tableState.tableNode, state, -1);
      state.tableState.state = TableParseState.TitleFetched;
    } else {
      // The title must precede the table itself.
      tableSyntaxError(current_node, state);
    }
  }

  /**
   * Parse the upper border that starts a table.
   *
   * <p>There are two formats for the upper border: - one is the same as the lower border that ends
   * a table - the other includes characters to specify the columns and their alignments.
   */
  private static void parseTableStart(SPLDOCElementNode current_node, ParseState state) {
    // Start the table and add it to the current node.
    state.tableState.group =
        (new ParserSPLDOC()).new SPLDOCElementNode(SPLDOCElementType.TableGroup, state);
    state.tableState.tableNode.addChild(state.tableState.group);
    state.tableState.rows = (new ParserSPLDOC()).new SPLDOCTableRowsNode(state);
    state.tableState.group.addChild(state.tableState.rows);
    addRow(state);
    addColumn(state);
    current_node.addChild(state.tableState.tableNode);
    // Parse for correct characters and column specifications.
    int start = state.pos;
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    int colStart = start;
    boolean definingColumns = (state.doc.charAt(start) == '|');
    while (state.pos < end) {
      if (state.doc.charAt(state.pos) == '-') {
        // Continuation of border.
      } else if (!definingColumns) {
        // Border must not contain any other characters unless defining columns.
        tableSyntaxError(current_node, state);
        break;
      } else if (state.doc.charAt(state.pos) == '|') {
        // Normal column boundary.
        if (state.pos > start && state.pos < end - 1) {
          if (state.pos < colStart + 4) {
            // Row boundary must be at least 3 characters.
            tableError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
            break;
          }
          if (state.doc.charAt(state.pos + 1) == '|') {
            // The entry for the current column also spans the next column.
            addColumnWithSpan(state);
          } else {
            colStart = state.pos;
            addColumn(state);
          }
        }
      } else if (state.doc.charAt(state.pos) == '+') {
        // Row header column boundary.
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          break;
        }
        if (getNumCols(state) > 1) {
          // Row header can be only the first column.
          tableSyntaxError(current_node, state);
          break;
        }
        colStart = state.pos;
        addColumn(state);
        state.tableState.tableNode.hasFirstColumnRowHeader = true;
      } else if (state.doc.charAt(state.pos) == ':') {
        // Column alignment specifier.
        if (!setColAlign(state)) {
          // Invalid alignment specification.
          tableSyntaxError(current_node, state);
          break;
        }
      } else {
        // Invalid character.
        tableSyntaxError(current_node, state);
        break;
      }
      state.pos++;
      if (state.pos == end) {
        // End of border.
        if (definingColumns) {
          if (state.doc.charAt(end - 1) != '|') {
            // Invalid last character.
            tableSyntaxError(current_node, state);
            break;
          }
          if (state.pos - 1 < colStart + 4) {
            // Row boundary must be at least 3 characters.
            state.pos--;
            tableError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
            break;
          }
        }
        // Skip trailing whitespace.
        skipLine(state);
        break;
      }
    }
    if (definingColumns) {
      // The columns have been defined in the upper border.
      state.tableState.state = TableParseState.FetchingRows;
    } else {
      // The columns will be defined by the first row content.
      state.tableState.state = TableParseState.DefiningColumns;
    }
  }

  /** Parse a boundary that ends the column header for a table. */
  private static void parseTableColumnHeaderBoundary(
      SPLDOCElementNode current_node, ParseState state) {
    // Ensure columns are defined, and allow only one column header.
    if (state.tableState.state != TableParseState.FetchingRows
        || state.tableState.hasColumnHeader) {
      tableSyntaxError(current_node, state);
      return;
    }
    // Set prior rows as column header.
    state.tableState.rows.isColumnHeader = true;
    // Close all the entries in the current row.
    resetColumn(state);
    while (nextColumn(state)) {
      endColEntry(state);
    }
    // Start a new set of rows for the table body.
    state.tableState.rows = (new ParserSPLDOC()).new SPLDOCTableRowsNode(state);
    state.tableState.group.addChild(state.tableState.rows);
    addRow(state);
    resetColumn(state);
    nextColumn(state);
    addColEntry(state);
    // Parse the input until terminated by the end of the header boundary.
    int colStart = state.pos;
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    state.tableState.rowState = (new ParserSPLDOC()).new TableRowState();
    parseColBoundary(current_node, state, false);
    if (state.tableState.rowState.rowError) {
      return;
    }
    while (state.pos < end) {
      if (state.pos == end - 1) {
        // End of header boundary.
        parseColBoundary(current_node, state, false);
        if (state.tableState.rowState.rowError) {
          break;
        }
        // Skip trailing whitespace.
        skipLine(state);
        break;
      } else if (state.doc.charAt(state.pos) == '=') {
        // Column header markup.
        state.pos++;
      } else if (state.doc.charAt(state.pos) == ':') {
        // Column alignment specifier.
        if (!setColAlign(state)) {
          // Invalid alignment specification.
          tableSyntaxError(current_node, state);
          break;
        }
        state.pos++;
      } else if (state.doc.charAt(state.pos) == '+') {
        // Row header column boundary.
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          break;
        }
        // Move to the next column and start a new entry for the row.
        if (!nextColumn(state)) {
          // Too many columns specified.
          tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
          break;
        }
        colStart = state.pos;
        addColEntry(state);
        parseColBoundary(current_node, state, false);
        if (state.tableState.rowState.rowError) {
          break;
        }
      } else if (state.doc.charAt(state.pos) == '|') {
        // Normal column boundary.
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          break;
        }
        if (state.doc.charAt(state.pos + 1) == '|') {
          // The current entry spans the next column.
          if (!nextColumnWithSpan(state)) {
            // Too many columns specified.
            tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
            break;
          }
        } else {
          // Move to the next column and start a new entry for the row.
          if (!nextColumn(state)) {
            // Too many columns specified.
            tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
            break;
          }
          colStart = state.pos;
          addColEntry(state);
        }
        parseColBoundary(current_node, state, false);
        if (state.tableState.rowState.rowError) {
          break;
        }
      } else {
        // Invalid character.
        tableSyntaxError(current_node, state);
        break;
      }
    }
    // Process any remaining columns not yet processed.
    while (nextColumn(state)) {
      // Start a new entry for the row.
      addColEntry(state);
    }
    state.tableState.hasColumnHeader = true;
  }

  /** Parse the lower border that ends a table. */
  private static void parseTableEnd(SPLDOCElementNode current_node, ParseState state) {
    // Parse for correct characters.
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    while (state.pos < end) {
      if (state.doc.charAt(state.pos) != '-') {
        // Invalid character.
        tableSyntaxError(current_node, state);
        break;
      }
      state.pos++;
      if (state.pos == end) {
        // Skip trailing whitespace.
        skipLine(state);
        break;
      }
    }
    // Close all the entries in the current row.
    resetColumn(state);
    while (nextColumn(state)) {
      endColEntry(state);
    }
    // Set the default alignments for all table entries.
    setDefaultAlignments(state);
    state.tableState.state = TableParseState.End;
  }

  /** Parse the column boundary in a table that starts the current column. */
  private static void parseColBoundary(
      SPLDOCElementNode current_node, ParseState state, boolean beforeTableEntry) {
    if (state.doc.charAt(state.pos) == '+') {
      // Column boundary that ends a row header.
      if (state.tableState.colNum != 1) {
        // Row header can be only the first column.
        tableRowSyntaxError(current_node, state);
        return;
      }
      if (state.tableState.state == TableParseState.DefiningColumns) {
        state.tableState.tableNode.hasFirstColumnRowHeader = true;
      } else if (!state.tableState.tableNode.hasFirstColumnRowHeader) {
        // Mixture of row header and normal column boundary.
        tableRowError(SPLDOCErrorType.TableMixedColumnBoundaryError, current_node, state);
        return;
      }
    } else if (state.doc.charAt(state.pos) == '|') {
      // Normal column boundary.
      if (state.tableState.colNum == 1 && state.tableState.tableNode.hasFirstColumnRowHeader) {
        // Mixture of row header and normal column boundary.
        tableRowError(SPLDOCErrorType.TableMixedColumnBoundaryError, current_node, state);
        return;
      }
    } else if (beforeTableEntry && state.doc.charAt(state.pos) == ':') {
      // Row alignment specifier.
      if (!setRowAlign(state)) {
        // Invalid alignment specification.
        tableRowSyntaxError(current_node, state);
        return;
      }
    } else {
      // Invalid column boundary character.
      tableRowSyntaxError(current_node, state);
      return;
    }
    state.pos++;
  }

  /**
   * Parse a column entry in a table.
   *
   * <p>This method adds the next line of text for the entry.
   */
  private static void parseTableEntry(SPLDOCElementNode current_node, ParseState state) {
    // Move to the column entry.
    if (state.tableState.state == TableParseState.DefiningColumns) {
      // The columns are being defined by the first line of row content.
      if (state.tableState.colNum == -1) {
        // The first column has already been created along with the table.
        nextColumn(state);
      } else {
        // Add a new column for the entry.
        addColumn(state);
      }
    } else {
      // The columns have already been defined.
      if (!nextColEntry(state)) {
        // Too many columns specified.
        tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
        return;
      }
    }
    // Parse the opening column boundary for the entry.
    parseColBoundary(current_node, state, true);
    if (state.tableState.rowState.rowError) {
      return;
    }
    // Skip the whitespace character following the column boundary.
    state.pos++;
    // Parse the input text until terminated by a column boundary.
    if (state.tableState.colState.doc == null) {
      state.tableState.colState.doc = new StringBuilder();
      state.tableState.colState.lineOffset = state.lineOffset;
    } else if (state.tableState.colState.doc.length() > 0) {
      // Add new line character between each line of input.
      state.tableState.colState.doc.append("\n");
    }
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    while (state.pos < end) {
      state.tableState.colState.doc.append(parseEscapes(state));
      if (state.pos >= end) {
        break;
      } else if (TableEntryEnd.equals(state.doc, state.pos)) {
        // End of column entry text, skip trailing whitespace.
        state.pos = getNextNonWhitespace(state.doc, state.pos);
        return;
      } else {
        // Add next character to entry.
        state.tableState.colState.doc.append(state.doc.charAt(state.pos));
      }
      state.pos++;
    }
    // Invalid end of entry.
    tableRowSyntaxError(current_node, state);
  }

  /**
   * Parse a row boundary in a table.
   *
   * <p>The row boundary may: - start a new row for all, or only some, of the columns. - include
   * characters to specify columns and their alignments in the next row.
   */
  private static void parseRowBoundary(SPLDOCElementNode current_node, ParseState state) {
    if (state.tableState.state != TableParseState.FetchingRows) {
      // Cannot define a new row before columns are defined.
      tableRowError(SPLDOCErrorType.TableNoColsRowBoundaryError, current_node, state);
      return;
    }
    // Move to the next entry.
    if (!nextColEntry(state)) {
      // Too many columns specified.
      tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
      return;
    }
    // Start a new row, if not already started.
    if (!state.tableState.rowState.newRow) {
      addRow(state);
      state.tableState.rowState.newRow = true;
    }
    int colStart = state.pos;
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    // Close the current entry and start a new one for the new row.
    endColEntry(state);
    addColEntry(state);
    // Parse the opening column boundary.
    parseColBoundary(current_node, state, false);
    if (state.tableState.rowState.rowError) {
      return;
    }
    // Parse the input until terminated by the end of the row boundary.
    while (state.pos < end) {
      if (TableRowContentEnd.equals(state.doc, state.pos)) {
        // End of row boundary due to the end of the row content.
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableRowError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          return;
        }
        // Process any remaining columns not yet processed.
        while (nextColumn(state)) {
          // Close any current entry and start a new entry for the new row.
          endColEntry(state);
          addColEntry(state);
        }
        return;
      } else if (TableEntryStart.equals(state.doc, state.pos)) {
        // End of row boundary, due to next column entry.
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableRowError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          return;
        }
        // Process any remaining columns that were spanned by the previous entry.
        while (!nextColumnHasEntry(state)) {
          // Move to the next column, and start a new entry for the new row.
          if (!nextColumn(state)) {
            // Too many columns specified.
            tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
            return;
          }
          addColEntry(state);
        }
        return;
      } else if (state.doc.charAt(state.pos) == '-') {
        // Row boundary markup.
        state.pos++;
      } else if (state.doc.charAt(state.pos) == ':') {
        // Column alignment specifier.
        if (!setColAlign(state)) {
          // Invalid alignment specification.
          tableRowSyntaxError(current_node, state);
          return;
        }
        state.pos++;
      } else if (state.doc.charAt(state.pos) == '+') {
        // Row header column boundary (not directly followed by an entry).
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableRowError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          return;
        }
        // Move to the next column, close the current entry and start a new one for the row.
        if (!nextColumn(state)) {
          // Too many columns specified.
          tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
          return;
        }
        colStart = state.pos;
        endColEntry(state);
        addColEntry(state);
        parseColBoundary(current_node, state, false);
        if (state.tableState.rowState.rowError) {
          return;
        }
      } else if (state.doc.charAt(state.pos) == '|') {
        // Normal column boundary (not directly followed by an entry).
        if (state.pos < colStart + 4) {
          // Row boundary must be at least 3 characters.
          tableRowError(SPLDOCErrorType.TableShortRowBoundaryError, current_node, state);
          return;
        }
        if (state.doc.charAt(state.pos + 1) == '|') {
          // The current entry spans the next column.
          if (!nextColumnWithSpan(state)) {
            // Too many columns specified.
            tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
            return;
          }
          // Close any previous entry in that column.
          endColEntry(state);
        } else {
          // Move to the next column, close any current entry and start a new one for the row.
          if (!nextColumn(state)) {
            // Too many columns specified.
            tableRowError(SPLDOCErrorType.TableTooManyColumnsError, current_node, state);
            return;
          }
          colStart = state.pos;
          endColEntry(state);
          addColEntry(state);
        }
        parseColBoundary(current_node, state, false);
        if (state.tableState.rowState.rowError) {
          return;
        }
      } else {
        // Invalid character.
        tableRowSyntaxError(current_node, state);
        return;
      }
    }
    // Syntax error.
    tableRowSyntaxError(current_node, state);
  }

  /** Parse a line containing the row content for a table. */
  private static void parseTableRowContent(SPLDOCElementNode current_node, ParseState state) {
    if (state.tableState.state != TableParseState.DefiningColumns
        && state.tableState.state != TableParseState.FetchingRows) {
      // Row content specified before a table has been started.
      tableSyntaxError(current_node, state);
      return;
    }
    // Parse row content.
    state.tableState.rowState = (new ParserSPLDOC()).new TableRowState();
    resetColumn(state);
    int end = getEndOfLineWhitespace(state.doc, state.pos);
    while (state.pos < state.length) {
      if (state.pos >= end) {
        // Invalid end of row content.
        tableRowSyntaxError(current_node, state);
      } else if (TableRowContentEnd.equals(state.doc, state.pos)) {
        // End of row content.
        if (nextColEntry(state)) {
          // Too few columns specified.
          state.pos = end; // Report whole line.
          tableRowError(SPLDOCErrorType.TableTooFewColumnsError, current_node, state);
        } else {
          // Skip trailing whitespace.
          skipLine(state);
          // Extend any column entries that span a new row.
          addRowSpans(state);
          break;
        }
      } else if (TableEntryStart.equals(state.doc, state.pos)) {
        // A column entry.
        parseTableEntry(current_node, state);
      } else if (TableRowBoundary.equals(state.doc, state.pos)) {
        // A boundary that starts a new row, or syntax error.
        parseRowBoundary(current_node, state);
      } else {
        // Syntax error.
        tableRowSyntaxError(current_node, state);
      }
      if (state.tableState.rowState.rowError) {
        // Abort processing the row content on error.
        break;
      }
    }
    // Columns may be defined in the table border or the first line of row content.
    if (state.tableState.state == TableParseState.DefiningColumns) {
      state.tableState.state = TableParseState.FetchingRows;
    }
  }

  /**
   * Parse the input for a table and add the table to the given node.
   *
   * <p>The input is processed, and errors reported, until the end of the table is reached.
   */
  private static void parseTable(SPLDOCElementNode current_node, ParseState state, int baseIndent) {
    state.tableState = (new ParserSPLDOC()).new TableState();
    state.tableState.tableNode = (new ParserSPLDOC()).new SPLDOCTableNode(state);
    String doc_line = "";
    while (state.pos < state.length) {
      skipComments(state, baseIndent);
      if (state.pos >= state.length) {
        // Unexpected end of input.
        doc_line = "";
        break;
      }
      doc_line = getLine(state.pos, state);
      int indent = getIndent(doc_line);
      if (indent > baseIndent && CodeBlock.equals(doc_line.substring(baseIndent))) {
        // The table is terminated by the start of a codeblock.
        break;
      }
      state.pos += indent;
      if (TableTitle.equals(doc_line)) {
        // The table has on optional title.
        parseTableTitle(current_node, state);
      } else if (TableStart.equals(doc_line)
          && (state.tableState.state == TableParseState.Start
              || state.tableState.state == TableParseState.TitleFetched)) {
        // The markup to start a table also matches table end or row content markup.
        // Start the table.
        parseTableStart(current_node, state);
      } else if (TableColumnHeaderBoundary.equals(doc_line)) {
        // Process a column header boundary.
        parseTableColumnHeaderBoundary(current_node, state);
      } else if (TableRowContentStart.equals(doc_line)) {
        // Process the next line of row content.
        parseTableRowContent(current_node, state);
      } else if (TableEnd.equals(doc_line)) {
        // Finish processing the table.
        parseTableEnd(current_node, state);
        state.tableState = null;
        return;
      } else {
        // The table has been terminated by other markup or plain text.
        break;
      }
    }
    // The table has been terminated unexpectedly.
    // Report the error, and add an error string into the output.
    current_node.newChild(SPLDOCErrorType.TableEndError, doc_line.trim(), state);
    current_node.newChild(SPLDOCElementType.Text, errorText, state);
    state.tableState = null;
  }

  /**
   * Return the end position of the font markup that starts at the current position, or -1 if the
   * font markup is not valid.
   *
   * <p>Note that the font markup must be adjacent to non-whitespace characters in the marked up
   * text, and cannot span paragraphs or new-line markup.
   */
  private static int endOfFontMarkup(SPLDOCMarkup markup, ParseState state, int baseIndent) {
    // Save original position.
    int startPos = state.pos;
    int startLineOffset = state.lineOffset;
    // Advance position past the opening markup.
    state.pos += markup.length;
    while (state.pos < state.length) {
      parseEscapes(state);
      if (state.pos >= state.length) {
        break;
      }
      if (state.doc.charAt(state.pos) == '\n') {
        if (state.pos + 1 < state.length && isEndOfCurrentText(state.pos + 1, state, baseIndent)) {
          // The markup is terminated by new-line markup, so is not valid.
          break;
        }
        // Continue to next line.
        state.pos++;
        state.lineOffset++;
        skipComments(state, baseIndent);
        continue;
      }
      if (state.doc.startsWith(markup.markup_close, state.pos)
          && !isWhitespace(state.doc.charAt(state.pos - 1))) {
        // Closing markup reached, and not preceded by whitespace, so is valid.
        int endPos = state.pos + markup.length;
        state.pos = startPos;
        state.lineOffset = startLineOffset;
        return endPos;
      }
      if (ReservedInline.equals(state.doc, state.pos)) {
        // Contains characters reserved for future markup, so is not valid.
        break;
      }
      SPLDOCMarkup nestedMarkup;
      if ((nestedMarkup = getFontMarkup(state)) != null) {
        // The markup contains nested font markup.
        int endNestedPos = endOfFontMarkup(nestedMarkup, state, baseIndent);
        if (endNestedPos < 0) {
          // The markup contains invalid nested markup, so is not valid.
          break;
        }
        state.pos = endNestedPos;
      } else {
        state.pos++;
      }
    }
    // The markup was not closed, so is not valid.
    state.pos = startPos;
    state.lineOffset = startLineOffset;
    return -1;
  }

  /**
   * Parse the input text, that is governed by the given starting indentation, for the given in-line
   * font markup until the end of the current text input. A text node containing the given text
   * block, and the resulting text, is added to the given node.
   */
  private static void parseFontMarkup(
      SPLDOCMarkup markup,
      SPLDOCElementNode node,
      String plain_text,
      ParseState state,
      int baseIndent) {
    // Find the end of the markup.
    int endPos = endOfFontMarkup(markup, state, baseIndent);
    // Advance position past the opening markup.
    state.pos += markup.length;
    if (endPos < 0) {
      // The markup is not valid, report the error and output the markup as plain text.
      node.newChild(
          SPLDOCErrorType.InvalidInlineMarkupError,
          getLine(state.pos - markup.length, state).trim(),
          state);
      addTextNode(node, plain_text + markup.markup_open, state);
      return;
    }
    // Add the preceding text and adjust the font for the marked up text.
    addTextNode(node, plain_text, state);
    SPLDOCElementNode font_node = node.newChild(markup.type, state);
    int endOfText = endPos - markup.length;
    state.fontLevel++;
    parseText(font_node, state, baseIndent, endOfText);
    state.fontLevel--;
    // Verify parsing finished at the end position.
    if (state.pos != endOfText) {
      font_node.newChild(
          SPLDOCErrorType.InternalError,
          "unexpected end of text within SPLDOC inline markup: ("
              + state.pos
              + ", "
              + endOfText
              + ", "
              + markup.markup_close
              + ")",
          state);
    }
    // Set position past the closing markup.
    state.pos = endPos;
  }

  /**
   * Return the end position of the link or image markup that starts at the current position, or -1
   * if the link markup is not valid.
   *
   * <p>Note that the link markup cannot span paragraphs or new-line markup.
   */
  private static int endOfLinkOrImageMarkup(SPLDOCMarkup markup, ParseState state, int baseIndent) {
    // Save original position.
    int startPos = state.pos;
    int startLineOffset = state.lineOffset;
    // Advance position past the opening markup.
    state.pos += markup.length;
    while (state.pos < state.length) {
      parseEscapes(state);
      if (state.pos >= state.length) {
        break;
      }
      if (state.doc.charAt(state.pos) == '\n') {
        if (state.pos + 1 < state.length && isEndOfCurrentText(state.pos + 1, state, baseIndent)) {
          // The markup is terminated by new-line markup, so is not valid.
          break;
        }
        // Continue to next line.
        state.pos++;
        state.lineOffset++;
        skipComments(state, baseIndent);
        continue;
      }
      if (state.doc.startsWith(markup.markup_close, state.pos)) {
        // Closing markup reached, so is valid.
        int endPos = state.pos + markup.length;
        state.pos = startPos;
        state.lineOffset = startLineOffset;
        return endPos;
      }
      state.pos++;
    }
    // The markup was not closed, so is not valid.
    state.pos = startPos;
    state.lineOffset = startLineOffset;
    return -1;
  }

  /**
   * Parse the input text, that is governed by the given starting indentation, for the given in-line
   * markup for a link or image, until the text is terminated by the end of the current text input.
   */
  private static void parseLinkOrImage(
      SPLDOCMarkup markup,
      SPLDOCElementNode node,
      String plain_text,
      ParseState state,
      int baseIndent) {
    // Find the end of the markup.
    int endPos = endOfLinkOrImageMarkup(markup, state, baseIndent);
    // Advance position past the opening markup.
    state.pos += markup.length;
    if (endPos < 0) {
      // The markup is not valid, report the error and output the markup as plain text.
      node.newChild(
          SPLDOCErrorType.InvalidInlineMarkupError,
          getLine(state.pos - markup.length, state).trim(),
          state);
      addTextNode(node, plain_text + markup.markup_open, state);
      return;
    }
    // Parse until the end of marked up text.
    StringBuilder marked_text = new StringBuilder();
    String url_text = "";
    boolean hasLabel = false;
    boolean isLinkMarkup = markup.equals(Link);
    SPLDOCMarkup labelMarkup = isLinkMarkup ? LinkLabel : ImageLabel;
    int endOfText = endPos - markup.length;
    while (state.pos < endOfText) {
      marked_text.append(parseEscapes(state));
      if (state.pos >= endOfText) {
        break;
      }
      boolean hasReserved = false;
      SPLDOCMarkup markupType = null;
      if (state.doc.charAt(state.pos) == '\n') {
        state.pos++;
        state.lineOffset++;
        if (state.pos < endOfText) {
          skipComments(state, baseIndent);
        }
        // Add a space between text on successive lines.
        marked_text.append(" ");
      } else if (!hasLabel && labelMarkup.equals(state.doc, state.pos)) {
        state.pos += labelMarkup.length;
        hasLabel = true;
        url_text = marked_text.toString();
        marked_text = new StringBuilder();
      } else if (hasLabel
          && isLinkMarkup
          && ((hasReserved = ReservedInline.equals(state.doc, state.pos))
              || (markupType = getFontMarkup(state)) != null)) {
        // Warn since link labels could contain font markup in the future.
        node.newChild(
            SPLDOCErrorType.ReservedNestedInlineError, getLine(state.pos, state).trim(), state);
        int end = (hasReserved ? ReservedInline.end : state.pos + markupType.length);
        while (state.pos < end) {
          marked_text.append(state.doc.charAt(state.pos));
          state.pos++;
        }
      } else {
        marked_text.append(state.doc.charAt(state.pos));
        state.pos++;
      }
    }
    if (!hasLabel) {
      url_text = marked_text.toString();
    }
    // Add the preceding text, and the link or image.
    addTextNode(node, plain_text, state);
    SPLDOCElementNode child_node = node.newChild(markup.type, state);
    child_node.newChild(
        (isLinkMarkup ? SPLDOCElementType.LinkURL : SPLDOCElementType.ImageURL),
        url_text.trim(),
        state);
    if (hasLabel) {
      String label = marked_text.toString().trim();
      if (!label.isEmpty()) {
        child_node.newChild(labelMarkup.type, label, state);
      }
    }
    // Verify parsing finished at the end position.
    if (state.pos != endOfText) {
      node.newChild(
          SPLDOCErrorType.InternalError,
          "unexpected end of text within SPLDOC inline markup: ("
              + state.pos
              + ", "
              + endOfText
              + ", "
              + markup.markup_close
              + ")",
          state);
    }
    // Set position past the closing markup.
    state.pos = endPos;
  }

  /**
   * Parse the input text, which may contain in-line markup, into the given node until the text is
   * terminated by the end of the current text input.
   */
  private static void parseText(SPLDOCElementNode current_node, ParseState state, int baseIndent) {
    parseText(current_node, state, baseIndent, state.length);
  }

  /**
   * Parse the input text, which may contain in-line markup, into the given node until the text is
   * terminated by the end of the current text input, or the given end position is reached.
   *
   * <p>The end position must not lie within any escaped text or comment lines.
   */
  private static void parseText(
      SPLDOCElementNode current_node, ParseState state, int baseIndent, int endPosition) {
    StringBuilder plain_text = new StringBuilder();
    while (state.pos < endPosition) {
      plain_text.append(parseEscapes(state));
      if (state.pos >= endPosition) {
        break;
      }
      SPLDOCMarkup markup;
      if (state.doc.charAt(state.pos) == '\n') {
        state.pos++;
        state.lineOffset++;
        if (state.pos < endPosition) {
          skipComments(state, baseIndent);
        }
        if (state.pos < endPosition && isEndOfCurrentText(state.pos, state, baseIndent)) {
          break;
        }
        // Add a space between text on successive lines.
        plain_text.append(" ");
      } else if (ReservedInline.equals(state.doc, state.pos)) {
        // Characters reserved for possible future markup are not allowed.
        // Report the error, and add an error string into the output.
        plain_text.append(errorText);
        String reserved = "";
        while (state.pos < ReservedInline.end) {
          reserved += state.doc.charAt(state.pos);
          plain_text.append(state.doc.charAt(state.pos));
          state.pos++;
        }
        current_node.newChild(SPLDOCErrorType.ReservedInlineError, reserved, state);
      } else if ((markup = getFontMarkup(state)) != null) {
        parseFontMarkup(markup, current_node, plain_text.toString(), state, baseIndent);
        plain_text = new StringBuilder();
      } else if (!state.inTitle && (markup = getLinkOrImageMarkup(state)) != null) {
        if (state.fontLevel <= 0) {
          parseLinkOrImage(markup, current_node, plain_text.toString(), state, baseIndent);
          plain_text = new StringBuilder();
        } else {
          // Link or image markup that is nested in font markup.
          if (markup.equals(Link)) {
            // Warn since font markup could contain links in the future.
            current_node.newChild(
                SPLDOCErrorType.ReservedNestedInlineError, getLine(state.pos, state).trim(), state);
          }
          plain_text.append(markup.markup_open);
          state.pos += markup.length;
        }
      } else {
        plain_text.append(state.doc.charAt(state.pos));
        state.pos++;
      }
    }
    addTextNode(current_node, plain_text.toString(), state);
  }

  /** Parse the next input text containing SPLDOC markup into the given node. */
  private static void parse(SPLDOCElementNode parent, ParseState state) {
    int baseIndent;
    int pos_prev = -1;
    SPLDOCElementNode current_node = null;
    SPLDOCElementNode listitem_node = null;
    // Skip any comments and blank lines at the start of this text.
    skipCommentsAndBlankLines(state, -1);
    if (parent.type == SPLDOCElementType.OList || parent.type == SPLDOCElementType.UList) {
      // A new list (where current_node will be set below).
      // Set the indentation level for determining code blocks, etc.
      baseIndent = adjustIndent(getIndent(getLine(state.pos, state)), state);
    } else {
      // Start a new paragraph for the new document, section, etc.
      current_node = parent.newChild(SPLDOCElementType.Paragraph, state);
      // Set the indentation level for determining code blocks, etc.
      baseIndent = state.startIndent;
    }
    while (state.pos < state.length) {
      // Get the next input line.
      String doc_line = getLine(state.pos, state);
      int indent = getIndent(doc_line);
      int adjustedIndent = adjustIndent(indent, state);
      // Check for new-line markup.
      if (Paragraph.equals(doc_line)) {
        // The start of a new paragraph.
        // Paragraphs are delimited by blank lines and can be empty.
        current_node =
            (state.listLevel > 0 ? listitem_node : parent)
                .newChild(SPLDOCElementType.Paragraph, state);
        pos_prev = state.pos;
        state.pos += doc_line.length();
        state.lineOffset++;
        continue;
      } else if (indent > baseIndent && CodeBlock.equals(doc_line.substring(baseIndent))) {
        // The start of a code block (determined by the indentation relative to the current level).
        parseCodeBlock(
            current_node.newChild(SPLDOCElementType.CodeBlock, state), state, baseIndent);
        continue;
      } else if (NewPage.equals(doc_line)) {
        // The start of a new page.
        if (state.listLevel > 0 || parent.type == SPLDOCElementType.Section) {
          // The current list or section has finished. Return to the previous parsing level.
          return;
        }
        // Determine the requested page nesting level.
        int level = 0;
        while (doc_line.charAt(indent + level) == '+') {
          level++;
        }
        if (level <= state.pageLevel) {
          // The current page has finished. Return to the previous parsing level.
          return;
        }
        // Create a new page.
        current_node = parent.newChild(SPLDOCElementType.NewPage, state);
        state.pos += indent + level;
        state.pageLevel++;
        parseTitle(current_node, state, baseIndent);
        current_node.text = getFirstSentence(state);
        parse(current_node, state);
        state.pageLevel--;
        continue;
      } else if (Section.equals(doc_line)) {
        // A section heading.
        if (state.listLevel > 0 || parent.type == SPLDOCElementType.Section) {
          // The current list or section has finished. Return to the previous parsing level.
          return;
        }
        // Create a new section.
        current_node = parent.newChild(SPLDOCElementType.Section, state);
        state.pos += indent + 1;
        parseTitle(current_node, state, baseIndent);
        parse(current_node, state);
        continue;
      } else if (OListItem.equals(doc_line)) {
        // An ordered list item.
        if (parent.type == SPLDOCElementType.OList && adjustedIndent == baseIndent) {
          // Add a new list item to the current list.
          listitem_node = parent.newChild(SPLDOCElementType.OListItem, state);
          if (current_node != null
              && current_node.type == SPLDOCElementType.Paragraph
              && current_node.isEmpty()) {
            // Start a new paragraph for the list item text.
            current_node = listitem_node.newChild(SPLDOCElementType.Paragraph, state);
          } else {
            current_node = listitem_node;
          }
          state.pos += doc_line.indexOf('.', indent) + 1;
        } else if (state.listLevel > 0 && adjustedIndent <= baseIndent) {
          // The current list has finished. Return to the previous parsing level.
          if (current_node.type == SPLDOCElementType.Paragraph && current_node.isEmpty()) {
            // Replace the last blank line so it can be used to start a new paragraph.
            state.pos = pos_prev;
            state.lineOffset--;
          }
          return;
        } else {
          // Create a new list. We will go to the code above to add a new list item.
          state.listLevel++;
          parse(current_node.newChild(SPLDOCElementType.OList, state), state);
          state.listLevel--;
          continue;
        }
      } else if (UListItem.equals(doc_line)) {
        // An unordered list item.
        if (parent.type == SPLDOCElementType.UList && adjustedIndent == baseIndent) {
          // Add a new list item to the current list.
          listitem_node = parent.newChild(SPLDOCElementType.UListItem, state);
          if (current_node != null
              && current_node.type == SPLDOCElementType.Paragraph
              && current_node.isEmpty()) {
            // Start a new paragraph for the list item text.
            current_node = listitem_node.newChild(SPLDOCElementType.Paragraph, state);
          } else {
            current_node = listitem_node;
          }
          state.pos += indent + 1;
        } else if (state.listLevel > 0 && adjustedIndent <= baseIndent) {
          // The current list has finished. Return to the previous parsing level.
          if (current_node.type == SPLDOCElementType.Paragraph && current_node.isEmpty()) {
            // Replace the last blank line so it can be used to start a new paragraph.
            state.pos = pos_prev;
            state.lineOffset--;
          }
          return;
        } else {
          // Create a new list. We will go to the code above to add a new list item.
          state.listLevel++;
          parse(current_node.newChild(SPLDOCElementType.UList, state), state);
          state.listLevel--;
          continue;
        }
      } else if (TableTitle.equals(doc_line) || TableStart.equals(doc_line)) {
        // The start of a table.
        parseTable(current_node, state, baseIndent);
        continue;
      } else if (Annotation.equals(doc_line)) {
        // A line that starts an annotation, and all following lines, are skipped.
        state.pos = state.length;
        return;
      } else if (Comment.equals(doc_line)) {
        // A line that starts a comment that will not be included in the output.
        skipComments(state, baseIndent);
        continue;
      } else if (ReservedNewline.equals(doc_line)) {
        // A line that starts with characters reserved for possible future markup.
        // Report the error, and add an error string into the output.
        current_node.newChild(SPLDOCErrorType.ReservedNewlineError, doc_line.trim(), state);
        current_node.newChild(SPLDOCElementType.Text, errorText + doc_line, state);
        state.pos += doc_line.length();
        state.lineOffset++;
        continue;
      } else if (state.listLevel > 0 && indent <= baseIndent) {
        // The current list has finished. Return to the previous parsing level.
        if (current_node.type == SPLDOCElementType.Paragraph && current_node.isEmpty()) {
          // Replace the last blank line so it can be used to start a new paragraph.
          state.pos = pos_prev;
          state.lineOffset--;
        }
        return;
      }
      // Parse the remaining text until terminated by new-line markup.
      parseText(current_node, state, baseIndent);
    }
  }

  // Parsing states when parsing for annotations.
  private enum AnnotationParseState {
    Start,
    LookingForNextAnnotation,
    LookingForTarget,
    LookingForDescription,
    LookingForEndOfDescription
  };

  /** Close any previous annotation and add it to the list of annotations. */
  private static void closePreviousAnnotation(
      AnnotationParseState annotationParseState,
      List<SPLDOCAnnotation> annotations,
      SPLDOCAnnotation annotation,
      StringBuilder description) {
    if (annotationParseState != AnnotationParseState.Start) {
      if (annotationParseState == AnnotationParseState.LookingForTarget) {
        // No required target.
        annotation.errorType = SPLDOCAnnotationErrorType.NoTargetError;
      }
      if (annotationParseState == AnnotationParseState.LookingForEndOfDescription) {
        annotation.description = description.toString();
      }
      annotations.add(annotation);
    }
  }

  /**
   * Parse the input SPLDOC markup text for annotations, and return the annotations.
   *
   * <p>Note that the annotation processing of SPL files and XML model files differs. The
   * annotations in SPL files are processed by the SPLDOC model builder, which interprets any line
   * starting with "@" as an annotation, (therefore a line in such a code block cannot start with an
   * "@" character). The annotations in XML model files are processed by the method below, which
   * interprets any line starting with "@" as an annotation only if there is no indentation that
   * might allow it to be part of a code block (therefore a line in a code block in an XML file can
   * start with an "@" character).
   */
  private static List<SPLDOCAnnotation> parseAnnotations(ParseState state) {
    List<SPLDOCAnnotation> annotations = new ArrayList<SPLDOCAnnotation>();
    AnnotationParseState annotationParseState = AnnotationParseState.Start;
    SPLDOCAnnotation annotation = null;
    StringBuilder description = null;
    boolean nextLine = true;
    int start, end;
    while (state.pos < state.length) {
      // Get the remainder of the current input line.
      String doc_line = getLine(state.pos, state);
      // Check for the start of an annotation.
      if (nextLine && Annotation.equals(doc_line) && !CodeBlock.equals(doc_line)) {
        // Close any previous annotation.
        closePreviousAnnotation(annotationParseState, annotations, annotation, description);
        // Open a new annotation.
        annotation = (new ParserSPLDOC()).new SPLDOCAnnotation(state.lineOffset);
        start = Annotation.end;
        end = getNextWhitespaceOrEnd(doc_line, start);
        if (end <= start) {
          // No tag name.
          annotation.errorType = SPLDOCAnnotationErrorType.NoTagNameError;
          annotationParseState = AnnotationParseState.LookingForNextAnnotation;
          nextLine = true;
        } else {
          annotation.name = doc_line.substring(start, end);
          if (AnnotationTagsWithTargets.contains(annotation.name)) {
            annotationParseState = AnnotationParseState.LookingForTarget;
          } else {
            annotationParseState = AnnotationParseState.LookingForDescription;
          }
          state.pos += end;
          nextLine = false;
        }
      } else if (annotationParseState == AnnotationParseState.LookingForTarget) {
        if ((start = getNextNonWhitespace(doc_line, 0)) >= 0) {
          end = getNextWhitespaceOrEnd(doc_line, start);
          annotation.target = doc_line.substring(start, end);
          annotationParseState = AnnotationParseState.LookingForDescription;
          state.pos += end;
          nextLine = false;
        } else {
          nextLine = true;
        }
      } else if (annotationParseState == AnnotationParseState.LookingForDescription) {
        if ((start = getNextNonWhitespace(doc_line, 0)) >= 0) {
          if (nextLine && start > state.startIndent) {
            // Strip off leading whitespace characters up to the starting indentation.
            start = state.startIndent;
          }
          description = new StringBuilder(doc_line.substring(start));
          annotationParseState = AnnotationParseState.LookingForEndOfDescription;
        }
        nextLine = true;
      } else if (annotationParseState == AnnotationParseState.LookingForEndOfDescription) {
        start = getNextNonWhitespace(doc_line, 0);
        if (start < 0) {
          // A blank line.
          start = 0;
        } else if (start > state.startIndent) {
          // Strip off leading whitespace characters up to the starting indentation.
          start = state.startIndent;
        }
        description.append(doc_line.substring(start));
        nextLine = true;
      } else {
        // Looking for first, or next, annotation.
        nextLine = true;
      }
      if (nextLine) {
        // Move to the next line.
        state.pos += doc_line.length();
        state.lineOffset++;
      }
    }
    // Close any previous annotation.
    closePreviousAnnotation(annotationParseState, annotations, annotation, description);
    return annotations;
  }

  /**
   * Return the first sentence of markup text, if any, starting at the current position.
   *
   * <p>The first sentence is defined as a string terminated by a period that is not preceded by
   * whitespace, and is followed by whitespace, or by markup that terminates the current text. Any
   * leading title is included as part of the sentence.
   */
  private static String getFirstSentence(ParseState state) {
    // Save original position.
    int startPos = state.pos;
    int startLineOffset = state.lineOffset;
    // If the markup begins with a title, include the title.
    int baseIndent = state.startIndent;
    String doc_line = getLine(state.pos, state);
    if (NewPage.equals(doc_line) || Section.equals(doc_line)) {
      state.pos += doc_line.length();
      skipCommentsAndBlankLines(state, baseIndent);
    }
    // Find the end of the first sentence.
    int endPos = state.length;
    while (state.pos < state.length) {
      skipComments(state, -1);
      if (state.pos >= state.length) {
        break;
      }
      doc_line = getLine(state.pos, state);
      int indent = getIndent(doc_line);
      if (Paragraph.equals(doc_line)
          || (indent > baseIndent && CodeBlock.equals(doc_line.substring(baseIndent)))
          || NewPage.equals(doc_line)
          || Section.equals(doc_line)
          || OListItem.equals(doc_line)
          || UListItem.equals(doc_line)
          || TableTitle.equals(doc_line)
          || TableStart.equals(doc_line)
          || Annotation.equals(doc_line)
          || ReservedNewline.equals(doc_line)) {
        // The above types of markup end the sentence.
        endPos = state.pos;
        break;
      }
      int ix = doc_line.indexOf('.');
      if (ix > 0
          && !isWhitespace(doc_line.charAt(ix - 1))
          && (ix + 1 == doc_line.length() || isWhitespace(doc_line.charAt(ix + 1)))) {
        // End of sentence found.
        endPos = state.pos + ix + 1;
        break;
      }
      state.pos += doc_line.length();
      state.lineOffset++;
    }
    state.pos = startPos;
    state.lineOffset = startLineOffset;
    return state.doc.substring(startPos, endPos);
  }

  /** Given SPLDOC markup text, return the first sentence of markup text. */
  public static String getFirstSentence(String doc_input) {
    if (doc_input == null) {
      return "";
    }
    ParseState state = (new ParserSPLDOC()).new ParseState(doc_input);
    return getFirstSentence(state);
  }

  /**
   * Returns true if the given input text containing SPLDOC markup contains a description, otherwise
   * returns false. Any SPLDOC annotations are ignored, and should be processed separately.
   */
  public static boolean hasDescription(String doc_input) {
    if (doc_input == null) {
      return false;
    }
    ParseState state = (new ParserSPLDOC()).new ParseState(doc_input);
    skipCommentsAndBlankLines(state, -1);
    if (getNextNonWhitespace(state.doc, state.pos) >= 0
        && !Annotation.equals(state.doc, state.pos)) {
      return true;
    }
    return false;
  }

  /**
   * Parse the given input text containing SPLDOC markup. Any SPLDOC annotations are ignored, and
   * should be processed separately. Returns the root node of the resulting SPLDOC syntax tree.
   */
  public static SPLDOCElementNode parse(String doc_input) {
    if (doc_input == null) {
      doc_input = "";
    }
    ParseState state = (new ParserSPLDOC()).new ParseState(doc_input);
    root = (new ParserSPLDOC()).new SPLDOCElementNode(SPLDOCElementType.Root, state);
    parse(root, state);
    // root.print();
    return root;
  }

  /**
   * Parse the given input text containing SPLDOC markup, and return any SPLDOC annotations, or an
   * empty list.
   */
  public static List<SPLDOCAnnotation> parseAnnotations(String doc_input) {
    if (doc_input == null) {
      doc_input = "";
    }
    ParseState state = (new ParserSPLDOC()).new ParseState(doc_input);
    return parseAnnotations(state);
  }

  /*
   * Test function.
   */
  static String TestSPLDOC_1 = "First paragraph with *italicized* text.\n";

  public static void main(String[] argv) {
    System.out.println("\n");
    SPLDOCElementNode root;
    System.out.println("");
    System.out.println("Test paragraph 30: \n" + TestSPLDOC_1);
    root = parse(TestSPLDOC_1);
    root.print();
  }
}
