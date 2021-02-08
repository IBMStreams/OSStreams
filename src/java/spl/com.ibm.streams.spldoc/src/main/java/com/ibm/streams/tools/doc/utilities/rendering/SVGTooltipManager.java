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
import java.awt.FontMetrics;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import org.apache.batik.svggen.DOMTreeManager;
import org.apache.batik.svggen.SVGGraphics2D;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;

public final class SVGTooltipManager {
  /**
   * A Singleton class that maintains a list of tooltips that will be added at the end of the DOM of
   * an SVG image. Tooltips must be added at the end of the DOM because SVG images have no native
   * Z-ordering; the contents are layered in the order in which it appears in the DOM.
   */
  private static final String tooltipIDPrefix = "sdt";

  private static SVGTooltipManager instance = null;

  private final ArrayList<TooltipTuple> tooltipRegistry = new ArrayList<TooltipTuple>();

  private SVGTooltipManager() {} // Allows instanciation via the getInstance() method only.

  public static SVGTooltipManager getInstance() {
    if (instance == null) {
      instance = new SVGTooltipManager();
    }

    return instance;
  }

  public synchronized void addTooltip(
      SVGGraphics2D graphics,
      Rectangle2D.Float triggerBox,
      String tipText,
      Point2D.Float location) {

    tooltipRegistry.add(new TooltipTuple(tipText, location));
    String triggerBoxID = tooltipIDPrefix + Integer.toString(tooltipRegistry.size() - 1);

    Element triggerBoxE = graphics.getDOMFactory().createElement("rect");
    addAttributeToElement(graphics, "id", triggerBoxID, triggerBoxE);
    addAttributeToElement(graphics, "x", Float.toString(triggerBox.x), triggerBoxE);
    addAttributeToElement(graphics, "y", Float.toString(triggerBox.y), triggerBoxE);
    addAttributeToElement(graphics, "width", Float.toString(triggerBox.width), triggerBoxE);
    addAttributeToElement(graphics, "height", Float.toString(triggerBox.height), triggerBoxE);
    addAttributeToElement(graphics, "style", "stroke-opacity:0; fill-opacity:0;", triggerBoxE);

    // Add a group containing the rect element, to the Document.
    // We have to add a group, as Batik does not provide a mechanism to obtain the current group
    // in order to add child Elements to that group.
    graphics.getDOMTreeManager().appendGroup(triggerBoxE, null);
  }

  private void addAttributeToElement(
      SVGGraphics2D graphics, String name, String value, Element element) {
    Attr a = graphics.getDOMFactory().createAttribute(name);
    a.setNodeValue(value);
    element.setAttributeNodeNS(a);
  }

  public void finalizeTooltips(SVGGraphics2D graphics) {

    FontMetrics tipMetrics = graphics.getFontMetrics(new Font("Helvetica", Font.ITALIC, 12));

    int registrySize = tooltipRegistry.size();

    Document domFactory = graphics.getDOMFactory();
    DOMTreeManager dtManager = graphics.getDOMTreeManager();

    for (int i = 0; i < registrySize; ++i) {
      // Code to be run for each tooltip entry in the registry....

      TooltipTuple tip = tooltipRegistry.get(i);
      String triggerBoxID = tooltipIDPrefix + Integer.toString(i);

      Element tooltipE = domFactory.createElement("g");
      addAttributeToElement(
          graphics,
          "style",
          "stroke:none;font-style:italic;"
              + "font-size:12px;"
              + "font-family:'Helvetica','SansSerif';",
          tooltipE);
      addAttributeToElement(graphics, "visibility", "hidden", tooltipE);

      // Add the group containing the tooltip, to the Document.
      dtManager.appendGroup(tooltipE, null);

      String[] tipLines = tip.text.split("\\n");
      Color bgc = RenderingDescriptor.TOOLTIP_BACKGROUND_COLOR;
      Color bdc = RenderingDescriptor.TOOLTIP_BORDER_COLOR;

      int maxStringWidth = 0;
      for (String line : tipLines) {
        if (tipMetrics.stringWidth(line) > maxStringWidth)
          maxStringWidth = tipMetrics.stringWidth(line);
      }

      // tooltip balloon
      {
        Element rectE = domFactory.createElement("rect");
        tooltipE.appendChild(rectE);
        addAttributeToElement(graphics, "x", Float.toString(tip.location.x), rectE);
        addAttributeToElement(
            graphics,
            "y",
            Float.toString(
                tip.location.y
                    // RTC 18602 - y position of the balloon needs to accommodate
                    // the number of lines in the tooltip
                    - (tipMetrics.getHeight() * tipLines.length)
                    + tipMetrics.getMaxDescent()
                    - RenderingDescriptor.TOOLTIP_BALLOON_TOP_MARGIN),
            rectE);
        addAttributeToElement(
            graphics,
            "width",
            Float.toString(
                RenderingDescriptor.TOOLTIP_BALLOON_LEFT_MARGIN
                    + (float) maxStringWidth * RenderingDescriptor.TOOLTIP_BALLOON_WIDTH_FACTOR
                    + RenderingDescriptor.TOOLTIP_BALLOON_RIGHT_MARGIN),
            rectE);
        addAttributeToElement(
            graphics,
            "height",
            Float.toString(
                tipMetrics.getHeight() * tipLines.length
                    + RenderingDescriptor.TOOLTIP_BALLOON_TOP_MARGIN
                    + RenderingDescriptor.TOOLTIP_BALLOON_BOTTOM_MARGIN),
            rectE);
        addAttributeToElement(
            graphics,
            "style",
            "fill:rgb("
                + bgc.getRed()
                + ","
                + bgc.getGreen()
                + ","
                + bgc.getBlue()
                + ");"
                + "stroke:rgb("
                + bdc.getRed()
                + ","
                + bdc.getGreen()
                + ","
                + bdc.getBlue()
                + ");",
            rectE);
        addAttributeToElement(
            graphics,
            "rx",
            Float.toString(RenderingDescriptor.TOOLTIP_BALLOON_CORNER_RADIUS),
            rectE);
        addAttributeToElement(
            graphics,
            "ry",
            Float.toString(RenderingDescriptor.TOOLTIP_BALLOON_CORNER_RADIUS),
            rectE);
      }

      // tooltip text line(s)
      for (int lineNumber = 0; lineNumber < tipLines.length; ++lineNumber) {
        Element textE = domFactory.createElement("text");
        tooltipE.appendChild(textE);
        Text textN = domFactory.createTextNode(tipLines[lineNumber]);
        textE.appendChild(textN);
        addAttributeToElement(
            graphics,
            "x",
            Float.toString(tip.location.x + RenderingDescriptor.TOOLTIP_BALLOON_LEFT_MARGIN),
            textE);
        // RTC 18602 - Position each line relative to the top of the balloon.
        int y_line = ((tipLines.length - 1) - lineNumber) * tipMetrics.getHeight();
        addAttributeToElement(graphics, "y", Float.toString(tip.location.y - y_line), textE);
        // addAttributeToElement(graphics, "y", Float.toString(tip.location.y +
        // lineNumber*tipMetrics.getHeight()),
        // textE);
      }

      { // tooltip activator
        Element setE = domFactory.createElement("set");
        addAttributeToElement(graphics, "attributeName", "visibility", setE);
        addAttributeToElement(graphics, "from", "hidden", setE);
        addAttributeToElement(graphics, "to", "visible", setE);
        addAttributeToElement(graphics, "begin", triggerBoxID + ".mouseover", setE);
        addAttributeToElement(graphics, "end", triggerBoxID + ".mouseout", setE);
        tooltipE.appendChild(setE);
      }
    } // for loop

    tooltipRegistry.clear();
  }

  private final class TooltipTuple {

    private final String text;
    private final Point2D.Float location;

    private TooltipTuple(String text, Point2D.Float location) {
      this.text = text;
      this.location = location;
    }
  }
}
