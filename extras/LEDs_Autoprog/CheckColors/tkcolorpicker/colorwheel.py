# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
# * Version: 1.0
# * Author: Harold Linke
# * Date: November 23rd, 2019
# * Copyright: Harold Linke 2019
# *
# * 
# * MobaLedCheckColors on Github: https://github.com/haroldlinke/MobaLedCheckColors
# * 
# *
# * History of Change
# * V1.00 23.11.2019 - Harold Linke - first release
# *
# * MobaLedCheckColors supports the MobaLedLib by Hardi Stengelin
# * https://github.com/Hardi-St/MobaLedLib
# *
# * MobaLedCheckColors is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# * 
# * MobaLedCheckColors is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# * 
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# * MobaLedCheckColors is based on tkColorPicker by Juliette Monsel
# * https://sourceforge.net/projects/tkcolorpicker/
# * 
# * tkcolorpicker - Alternative to colorchooser for Tkinter.
# * Copyright 2017 Juliette Monsel <j_4321@protonmail.com>
# * 
# * tkcolorpicker is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# * 
# * tkcolorpicker is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# * 
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# * 
# * The code for changing pages was derived from: http://stackoverflow.com/questions/7546050/switch-between-two-frames-in-tkinter
# * License: http://creativecommons.org/licenses/by-sa/3.0/	
# ***************************************************************************



from tkcolorpicker.functions import tk, round2, rgb_to_hexa, hue2col, rgb_to_hsv, hsv_to_rgb
import math

class ColorWheel(tk.Canvas):
    """Wheel color gradient with selection cross."""

    def __init__(self, parent, hue, color=None, cr=100, cg=69, cb=94, height=256, width=256, **kwargs):
        """
        Create a ColorSquare.

        Keyword arguments:
            * parent: parent window
            * hue: color square gradient for given hue (color in top right corner
                   is (hue, 100, 100) in HSV
            * color: initially selected color given in HSV
            * width, height and any keyword option accepted by a tkinter Canvas
        """
        tk.Canvas.__init__(self, parent, height=height, width=width, **kwargs)
        self.bg = tk.PhotoImage(width=width, height=height, master=self)
        self._hue = hue
        if not color:
            color = hue2col(self._hue)
        self.bind('<Configure>', lambda e: self._draw(color))
        self.bind('<ButtonPress-1>', self._on_click)
        self.bind('<B1-Motion>', self._on_move)
        
        self.cr = cr
        self.cg = cg
        self.cb = cb

    def _fill(self):
        """Create the gradient."""
        r, g, b = hue2col(self._hue)
        width = self.winfo_width()
        height = self.winfo_height()
        h = float(height - 1)
        w = float(width - 1)

        if height:
            data = []
            brightness = int(self._hue) #int(self._hue/255*100)
            
            radius = min(height,width)/2.0
            centre = height/2, width/2
            crf = 100/self.cr
            cgf = 100/self.cg
            cbf = 100/self.cb
            for y in range(height):
                line = []
                ry = y - centre[1]
                y2 = ry*ry
                for x in range(width):
                    rx = x - centre[0]
                    x2 = rx*rx                    
                    
                    s = math.sqrt(x2 + y2) / radius
#                    s = ((x2 + (y - centre[1])**2.0)**0.5 / radius
                    if s <= 1.0:
                        h = ((math.atan2(ry, rx) / math.pi) + 1.0) / 2.0
                        rxy,gxy,bxy = hsv_to_rgb(h*360, s*100, 100)

                        rxy = int(rxy*crf)
                        gxy =  int(gxy*cgf)
                        bxy =  int(bxy*cbf)
                        if rxy > 255: rxy = 255
                        if gxy > 255: gxy = 255
                        if bxy > 255: bxy = 255

                        color = rgb_to_hexa(rxy, gxy, bxy)
                        line.append(color)
                    else:
                        color = rgb_to_hexa(255, 255, 255)
                        line.append(color)
                        
                data.append("{" + " ".join(line) + "}")
            self.bg.put(" ".join(data))
        
    def _draw(self, color):
        """Draw the gradient and the selection cross on the canvas."""
        width = self.winfo_width()
        height = self.winfo_height()
        self.delete("bg")
        self.delete("cross_h")
        self.delete("cross_v")
        del self.bg
        self.bg = tk.PhotoImage(width=width, height=height, master=self)
        self._fill()
        self.create_image(0, 0, image=self.bg, anchor="nw", tags="bg")
        self.tag_lower("bg")
        h, s, v = color
        
        radius = min(height,width)/2.0
        centre = height/2, width/2 
        radh = math.radians(h)

        x = centre[1] - width * math.cos(radh) * s/200
        y = centre[0] - height * math.sin(radh) * s/200        

        self.create_line(0, y , width, y , tags="cross_h",
                         fill="#C2C2C2")
        self.create_line(x , 0, x , height, tags="cross_v",
                         fill="#C2C2C2")

    def get_hue(self):
        """Return hue."""
        return self._hue

    def set_hue(self, value):
        """Set hue."""
        old = self._hue
        self._hue = value
        if value != old:
            #self._fill()
            self.event_generate("<<ColorChanged>>")
            
    def set_colorcorrection(self, cr, cg, cb):
        """Set hue."""
        self.cr = int(cr)
        self.cg = int(cg)
        self.cb = int(cb)
        self._fill()
        self.event_generate("<<ColorChanged>>")
            

    def _on_click(self, event):
        """Move cross on click."""
        x = event.x
        y = event.y
        
        self.coords('cross_h', 0, y, self.winfo_width(), y)
        self.coords('cross_v', x, 0, x, self.winfo_height())
        self.event_generate("<<ColorChanged>>")

    def _on_move(self, event):
        """Make the cross follow the cursor."""
        w = self.winfo_width()
        h = self.winfo_height()
        x = min(max(event.x, 0), w)
        y = min(max(event.y, 0), h)
        self.coords('cross_h', 0, y, w, y)
        self.coords('cross_v', x, 0, x, h)
        self.event_generate("<<ColorChanged>>")

    def get(self):
        """Return selected color with format (RGB, HSV, HEX)."""
        x = self.coords('cross_v')[0]
        y = self.coords('cross_h')[1]
        xp = min(x, self.bg.width() - 1)
        yp = min(y, self.bg.height() - 1)
        try:
            r, g, b = self.bg.get(round2(xp), round2(yp))
        except ValueError:
            r, g, b = self.bg.get(round2(xp), round2(yp)).split()
            r, g, b = int(r), int(g), int(b)

        h, s, v = rgb_to_hsv(r, g, b)

        height = self.bg.height()
        width = self.bg.width()
        radius = min(height,width)/2.0
        centre = height/2, width/2            

        rx = x - centre[0]
        ry = y - centre[1]
        
        v = self.get_hue()
                
        s = ((x - centre[0])**2.0 + (y - centre[1])**2.0)**0.5 / radius
        if s > 1.0: 
            s = 100
        else:
            s = int(s*100)
            
        h = int(360 * ((math.atan2(ry, rx) / math.pi) + 1.0) / 2.0)
        
        
        r,g,b = hsv_to_rgb(h, s, v)                    
                      
        hexa = rgb_to_hexa(r, g, b)            

#        r, g, b = hsv_to_rgb(h, s, v)
#        s = round2((1 - float(y) / self.winfo_height()) * 100)
#        v = round2(100 * float(x) / self.winfo_width())
        return (r, g, b), (h, s, v), hexa

    def set_rgb(self, sel_color):
        """Put cursor on sel_color given in RGB."""
        width = self.winfo_width()
        height = self.winfo_height()
        h, s, v = rgb_to_hsv(*sel_color)
        self.set_hue(v)
        
        radius = min(height,width)/2.0
        centre = height/2, width/2 
        
        x = centre[1] - width * math.cos(radh) * s/200
        y = centre[0] - height * math.sin(radh) * s/200
        
        self.coords('cross_h', 0, y, width, y)
        self.coords('cross_v', x, 0, x, height)

    def set_hsv(self, sel_color):
        """Put cursor on sel_color given in HSV."""
        width = self.winfo_width()
        height = self.winfo_height()
        h, s, v = sel_color
        self.set_hue(v)
        
        radius = min(height,width)/2.0
        centre = height/2, width/2 
        radh = math.radians(h)

        x = centre[1] - width * math.cos(radh) * s/200
        y = centre[0] - height * math.sin(radh) * s/200
        self.coords('cross_h', 0, y, width, y)
        self.coords('cross_v', x, 0, x, height)
