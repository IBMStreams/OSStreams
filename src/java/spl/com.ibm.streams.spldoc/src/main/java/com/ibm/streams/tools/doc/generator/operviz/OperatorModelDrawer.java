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

package com.ibm.streams.tools.doc.generator.operviz;

import com.ibm.streams.tools.doc.messages.Messages;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.rendering.RenderingDescriptor;
import com.ibm.streams.tools.doc.utilities.rendering.SVGTooltipManager;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.GradientPaint;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.geom.RoundRectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.imageio.ImageIO;
import org.apache.batik.svggen.SVGGraphics2D;

public class OperatorModelDrawer {
  private SVGGraphics2D graphics_ = null;
  private OperatorModelProperties props_ = null;
  private Font opKindLabelFont_ = null;
  private Font opLangLabelFont_ = null;
  private Font opThreadingLabelFont_ = null;
  private Font portCardinalityLabelFont_ = null;
  private Font portMutabilityLabelFont_ = null;
  private Font windowPunctuationLabelFont_ = null;
  private Rectangle2D.Float operBounds_ = null;
  private int numInputPortSets_ = 0, numOutputPortSets_ = 0;
  private int imageOriginX_, imageOriginY_, imageWidth_, imageHeight_;

  public OperatorModelDrawer(OperatorModelProperties props) {
    props_ = props;
  }

  public ArrayList<Integer> paint(SVGGraphics2D graphics) {
    graphics_ = graphics;
    ArrayList<Integer> imageDimensions = setupSizes();
    paintOperatorBody();
    paintInputPorts();
    paintOutputPorts();
    finalizeTooltips();
    return imageDimensions;
  }

  private ArrayList<Integer> setupSizes() {
    float portWidth = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE;
    float operWidth = RenderingDescriptor.PRIMITIVE_OPERATOR_BODY_DEFAULT_WIDTH;
    float operHeight = RenderingDescriptor.PRIMITIVE_OPERATOR_BODY_DEFAULT_HEIGHT;
    float tooltipWidth = RenderingDescriptor.PRIMITIVE_OPERATOR_TOOLTIP_DEFAULT_WIDTH;
    numInputPortSets_ = props_.getInputPortSets().size();
    numOutputPortSets_ = props_.getOutputPortSets().size();
    float operHeightNeeded =
        Math.max(numInputPortSets_, numOutputPortSets_)
            * (RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE
                + RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SPACING);
    operHeight = Math.max(operHeight, operHeightNeeded);

    float margin = RenderingDescriptor.PRIMITIVE_OPERATOR_MARGIN;
    imageOriginX_ = 0;
    imageOriginY_ = 0;
    imageWidth_ =
        2 * Math.round(margin)
            + 2 * Math.round(portWidth)
            + Math.round(operWidth)
            + Math.round(tooltipWidth);
    imageHeight_ = 2 * Math.round(margin) + Math.round(operHeight);
    operBounds_ = new Rectangle2D.Float(margin + portWidth, margin, operWidth, operHeight);

    // RTC 27182: Going from RHEL6 to RHEL7 the characters become compressed.
    // Changing from SERIF to SANS_SERIF means much less compression and better legibility.
    float size = operBounds_.width / 10;
    opKindLabelFont_ = new Font(Font.SANS_SERIF, Font.BOLD, (int) size);
    opLangLabelFont_ = new Font("Courier", Font.PLAIN, (int) size);
    opThreadingLabelFont_ = new Font(Font.SANS_SERIF, Font.BOLD, (int) (size * 0.8));
    portCardinalityLabelFont_ = new Font(Font.SANS_SERIF, Font.PLAIN, (int) size);
    portMutabilityLabelFont_ = opThreadingLabelFont_;
    windowPunctuationLabelFont_ = opKindLabelFont_;

    ArrayList<Integer> imageDimensions = new ArrayList<Integer>(2);
    imageDimensions.add(0, new Integer(imageWidth_));
    imageDimensions.add(1, new Integer(imageHeight_ + opLangLabelFont_.getSize()));

    Rectangle2D.Float frameRectangle =
        new Rectangle2D.Float(imageOriginX_, imageOriginY_, imageWidth_, imageHeight_);

    graphics_.setColor(
        RenderingDescriptor.TRANSPARENT_BLACK_BACKGROUND_COLOR); // for frame boundary definition
    graphics_.fill(frameRectangle);

    return imageDimensions;
  }

  private void paintOperatorBody() {
    RoundRectangle2D.Float[] rectangles =
        RenderingDescriptor.getRoundedRectanglesWithBorder(
            operBounds_.x,
            operBounds_.y,
            operBounds_.width,
            operBounds_.height,
            RenderingDescriptor.OPERATOR_NODE_BODY_BORDER_WIDTH,
            1);

    // Border
    graphics_.setColor(RenderingDescriptor.OPERATOR_NODE_BODY_BORDER_COLOR);
    graphics_.fill(rectangles[0]);

    // Box
    // Use same gradient as composite operator graph for this detailed operator graphic.
    // The gradient spans 90% of the rectangle's height
    float y90 = (float) (operBounds_.y + (operBounds_.height * 0.9));
    GradientPaint gradientPaint =
        new GradientPaint(
            operBounds_.x,
            operBounds_.y,
            RenderingDescriptor.OPERATOR_NODE_BODY_FILL_COLOR,
            operBounds_.x,
            y90,
            RenderingDescriptor.OPERATOR_NODE_BODY_FILL_COLOR_1);
    graphics_.setPaint(gradientPaint);
    // TODO: Remove disabled code.
    // graphics_.setColor(RenderingDescriptor.OPERATOR_NODE_BODY_FILL_COLOR);
    graphics_.fill(rectangles[1]);

    graphics_.setColor(Color.BLACK);
    graphics_.setFont(opKindLabelFont_);
    RoundRectangle2D.Float innerRectangle = rectangles[1];
    Rectangle2D.Float textBounds =
        new Rectangle2D.Float(
            innerRectangle.x + innerRectangle.width * 0.1f,
            innerRectangle.y,
            innerRectangle.width * 0.8f,
            opKindLabelFont_.getSize());
    drawTextWithinBounds(props_.getOperatorKind(), textBounds, false);
    String iconPath = props_.getIconPath();
    if (iconPath != null) {
      BufferedImage icon = null;
      try {
        icon = ImageIO.read(new File(iconPath));
      } catch (IOException e) {
      }
      if (icon != null) {
        graphics_.drawImage(
            icon,
            (int) (operBounds_.getCenterX() - icon.getWidth() / 2.0),
            (int) (operBounds_.getCenterY() - icon.getHeight() / 2.0),
            null);
      }
    }
    graphics_.setFont(opLangLabelFont_);
    OperatorModelProperties.OperatorLanguage lang = props_.getOperatorLanguage();
    textBounds =
        new Rectangle2D.Float(
            innerRectangle.x + innerRectangle.width * 0.1f,
            innerRectangle.y + innerRectangle.height - opLangLabelFont_.getSize(),
            innerRectangle.width * 0.8f,
            opLangLabelFont_.getSize());
    drawTextWithinBounds(
        lang == OperatorModelProperties.OperatorLanguage.CPP ? "C++" : lang.toString(),
        textBounds,
        false);
    graphics_.setFont(opThreadingLabelFont_);
    String threadLabel = "";
    String explanation = "";
    OperatorModelProperties.SingleThreadedContext threadContext = props_.getThreadContext();
    Logger logger = Logger.getInstance();
    switch (threadContext) {
      case Never:
        threadLabel = "mt";
        explanation = logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_NEVER);
        break;
      case Always:
        threadLabel = "st";
        explanation = logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_ALWAYS);
        break;
      case WindowDependent:
        threadLabel = "wd";
        explanation = logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_WINDOW);
        break;
      default:
        throw new AssertionError(threadContext);
    }
    textBounds =
        new Rectangle2D.Float(
            innerRectangle.x + innerRectangle.width * 0.8f,
            innerRectangle.y + innerRectangle.height - opLangLabelFont_.getSize(),
            innerRectangle.width * 0.15f,
            opThreadingLabelFont_.getSize());
    drawTextWithinBounds(threadLabel, textBounds, false);
    Point2D.Float tooltipLoc =
        new Point2D.Float(
            (float) textBounds.getCenterX(),
            (float) textBounds.getMinY() + 2 * graphics_.getFontMetrics().getHeight() + 2);
    SVGTooltipManager.getInstance()
        .addTooltip((SVGGraphics2D) graphics_, textBounds, explanation, tooltipLoc);

    if (props_.getMandatoryParameters().size() + props_.getOptionalParameters().size() > 0) {
      textBounds =
          new Rectangle2D.Float(
              innerRectangle.x + innerRectangle.width * 0.1f,
              innerRectangle.y + innerRectangle.height - opLangLabelFont_.getSize(),
              innerRectangle.width * 0.15f,
              opLangLabelFont_.getSize());
      drawTextWithinBounds("p", textBounds, false);
      // Create tooltip that lists operator's parameters. To limit width
      // of toolkit text, break list into multiple liens.
      StringBuilder builder = new StringBuilder();
      int cur_line_length = 0;
      if (props_.getMandatoryParameters().size() > 0) {
        builder.append(logger.getMessage(Messages.MSG_REQUIRED_PARAMETER)).append(" ");
        cur_line_length = builder.length();
        boolean first = true;
        for (String param : props_.getMandatoryParameters()) {
          if (first) first = false;
          else builder.append(", ");
          // RTC 18602 - create a new line if the list of parameters exceeds this width
          if (cur_line_length > RenderingDescriptor.PRIMITIVE_OPERATOR_TOOLTIP_LINE_WIDTH) {
            builder.append("\n");
            cur_line_length = 0;
          }
          builder.append(param);
          cur_line_length += param.length();
        }

        if (props_.getOptionalParameters().size() > 0) {
          builder.append(" / ");
          cur_line_length += " / ".length();
        }
      }
      if (props_.getOptionalParameters().size() > 0) {
        // RTC 18602 - create a new line if the list of parameters exceeds this width
        if (cur_line_length > RenderingDescriptor.PRIMITIVE_OPERATOR_TOOLTIP_LINE_WIDTH) {
          builder.append("\n");
          cur_line_length = 0;
        }
        String optional_label = logger.getMessage(Messages.MSG_OPTIONAL_PARAMETER);
        builder.append(optional_label + " ");
        cur_line_length += optional_label.length() + 1;
        boolean first = true;

        for (String param : props_.getOptionalParameters()) {
          if (first) first = false;
          else builder.append(", ");
          // RTC 18602 - create a new line if the list of parameters exceeds this width
          if (cur_line_length > RenderingDescriptor.PRIMITIVE_OPERATOR_TOOLTIP_LINE_WIDTH) {
            builder.append("\n");
            cur_line_length = 0;
          }
          builder.append(param);
          cur_line_length += param.length();
        }
      }
      tooltipLoc =
          new Point2D.Float(
              (float) textBounds.getCenterX(),
              textBounds.y + 2 * graphics_.getFontMetrics().getHeight() + 2);
      SVGTooltipManager.getInstance()
          .addTooltip((SVGGraphics2D) graphics_, textBounds, builder.toString(), tooltipLoc);
    }
  }

  private void paintInputPorts() {
    int i = 0;
    for (OperatorModelProperties.InputPortSet iport : props_.getInputPortSets()) {
      paintInputPort(iport, i);
      i++;
    }
  }

  private void paintInputPort(OperatorModelProperties.InputPortSet iport, int i) {
    float portWidth = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE;
    float portHeight = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE;
    float portSpacing = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SPACING;
    float portYstart =
        (operBounds_.height
                - numInputPortSets_ * portHeight
                - (numInputPortSets_ - 1) * portSpacing)
            / 2;
    Rectangle2D.Float bounds =
        new Rectangle2D.Float(
            operBounds_.x - portWidth,
            operBounds_.y + portYstart + i * (portHeight + portSpacing),
            portWidth,
            portHeight);
    boolean optional = iport.isOptional();
    int cardinality = iport.getCardinality();
    boolean mutable = iport.isMutable();
    paintPortBox(bounds, optional, cardinality, mutable);
    // punctuation
    int pwidth = graphics_.getFontMetrics().charWidth('!');
    Rectangle2D.Float punctBounds =
        new Rectangle2D.Float(bounds.x - 2 * pwidth, bounds.y, 2 * pwidth, bounds.height);
    OperatorModelProperties.InputPortSet.PunctuationMode pmode = iport.getPunctuationMode();
    Logger logger = Logger.getInstance();
    switch (pmode) {
      case Expecting:
        paintPunctuation(
            true, /*"This port is expecting punctuations"*/
            logger.getMessage(Messages.MSG_PORT_EXPECTING_PUNCTUATIONS),
            punctBounds);
        break;
      case Oblivious:
        break;
      case WindowBound:
        paintPunctuation(
            false, /*"This port may expect punctuations if a punctuation-based window is present"*/
            logger.getMessage(Messages.MSG_PORT_WINDOW_BOUND_PUNCTUATIONS),
            punctBounds);
        break;
      default:
        throw new AssertionError(pmode);
    }
    // windowing
    if (iport.getWindowingMode()
        != OperatorModelProperties.InputPortSet.WindowingMode.NonWindowed) {
      switch (iport.getWindowingMode()) {
        case OptionallyWindowed:
          paintWindow(false, bounds);
          break;
        case Windowed:
          paintWindow(true, bounds);
          break;
        default:
          throw new AssertionError(pmode);
      }
    }
  }

  private void paintWindow(boolean mandatory, Rectangle2D.Float portBounds) {
    Rectangle2D.Float windowBounds =
        new Rectangle2D.Float(
            portBounds.x + portBounds.width * 1.2f,
            (float) portBounds.getCenterY() - portBounds.height * 0.3f,
            operBounds_.width / 5,
            portBounds.height * 0.6f);
    if (!mandatory) graphics_.setColor(RenderingDescriptor.PORT_OPTIONAL_WINDOWING_COLOR);
    else graphics_.setColor(RenderingDescriptor.PORT_MANDATORY_WINDOWING_COLOR);
    graphics_.fill(windowBounds);
    if (!mandatory) graphics_.setColor(Color.black);
    else graphics_.setColor(Color.white);
    drawTextWithinBounds("w", windowBounds, true);
    graphics_.setColor(Color.black);
    Logger logger = Logger.getInstance();
    String description = "";
    if (mandatory)
      description = /*"This port always has a window"*/
          logger.getMessage(Messages.MSG_PORT_WINDOW_MANDATORY);
    else
      description = /*"This port optionally has a window"*/
          logger.getMessage(Messages.MSG_PORT_WINDOW_OPTIONAL);
    Point2D.Float tooltipLoc =
        new Point2D.Float(
            (float) windowBounds.getMaxX() + 2,
            (float) windowBounds.getMaxY() + graphics_.getFontMetrics().getHeight() + 2);
    SVGTooltipManager.getInstance()
        .addTooltip((SVGGraphics2D) graphics_, windowBounds, description, tooltipLoc);
  }

  private void paintOutputPorts() {
    int i = 0;
    for (OperatorModelProperties.OutputPortSet oport : props_.getOutputPortSets()) {
      paintOutputPort(oport, i);
      i++;
    }
  }

  private void paintOutputPort(OperatorModelProperties.OutputPortSet oport, int i) {
    float portWidth = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE;
    float portHeight = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SIZE;
    float portSpacing = RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_SPACING;
    float portYstart =
        (operBounds_.height
                - numOutputPortSets_ * portHeight
                - (numOutputPortSets_ - 1) * portSpacing)
            / 2;
    Rectangle2D.Float bounds =
        new Rectangle2D.Float(
            operBounds_.x + operBounds_.width,
            operBounds_.y + portYstart + i * (portHeight + portSpacing),
            portWidth,
            portHeight);
    boolean optional = oport.isOptional();
    int cardinality = oport.getCardinality();
    boolean mutable = oport.isMutable();
    paintPortBox(bounds, optional, cardinality, mutable);
    // punctuation
    int pwidth = graphics_.getFontMetrics().charWidth('!');
    Rectangle2D.Float punctBounds =
        new Rectangle2D.Float(
            (float) bounds.getMaxX(), (float) bounds.getMinY(), 2f * pwidth, bounds.height);
    Logger logger = Logger.getInstance();
    OperatorModelProperties.OutputPortSet.PunctuationMode pmode = oport.getPunctuationMode();
    switch (pmode) {
      case Free:
        break;
      case Generating:
        paintPunctuation(
            true, /*"This port is generating punctuations"*/
            logger.getMessage(Messages.MSG_PORT_GENERATING_PUNCTUATIONS),
            punctBounds);
        break;
      case Preserving:
        paintPunctuation(
            false, /*"This port is preserving punctuations"*/
            logger.getMessage(Messages.MSG_PORT_PRESERVING_PUNCTUATIONS),
            punctBounds);
        break;
      default:
        throw new AssertionError(pmode);
    }
  }

  private void paintPunctuation(
      boolean mandatory, String description, Rectangle2D.Float punctBounds) {
    if (mandatory) graphics_.setColor(Color.BLACK);
    else graphics_.setColor(Color.GRAY);
    graphics_.setFont(windowPunctuationLabelFont_);
    drawTextWithinBounds("!", punctBounds, true);
    Point2D.Float tooltipLoc =
        new Point2D.Float(
            punctBounds.x + punctBounds.width + 2, punctBounds.y + punctBounds.height + 2);
    SVGTooltipManager.getInstance()
        .addTooltip((SVGGraphics2D) graphics_, punctBounds, description, tooltipLoc);
  }

  private void paintPortBox(
      Rectangle2D.Float bounds, boolean optional, int cardinality, boolean mutable) {
    RoundRectangle2D.Float[] portRectangles =
        RenderingDescriptor.getRoundedRectanglesWithBorder(
            bounds.x,
            bounds.y,
            bounds.width,
            bounds.height,
            RenderingDescriptor.PRIMITIVE_OPERATOR_PORTSET_BORDER_WIDTH,
            1);
    graphics_.setColor(RenderingDescriptor.PORT_BORDER_COLOR);
    graphics_.fill(portRectangles[0]);
    if (optional) graphics_.setColor(RenderingDescriptor.OPTIONAL_PORT_BACKGROUND_COLOR);
    else graphics_.setColor(RenderingDescriptor.PORT_BACKGROUND_COLOR);

    graphics_.fill(portRectangles[1]);
    graphics_.setColor(Color.BLACK);
    String text = "*";
    if (cardinality > 0) text = String.valueOf(cardinality);
    graphics_.setFont(portCardinalityLabelFont_);
    drawTextWithinBounds(text, bounds, true);
    Logger logger = Logger.getInstance();
    String description = null;
    if (mutable) {
      text = "m";
      description = /*"mutable"*/ logger.getMessage(Messages.MSG_MUTABLE);
    } else {
      text = "i";
      description = /*"immutable"*/ logger.getMessage(Messages.MSG_IMMUTABLE);
    }
    graphics_.setFont(portMutabilityLabelFont_);
    FontMetrics metrics = graphics_.getFontMetrics();
    Rectangle2D.Float mutabilityBounds =
        new Rectangle2D.Float(
            bounds.x, bounds.y - metrics.getHeight(), bounds.width, metrics.getHeight());
    drawTextWithinBounds(text, mutabilityBounds, true);
    Point2D.Float tooltipLoc =
        new Point2D.Float(
            mutabilityBounds.x + mutabilityBounds.width + 2,
            mutabilityBounds.y + mutabilityBounds.height + 2);
    SVGTooltipManager.getInstance()
        .addTooltip((SVGGraphics2D) graphics_, mutabilityBounds, description, tooltipLoc);
  }

  /// Draw within bounds, if it does not fit, shorted with ellipses and add a tooltip
  private void drawTextWithinBounds(
      final String text, final Rectangle2D.Float bounds, boolean centerY) {
    FontMetrics metrics = graphics_.getFontMetrics();
    float y =
        centerY
            ? (float) bounds.getCenterY() + metrics.getAscent() - metrics.getHeight() / 2
            : bounds.y + metrics.getAscent();
    float w = metrics.charsWidth(text.toCharArray(), 0, text.length());
    if (w > bounds.width) {
      String shortText = text;
      boolean done = false;
      while (!done && text.length() > 1) {
        int length = shortText.length();
        shortText = shortenString(shortText, length - 1);
        if (shortText.length() == length) done = true;
        else {
          w = metrics.charsWidth(shortText.toCharArray(), 0, shortText.length());
          done = (w <= bounds.getWidth());
        }
      }
      graphics_.setClip(bounds);
      float x = bounds.x;
      graphics_.drawString(shortText, x, y);
      graphics_.setClip(null);
      // RTC 18602 - Compute y coordinate for the position of the tooltip.
      // Leave enough room to accommodate the height of the tooltip, if the
      // position is close to y == 0;
      float y_pos = bounds.y - bounds.height / 3;
      if (y_pos < bounds.height) {
        // Add 2 to accommodate border of tooltip box
        y_pos = bounds.height + 2;
      }
      SVGTooltipManager.getInstance()
          .addTooltip(
              (SVGGraphics2D) graphics_, bounds, text, new Point2D.Float(bounds.x + 2, y_pos));
    } else {
      float x = (float) bounds.getCenterX() - w / 2;
      graphics_.drawString(text, x, y);
    }
  }

  private void finalizeTooltips() {
    SVGTooltipManager.getInstance().finalizeTooltips(graphics_);
  }

  /// Shorten a long text by putting "..." before 1/4ths of it from the end,
  /// if longer than a given bound
  /// @param txt source text
  /// @param n maximum length of the result
  /// @return the shortened string
  String shortenString(String text, int n) {
    if (text.length() <= n) {
      return text;
    } else if (n < 4) {
      return text.substring(0, n);
    } else {
      int r = n / 4;
      int m = n - r;
      String str = text.substring(0, m - 3);
      str += "...";
      str += text.substring(text.length() - r);
      return str;
    }
  }
}
