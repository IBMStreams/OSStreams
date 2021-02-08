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

package com.ibm.streams.tools.doc.utilities.rendering;

import java.awt.Color;
import java.awt.Font;
import java.awt.geom.RoundRectangle2D;
import java.util.Hashtable;

/**
 * The rendering descriptor describes how the node graphics and its pins are supposed to be drawn.
 */
public class RenderingDescriptor {

  // TODO: Remove disabled code for color definitions

  // Colors
  // Node border colors 6699cc (102, 153, 204), thickness: 1px, corner diameter: 10px

  // public static final Color OPERATOR_NODE_BODY_BORDER_COLOR = 	new Color(72,79,149);
  public static final Color OPERATOR_NODE_BODY_BORDER_COLOR = new Color(102, 153, 204);

  // public static final Color OPERATOR_NODE_BODY_FILL_COLOR = 		new Color(158,221,249);
  // 1st file color, fafdfe (250, 253, 254), starting color
  public static final Color OPERATOR_NODE_BODY_FILL_COLOR = new Color(250, 253, 254);
  // 2nd fill color: c6ecf7 (198, 236,247), ending color at 90% of rectangle
  public static final Color OPERATOR_NODE_BODY_FILL_COLOR_1 = new Color(198, 236, 247);
  // 3rd fill color: 8aa5ad, (138, 165, 173), ending color at 100% of rectangle
  public static final Color OPERATOR_NODE_BODY_FILL_COLOR_2 = new Color(138, 165, 173);

  // public static final Color COMPOSITE_FRAME_BACKGROUND_COLOR = 	new Color(234,245,250);
  // Use white background - same as Studio.
  public static final Color COMPOSITE_FRAME_BACKGROUND_COLOR = Color.WHITE;
  // public static final Color COMPOSITE_FRAME_TITLEBAR_COLOR = 		new Color(0,0,102);
  // Use same color as Studio for composite title bar
  public static final Color COMPOSITE_FRAME_TITLEBAR_COLOR = new Color(0, 0.545f, 0.9372f, 0.6f);
  public static final Color COMPOSITE_FRAME_TITLE_COLOR = Color.BLACK;

  public static final Color PORT_MANDATORY_WINDOWING_COLOR = Color.BLACK;
  public static final Color PORT_OPTIONAL_WINDOWING_COLOR = Color.GRAY;

  public static final Color PORT_BACKGROUND_COLOR = new Color(255, 247, 142);
  public static final Color OPTIONAL_PORT_BACKGROUND_COLOR = Color.GRAY;
  public static final Color PORT_BORDER_COLOR = new Color(236, 131, 101);

  // public static final Color GRAPH_LINK_COLOR = 					Color.GRAY;
  // Use same color as Studio for links.
  public static final Color GRAPH_LINK_COLOR = new Color(0, 102, 153);

  public static final Color TRANSPARENT_BLACK_BACKGROUND_COLOR = new Color(0, 0, 0, 0);

  public static final Color TOOLTIP_BACKGROUND_COLOR = new Color(230, 230, 255);
  public static final Color TOOLTIP_BORDER_COLOR = new Color(0, 0, 80);

  // Sizes

  public static final int COMPOSITE_GRAPH_IMAGE_MIN_WIDTH = 320;
  public static final int COMPOSITE_GRAPH_IMAGE_MIN_HEIGHT = 240;
  public static final int COMPOSITE_GRAPH_IMAGE_MAX_WIDTH = 640;
  public static final int COMPOSITE_GRAPH_IMAGE_MAX_HEIGHT = 480;
  public static final int COMPOSITE_GRAPH_IMAGE_MARGIN = 2;
  public static final double COMPOSITE_GRAPH_IMAGE_MAX_ZOOM = 1.8;

  public static final float GRAPH_NODE_DEFAULT_WIDTH = 65;
  public static final float GRAPH_NODE_DEFAULT_HEIGHT = 40;

  // An approximate width that should accommodate most tooltips.
  public static final float PRIMITIVE_OPERATOR_TOOLTIP_DEFAULT_WIDTH = 700;
  // RTC 18602 - the number of characters in a tooltip line
  public static final float PRIMITIVE_OPERATOR_TOOLTIP_LINE_WIDTH = 40;
  public static final float PRIMITIVE_OPERATOR_BODY_DEFAULT_WIDTH = 120;
  public static final float PRIMITIVE_OPERATOR_BODY_DEFAULT_HEIGHT = 80;

  public static final float PRIMITIVE_OPERATOR_MARGIN = 10;

  // public static final float OPERATOR_NODE_BODY_BORDER_WIDTH = 		3;
  public static final float OPERATOR_NODE_BODY_BORDER_WIDTH = 1;

  public static final float COMPOSITE_FRAME_SHADOW_WIDTH = 1;
  public static final float COMPOSITE_FRAME_MARGIN = 24;

  public static final float GRAPH_NODE_PORT_SIZE = 4;
  public static final float GRAPH_NODE_PORT_SPACING = 4;
  public static final float GRAPH_NODE_PORT_BORDER_WIDTH = 1;
  public static final float GRAPH_OUTER_PORT_SIZE = 8;
  public static final float GRAPH_OUTER_PORT_SPACING = 8;
  public static final float GRAPH_OUTER_PORT_BORDER_WIDTH = 2;
  public static final float PRIMITIVE_OPERATOR_PORTSET_SIZE = 16;
  public static final float PRIMITIVE_OPERATOR_PORTSET_SPACING = 16;
  public static final float PRIMITIVE_OPERATOR_PORTSET_BORDER_WIDTH = 2;

  public static final float GRAPH_LINK_WIDTH = 2;

  public static final float TOOLTIP_BALLOON_LEFT_MARGIN = 1;
  public static final float TOOLTIP_BALLOON_RIGHT_MARGIN = 1;
  public static final float TOOLTIP_BALLOON_BOTTOM_MARGIN = 1;
  public static final float TOOLTIP_BALLOON_TOP_MARGIN = 1;
  public static final float TOOLTIP_BALLOON_CORNER_RADIUS = 2;
  // RTC 18815 - A workaround to increase width of tooltip to better fix text.
  // public static final float TOOLTIP_BALLOON_WIDTH_FACTOR = 		0.96f;
  public static final float TOOLTIP_BALLOON_WIDTH_FACTOR = 1.06f;

  // Fonts
  // RTC 27182: Going from RHEL6 to RHEL7 the characters become compressed.
  // Changing from SERIF to SANS_SERIF means much less compression and better legibility.

  public static final Font COMPOSITE_FRAME_TITLE_FONT = new Font(Font.SANS_SERIF, Font.PLAIN, 12);

  public static final Font FONT_BOLD_DEFAULT = new Font(Font.SANS_SERIF, Font.BOLD, 8);

  @SuppressWarnings("serial")
  public static final Hashtable<Integer, Font> FONTS_BOLD =
      new Hashtable<Integer, Font>() {
        {
          put(new Integer(4), new Font(Font.SANS_SERIF, Font.BOLD, 4));
          put(new Integer(5), new Font(Font.SANS_SERIF, Font.BOLD, 5));
          put(new Integer(6), new Font(Font.SANS_SERIF, Font.BOLD, 6));
          put(new Integer(7), new Font(Font.SANS_SERIF, Font.BOLD, 7));
          put(new Integer(8), FONT_BOLD_DEFAULT);
          put(new Integer(9), new Font(Font.SANS_SERIF, Font.BOLD, 9));
          put(new Integer(10), new Font(Font.SANS_SERIF, Font.BOLD, 10));
          put(new Integer(11), new Font(Font.SANS_SERIF, Font.BOLD, 11));
          put(new Integer(12), new Font(Font.SANS_SERIF, Font.BOLD, 12));
          put(new Integer(13), new Font(Font.SANS_SERIF, Font.BOLD, 13));
          put(new Integer(14), new Font(Font.SANS_SERIF, Font.BOLD, 14));
          put(new Integer(15), new Font(Font.SANS_SERIF, Font.BOLD, 15));
          put(new Integer(16), new Font(Font.SANS_SERIF, Font.BOLD, 16));
          put(new Integer(17), new Font(Font.SANS_SERIF, Font.BOLD, 17));
          put(new Integer(18), new Font(Font.SANS_SERIF, Font.BOLD, 18));
          put(new Integer(19), new Font(Font.SANS_SERIF, Font.BOLD, 19));
          put(new Integer(20), new Font(Font.SANS_SERIF, Font.BOLD, 20));
          put(new Integer(21), new Font(Font.SANS_SERIF, Font.BOLD, 21));
          put(new Integer(22), new Font(Font.SANS_SERIF, Font.BOLD, 22));
          put(new Integer(23), new Font(Font.SANS_SERIF, Font.BOLD, 23));
          put(new Integer(24), new Font(Font.SANS_SERIF, Font.BOLD, 24));
        }
      };

  public static final Font FONT_PLAIN_DEFAULT = new Font(Font.SANS_SERIF, Font.PLAIN, 8);

  @SuppressWarnings("serial")
  public static final Hashtable<Integer, Font> FONTS_PLAIN =
      new Hashtable<Integer, Font>() {
        {
          put(new Integer(4), new Font(Font.SANS_SERIF, Font.PLAIN, 4));
          put(new Integer(5), new Font(Font.SANS_SERIF, Font.PLAIN, 5));
          put(new Integer(6), new Font(Font.SANS_SERIF, Font.PLAIN, 6));
          put(new Integer(7), new Font(Font.SANS_SERIF, Font.PLAIN, 7));
          put(new Integer(8), FONT_PLAIN_DEFAULT);
          put(new Integer(9), new Font(Font.SANS_SERIF, Font.PLAIN, 9));
          put(new Integer(10), new Font(Font.SANS_SERIF, Font.PLAIN, 10));
          put(new Integer(11), new Font(Font.SANS_SERIF, Font.PLAIN, 11));
          put(new Integer(12), new Font(Font.SANS_SERIF, Font.PLAIN, 12));
          put(new Integer(13), new Font(Font.SANS_SERIF, Font.PLAIN, 13));
          put(new Integer(14), new Font(Font.SANS_SERIF, Font.PLAIN, 14));
          put(new Integer(15), new Font(Font.SANS_SERIF, Font.PLAIN, 15));
          put(new Integer(16), new Font(Font.SANS_SERIF, Font.PLAIN, 16));
          put(new Integer(17), new Font(Font.SANS_SERIF, Font.PLAIN, 17));
          put(new Integer(18), new Font(Font.SANS_SERIF, Font.PLAIN, 18));
          put(new Integer(19), new Font(Font.SANS_SERIF, Font.PLAIN, 19));
          put(new Integer(20), new Font(Font.SANS_SERIF, Font.PLAIN, 20));
          put(new Integer(21), new Font(Font.SANS_SERIF, Font.PLAIN, 21));
          put(new Integer(22), new Font(Font.SANS_SERIF, Font.PLAIN, 22));
          put(new Integer(23), new Font(Font.SANS_SERIF, Font.PLAIN, 23));
          put(new Integer(24), new Font(Font.SANS_SERIF, Font.PLAIN, 24));
        }
      };

  // Logic and Calculation Utilities

  protected static final int fontSizeMinimum = 4;
  protected static final int fontSizeDefault = 12;
  protected static final int fontSizeMaximum = 24;

  protected static final double lineSpacingMinimum = 1.6;
  protected static final double lineSpacingDefault = 1.6;
  protected static final double lineSpacingMaximum = 3;

  protected static final int marginMinimum = 5;
  protected static final int marginDefault = 5;
  protected static final int marginMaximum = 10;

  public static Font getNameFont(double zoom) {

    // Make the font scale a bit slower than the zoom
    double fontFactorOfZoom = 0.7;
    int fontSize = new Double(zoom * fontFactorOfZoom * fontSizeDefault).intValue();
    // Make sure it is at least the minimum
    if (fontSize < fontSizeMinimum) {
      fontSize = fontSizeMinimum;
    }
    // Make sure it is not larger than the maximum
    else if (fontSize > fontSizeMaximum) {
      fontSize = fontSizeMaximum;
    }

    Integer key = new Integer(fontSize);
    Font font = FONTS_BOLD.get(key);
    if (font == null) {
      font = FONT_BOLD_DEFAULT;
    }

    return font;
  }

  public static Font getDetailsFont(double zoom) {

    // Make the font scale a bit slower than the zoom
    double fontFactorOfZoom = 0.6;
    int fontSize = new Double(zoom * fontFactorOfZoom * fontSizeDefault).intValue();
    // Make sure it is at least the minimum
    if (fontSize < fontSizeMinimum) {
      fontSize = fontSizeMinimum;
    }
    // Make sure it is not larger than the maximum
    else if (fontSize > fontSizeMaximum) {
      fontSize = fontSizeMaximum;
    }

    Integer key = new Integer(fontSize);
    Font font = FONTS_PLAIN.get(key);
    if (font == null) {
      font = FONT_PLAIN_DEFAULT;
    }

    return font;
  }

  public static double getLineSpacing(double zoom) {

    double lineSpacing = zoom * lineSpacingDefault;
    // Make sure it is at least the minimum
    if (lineSpacing < lineSpacingMinimum) {
      lineSpacing = lineSpacingMinimum;
    }
    // Make sure it is not larger than the maximum
    else if (lineSpacing > lineSpacingMaximum) {
      lineSpacing = lineSpacingMaximum;
    }

    // System.out.println("LineSpacing: " + lineSpacing);
    return lineSpacing;
  }

  public static int getMargin(double zoom) {

    int margin = new Double(zoom * marginDefault).intValue();
    // Make sure it is at least the minimum
    if (margin < marginMinimum) {
      margin = marginMinimum;
    }
    // Make sure it is not larger than the maximum
    else if (margin > marginMaximum) {
      margin = marginMaximum;
    }

    // System.out.println("Margin: " + margin);
    return margin;
  }

  public static RoundRectangle2D.Float[] getRoundedRectanglesWithBorder(
      float x, float y, float width, float height, float border, float zoomFactor) {
    RoundRectangle2D.Float[] rects = new RoundRectangle2D.Float[2];
    rects[0] =
        new RoundRectangle2D.Float(
            x, y, width, height, Math.min(width, height) / 8, Math.min(width, height) / 8);

    float scaledBorder = border * zoomFactor;
    rects[1] =
        new RoundRectangle2D.Float(
            x + scaledBorder,
            y + scaledBorder,
            width - 2 * scaledBorder,
            height - 2 * scaledBorder,
            Math.min(width - 2 * scaledBorder, height - 2 * scaledBorder) / 8,
            Math.min(width - 2 * border, height - 2 * border) / 8);
    return rects;
  }
}
