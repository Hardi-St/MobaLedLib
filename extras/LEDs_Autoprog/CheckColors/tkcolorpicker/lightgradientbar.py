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



from tkcolorpicker.functions import tk, round2, rgb_to_hexa, hue2col

max_value = 100.

class LightGradientBar(tk.Canvas):
    """brighness gradient light bar with selection cursor."""
    
    def __init__(self, parent, hue=0, value=0, height=11, width=256, variable=None,
                 **kwargs):
        """
        Create a GradientBar.

        Keyword arguments:
            * parent: parent window
            * hue: initially selected hue value
            * variable: IntVar linked to the alpha value
            * height, width, and any keyword argument accepted by a tkinter Canvas
        """
        tk.Canvas.__init__(self, parent, width=width, height=height, **kwargs)

        self._variable = variable
        if variable is not None:
            try:
                value = int(variable.get())
            except Exception:
                pass
        else:
            self._variable = tk.IntVar(self)
        if value > max_value:
            value = int(max_value)
        elif value < 0:
            value = 0
        self._variable.set(value)
        try:
            self._variable.trace_add("write", self._update_value)
        except Exception:
            self._variable.trace("w", self._update_value)

        self.gradient = tk.PhotoImage(master=self, width=width, height=height)

        self.bind('<Configure>', lambda e: self._draw_gradient(value))
        self.bind('<ButtonPress-1>', self._on_click)
        self.bind('<B1-Motion>', self._on_move)

    def _draw_gradient(self, value):
        """Draw the gradient and put the cursor on hue."""
        self.delete("gradient")
        self.delete("cursor")
        del self.gradient
        width = self.winfo_width()
        height = self.winfo_height()

        self.gradient = tk.PhotoImage(master=self, width=width, height=height)

        line = []
        for i in range(width):
            col = int(float(i) / width * 255)
            rij = col
            gij = col
            bij = col
            line.append(rgb_to_hexa(rij, gij, bij))
                        
        line = "{" + " ".join(line) + "}"
        self.gradient.put(" ".join([line for j in range(height)]))
        self.create_image(0, 0, anchor="nw", tags="gradient",
                          image=self.gradient)
        self.lower("gradient")

        x = value / max_value * width
        self.create_line(x, 0, x, height, width=4, tags='cursor',fill="RED")

    def _on_click(self, event):
        """Move selection cursor on click."""
        x = event.x
        self.coords('cursor', x, 0, x, self.winfo_height())
        self._variable.set(round2((max_value * x) / self.winfo_width()))

    def _on_move(self, event):
        """Make selection cursor follow the cursor."""
        w = self.winfo_width()
        x = min(max(event.x, 0), w)
        self.coords('cursor', x, 0, x, self.winfo_height())
        self._variable.set(round2((max_value * x) / w))

    def _update_value(self, *args):
        value = int(self._variable.get())
        if value > max_value:
            value = int(max_value)
        elif value < 0:
            value = 0
        self.set(0,0,value)
        self.event_generate("<<HueChanged>>")

    def get(self):
        """Return hue of color under cursor."""
        coords = self.coords('cursor')
        return round2(max_value * coords[0] / self.winfo_width())

    def set(self, hue, sat, value):
        """Set cursor position on the color corresponding to the hue value."""
        x = value / max_value * self.winfo_width()
        self.coords('cursor', x, 0, x, self.winfo_height())
        self._variable.set(value)
