# -*- coding: utf-8 -*-
"""
tkcolorpicker - Alternative to colorchooser for Tkinter.
Copyright 2017 Juliette Monsel <j_4321@protonmail.com>

tkcolorpicker is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

tkcolorpicker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Colorpicker dialog
"""


#from PIL import ImageTk
from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
    overlay, PALETTE, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv
from tkcolorpicker.alphabar import AlphaBar
from tkcolorpicker.gradientbar import GradientBar
from tkcolorpicker.colorsquare import ColorSquare
from tkcolorpicker.spinbox import Spinbox
from tkcolorpicker.limitvar import LimitVar
from locale import getdefaultlocale
import re
import math


# --- Translation
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha"}

try:
    TR = EN
    if getdefaultlocale()[0][:2] == 'fr':
        TR = FR
    else:
        if getdefaultlocale()[0][:2] == 'de':
            TR = DE        
except ValueError:
    TR = EN


def _(text):
    """Translate text."""
    return TR.get(text, text)


class ColorPicker(tk.Toplevel):
    """Color picker dialog."""

    def __init__(self, parent=None, color=(255, 0, 0), 
                 title=_("MobaLedLib LED Farbentester"), led=0,ledcount=1,serport=None):
        """
        Create a ColorPicker dialog.

        Arguments:
            * parent: parent window
            * color: initially selected color in rgb or hexa format
            * alpha: alpha channel support (boolean)
            * title: dialog title
        """
        
        self.serport = serport
        tk.Toplevel.__init__(self, parent)

        self.title(title)
        self.transient(self.master)
        self.resizable(False, False)
        self.rowconfigure(1, weight=1)
        geometry = self.geometry(None)
        self.geometry("+100+100")
        self.color = ""
        style = ttk.Style(self)
        style.map("palette.TFrame", relief=[('focus', 'sunken')],
                  bordercolor=[('focus', "#4D4D4D")])
        self.configure(background=style.lookup("TFrame", "background"))

        if isinstance(color, str):
            if re.match(r"^#[0-9A-F]{8}$", color.upper()):
                col = hexa_to_rgb(color)
                self._old_color = col[:3]
                old_color = color[:7]
            elif re.match(r"^#[0-9A-F]{6}$", color.upper()):
                self._old_color = hexa_to_rgb(color)
                old_color = color
            else:
                col = self.winfo_rgb(color)
                self._old_color = tuple(round2(c * 255 / 65535) for c in col)
                args = self._old_color
                old_color = rgb_to_hexa(*args)
        else:
            self._old_color = color[:3]
            old_color = rgb_to_hexa(*color)

        # --- GradientBar
        hue = col2hue(*self._old_color)
        bar = ttk.Frame(self, borderwidth=2, relief='groove')
        self.bar = GradientBar(bar, hue=hue, width=200, highlightthickness=0)
        self.bar.pack()

        # --- ColorSquare
        square = ttk.Frame(self, borderwidth=2, relief='groove')
        self.square = ColorSquare(square, hue=hue, width=200, height=200,
                                  color=rgb_to_hsv(*self._old_color),
                                  highlightthickness=0)
        self.square.pack()

        frame = ttk.Frame(self)
        frame.columnconfigure(1, weight=1)
        frame.rowconfigure(1, weight=1)

        # --- color preview: initial color and currently selected color side by side
        preview_frame = ttk.Frame(frame, relief="groove", borderwidth=2)
        preview_frame.grid(row=0, column=0, sticky="nw", pady=2)
        old_color_prev = tk.Label(preview_frame, background=old_color[:7],
                                  width=5, highlightthickness=0, height=2,
                                  padx=0, pady=0)
        self.color_preview = tk.Label(preview_frame, width=5, height=2,
                                      pady=0, background=old_color[:7],
                                      padx=0, highlightthickness=0)
        old_color_prev.bind("<1>", self._reset_preview)
        old_color_prev.grid(row=0, column=0)
        self.color_preview.grid(row=0, column=1)

        # --- palette
        palette = ttk.Frame(frame)
        palette.grid(row=0, column=1, rowspan=2, sticky="ne")
        for i, col in enumerate(PALETTE):
            coltemp = int(col[-6:-1])
            r,g,b = self._convert_K_to_RGB(coltemp)
            args = (r,g,b)
            hexcolor = rgb_to_hexa(*args)
            f = ttk.Frame(palette, borderwidth=1, relief="raised",
                          style="palette.TFrame")
            l = tk.Label(f, background=hexcolor, width=12, height=2,text=_(col))
            l.bind("<1>", self._palette_cmd)
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)

        col_frame = ttk.Frame(self)

        # --- hsv
        hsv_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        hsv_frame.pack(pady=(0, 4), fill="x")
        hsv_frame.columnconfigure(0, weight=1)
        self.hue = LimitVar(0, 360, self)
        self.saturation = LimitVar(0, 100, self)
        self.value = LimitVar(0, 100, self)

        self.s_h = Spinbox(hsv_frame, from_=0, to=360, width=5, name='spinbox',
                      textvariable=self.hue, command=self._update_color_hsv)
        self.s_s = Spinbox(hsv_frame, from_=0, to=100, width=5,
                      textvariable=self.saturation, name='spinbox',
                      command=self._update_color_hsv)
        self.s_v = Spinbox(hsv_frame, from_=0, to=100, width=5, name='spinbox',
                      textvariable=self.value, command=self._update_color_hsv)
        h, s, v = rgb_to_hsv(*self._old_color)
        self.s_h.delete(0, 'end')
        self.s_h.insert(0, h)
        self.s_s.delete(0, 'end')
        self.s_s.insert(0, s)
        self.s_v.delete(0, 'end')
        self.s_v.insert(0, v)
        self.s_h.grid(row=0, column=1, sticky='w', padx=4, pady=4)
        self.s_s.grid(row=1, column=1, sticky='w', padx=4, pady=4)
        self.s_v.grid(row=2, column=1, sticky='w', padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Farbton [F/f]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Sättigung [S/s]')).grid(row=1, column=0, sticky='e',
                                                        padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Helligkeit [H/h]')).grid(row=2, column=0, sticky='e',
                                                   padx=4, pady=4)

        # --- rgb
        rgb_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        rgb_frame.pack(pady=4, fill="x")
        rgb_frame.columnconfigure(0, weight=1)
        self.red = LimitVar(0, 255, self)
        self.green = LimitVar(0, 255, self)
        self.blue = LimitVar(0, 255, self)

        self.s_red = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.red, command=self._update_color_rgb)
        self.s_green = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                          textvariable=self.green, command=self._update_color_rgb)
        self.s_blue = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                         textvariable=self.blue, command=self._update_color_rgb)
        self.s_red.delete(0, 'end')
        self.s_red.insert(0, self._old_color[0])
        self.s_green.delete(0, 'end')
        self.s_green.insert(0, self._old_color[1])
        self.s_blue.delete(0, 'end')
        self.s_blue.insert(0, self._old_color[2])
        self.s_red.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_green.grid(row=1, column=1, sticky='e', padx=4, pady=4)
        self.s_blue.grid(row=2, column=1, sticky='e', padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Rot (LED 1)[R/r]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün (LED2) [G/g]')).grid(row=1, column=0, sticky='e',
                                                   padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Blau (LED3) [B/b]')).grid(row=2, column=0, sticky='e',
                                                  padx=4, pady=4)
        # --- hexa
        hexa_frame = ttk.Frame(col_frame)
        hexa_frame.pack(fill="x")
        self.hexa = ttk.Entry(hexa_frame, justify="center", width=10, name='entry')
        self.hexa.insert(0, old_color.upper())
        ttk.Label(hexa_frame, text="HTML").pack(side="left", padx=4, pady=(4, 1))
        self.hexa.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)


        # --- LED
        led_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        led_frame.pack(pady=4, fill="x")
        led_frame.columnconfigure(0, weight=1)
        self.lednum = LimitVar(0, 255, self)
        self.ledcount = LimitVar(1, 256, self)
        
        self.s_led = Spinbox(led_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.lednum, command=self._update_led_num)
        self.s_ledcount = Spinbox(led_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledcount, command=self._update_led_count)
        self.s_led.delete(0, 'end')
        self.s_led.insert(0, led)
        self.s_led.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_ledcount.delete(0, 'end')
        self.s_ledcount.insert(1, ledcount)
        self.s_ledcount.grid(row=1, column=1, sticky='e', padx=4, pady=4)       

        ttk.Label(led_frame, text=_('LED Adresse [+/-]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(led_frame, text=_('LED Anzahl [A/a]')).grid(row=1, column=0, sticky='e',
                                                 padx=4, pady=4)
        
        # --- Colortemperature
        ct_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        ct_frame.pack(pady=4, fill="x")
        ct_frame.columnconfigure(0, weight=1)
        ct_min = 1000
        ct_max = 20000
        self.ct = LimitVar(ct_min, ct_max, self)

        self.s_ct = Spinbox(ct_frame, from_=ct_min, to=ct_max, width=5, name='spinbox',
                        textvariable=self.ct, command=self._update_ct, increment = 25)
        self.s_ct.delete(0, 'end')
        self.s_ct.insert(0, ct_min)
        self.s_ct.grid(row=0, column=1, sticky='e', padx=4, pady=4)

        ttk.Label(ct_frame, text=_('Farbtemperature (K) [T/t]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        # --- Buttons
        button_frame = ttk.Frame(self)
        ttk.Button(button_frame, text=_("Alle LED aus [CTRL-c]"), command=self.led_off).pack(side="right", padx=10)
        ttk.Button(button_frame, text=_("Cancel [ESC]"), command=self.cancel).pack(side="right", padx=10)

        # --- placement
        bar.grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        square.grid(row=1, column=0, padx=10, pady=(9, 0), sticky='n')
        col_frame.grid(row=0, rowspan=2, column=1, padx=(4, 10), pady=(10, 4))
        frame.grid(row=3, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        button_frame.grid(row=4, columnspan=2, pady=(0, 10), padx=10)

        # --- bindings
        self.bar.bind("<ButtonRelease-1>", self._change_color, True)
        self.bar.bind("<Button-1>", self._unfocus, True)
        self.square.bind("<Button-1>", self._unfocus, True)
        self.square.bind("<ButtonRelease-1>", self._change_sel_color, True)
        self.square.bind("<B1-Motion>", self._change_sel_color, True)
        self.s_red.bind('<FocusOut>', self._update_color_rgb)
        self.s_green.bind('<FocusOut>', self._update_color_rgb)
        self.s_blue.bind('<FocusOut>', self._update_color_rgb)
        self.s_red.bind('<Return>', self._update_color_rgb)
        self.s_green.bind('<Return>', self._update_color_rgb)
        self.s_blue.bind('<Return>', self._update_color_rgb)
        self.s_red.bind('<Control-a>', self._select_all_spinbox)
        self.s_green.bind('<Control-a>', self._select_all_spinbox)
        self.s_blue.bind('<Control-a>', self._select_all_spinbox)
        self.s_led.bind('<Control-a>', self._select_all_spinbox)
        self.s_ledcount.bind('<Control-a>', self._select_all_spinbox)
        self.s_h.bind('<FocusOut>', self._update_color_hsv)
        self.s_s.bind('<FocusOut>', self._update_color_hsv)
        self.s_v.bind('<FocusOut>', self._update_color_hsv)
        self.s_h.bind('<Return>', self._update_color_hsv)
        self.s_s.bind('<Return>', self._update_color_hsv)
        self.s_v.bind('<Return>', self._update_color_hsv)
        self.s_h.bind('<Control-a>', self._select_all_spinbox)
        self.s_s.bind('<Control-a>', self._select_all_spinbox)
        self.s_v.bind('<Control-a>', self._select_all_spinbox)
        self.s_ct.bind("<FocusOut>", self._update_ct)
        self.s_ct.bind("<Return>", self._update_ct)
        self.s_ct.bind("<Control-a>", self._select_all_entry)
        self.hexa.bind("<FocusOut>", self._update_color_hexa)
        self.hexa.bind("<Return>", self._update_color_hexa)
        self.hexa.bind("<Control-a>", self._select_all_entry)        

        self.bind("F",self.s_h.invoke_buttonup)
        self.bind("f",self.s_h.invoke_buttondown)
        self.bind("S",self.s_s.invoke_buttonup)
        self.bind("s",self.s_s.invoke_buttondown)
        self.bind("H",self.s_v.invoke_buttonup)
        self.bind("h",self.s_v.invoke_buttondown)
        self.bind("R",self.s_red.invoke_buttonup)
        self.bind("r",self.s_red.invoke_buttondown) 
        self.bind("G",self.s_green.invoke_buttonup)
        self.bind("g",self.s_green.invoke_buttondown) 
        self.bind("B",self.s_blue.invoke_buttonup)
        self.bind("b",self.s_blue.invoke_buttondown)
        self.bind("+",self.s_led.invoke_buttonup)
        self.bind("-",self.s_led.invoke_buttondown)         
        self.bind("A",self.s_ledcount.invoke_buttonup)
        self.bind("a",self.s_ledcount.invoke_buttondown)           
        self.bind("T",self.s_ct.invoke_buttonup)
        self.bind("t",self.s_ct.invoke_buttondown)
        self.bind("<Escape>",self.cancel)
        self.bind("<Control-c>",self.led_off)
        self.focus_set()
        self.wait_visibility()

        self.lift()
        self.grab_set()


    def get_color(self):
        """Return selected color, return an empty string if no color is selected."""
        return self.color

    @staticmethod
    def _select_all_spinbox(event):
        """Select all entry content."""
        event.widget.selection('range', 0, 'end')
        return "break"

    @staticmethod
    def _select_all_entry(event):
        """Select all entry content."""
        event.widget.selection_range(0, 'end')
        return "break"

    def _unfocus(self, event):
        """Unfocus palette items when click on bar or square."""
        w = self.focus_get()
        if w != self and 'spinbox' not in str(w) and 'entry' not in str(w):
            self.focus_set()

    def _update_preview(self):
        """Update color preview."""
        color = self.hexa.get()
        self.color_preview.configure(background=color)
        
        ledcount = self.ledcount.get()
        
        if ledcount >0:

#        message = "#L" + '{:02x}'.format(self.lednum.get()) + " " + '{:02x}'.format(self.red.get()) + " " + '{:02x}'.format(self.green.get()) + " " + '{:02x}'.format(self.blue.get()) + "\n"  
            message = "#L" + '{:02x}'.format(self.lednum.get()) + " " + '{:02x}'.format(self.red.get()) + " " + '{:02x}'.format(self.green.get()) + " " + '{:02x}'.format(self.blue.get()) + " " + '{:02x}'.format(self.ledcount.get()) + "\n"  
        else:
            message = "#L" + '{:02x}'.format(self.lednum.get()) + " " + '{:02x}'.format(self.red.get()) + " " + '{:02x}'.format(self.green.get()) + " " + '{:02x}'.format(self.blue.get()) + "\n"  
            
        if self.serport:
            self.serport.write(message.encode())
            print("Message send to ARDUINO: ",message)
#            cc=str(self.serport.readline())
#            print("ARDUINO Feedback:",cc[2:][:-5])        

    def _reset_preview(self, event):
        """Respond to user click on a palette item."""
        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        args = self._old_color
        color = rgb_to_hexa(*args)
        h, s, v = rgb_to_hsv(*self._old_color)
        self.red.set(self._old_color[0])
        self.green.set(self._old_color[1])
        self.blue.set(self._old_color[2])
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self.bar.set(h)
        self.square.set_hsv((h, s, v))
        self._update_preview()

    def _palette_cmd(self, event):
        """Respond to user click on a palette item."""
        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        r, g, b = self.winfo_rgb(label.cget("background"))
        text = event.widget["text"]
        coltemp = int(text[-6:-1])
        self.ct.set(coltemp)
        
        r = round2(r * 255 / 65535)
        g = round2(g * 255 / 65535)
        b = round2(b * 255 / 65535)
        args = (r, g, b)
        color = rgb_to_hexa(*args)
        h, s, v = rgb_to_hsv(r, g, b)
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self.bar.set(h)
        self.square.set_hsv((h, s, v))
        self._update_preview()

    def _change_sel_color(self, event):
        """Respond to motion of the color selection cross."""
        (r, g, b), (h, s, v), color = self.square.get()
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self._update_preview()

    def _change_color(self, event):
        """Respond to motion of the hsv cursor."""
        h = self.bar.get()
        self.square.set_hue(h)
        (r, g, b), (h, s, v), sel_color = self.square.get()
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, sel_color.upper())
        self._update_preview()

    def _change_alpha(self, event):
        """Respond to motion of the alpha cursor."""
        a = self.alphabar.get()
        self.alpha.set(a)
        hexa = self.hexa.get()
        hexa = hexa[:7] + ("%2.2x" % a).upper()
        self.hexa.delete(0, 'end')
        self.hexa.insert(0, hexa)
        self._update_preview()

    def _update_color_hexa(self, event=None):
        """Update display after a change in the HEX entry."""
        color = self.hexa.get().upper()
        self.hexa.delete(0, 'end')
        self.hexa.insert(0, color)
        if re.match(r"^#[0-9A-F]{6}$", color):
            r, g, b = hexa_to_rgb(color)
            self.red.set(r)
            self.green.set(g)
            self.blue.set(b)
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            self.bar.set(h)
            self.square.set_hsv((h, s, v))
        else:
            self._update_color_rgb()
        self._update_preview()

    def _update_color_hsv(self, event=None):
        """Update display after a change in the HSV spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            h = self.hue.get()
            s = self.saturation.get()
            v = self.value.get()
            sel_color = hsv_to_rgb(h, s, v)
            self.red.set(sel_color[0])
            self.green.set(sel_color[1])
            self.blue.set(sel_color[2])
            hexa = rgb_to_hexa(*sel_color)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h)
            self._update_preview()

    def _update_color_rgb(self, event=None):
        """Update display after a change in the RGB spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            r = self.red.get()
            g = self.green.get()
            b = self.blue.get()
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            args = (r, g, b)
            hexa = rgb_to_hexa(*args)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h)
            self._update_preview()
            
    def _update_led_num(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            led = self.lednum.get()
            self._update_preview()

    def _update_led_count(self, event=None):
        """Update display after a change in the LED count spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ledcount = self.ledcount.get()
            self._update_preview()
            
    def _convert_K_to_RGB(self, colour_temperature):
        """
        Converts from K to RGB, algorithm courtesy of 
        http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
        """
        #range check
        if colour_temperature < 1000: 
            colour_temperature = 1000
        elif colour_temperature > 40000:
            colour_temperature = 40000
        
        tmp_internal = colour_temperature / 100.0
        
        # red 
        if tmp_internal <= 66:
            red = 255
        else:
            tmp_red = 329.698727446 * math.pow(tmp_internal - 60, -0.1332047592)
            if tmp_red < 0:
                red = 0
            elif tmp_red > 255:
                red = 255
            else:
                red = int(tmp_red)
        
        # green
        if tmp_internal <=66:
            tmp_green = 99.4708025861 * math.log(tmp_internal) - 161.1195681661
            if tmp_green < 0:
                green = 0
            elif tmp_green > 255:
                green = 255
            else:
                green = int(tmp_green)
        else:
            tmp_green = 288.1221695283 * math.pow(tmp_internal - 60, -0.0755148492)
            if tmp_green < 0:
                green = 0
            elif tmp_green > 255:
                green = 255
            else:
                green = int(tmp_green)
        
        # blue
        if tmp_internal >=66:
            blue = 255
        elif tmp_internal <= 19:
            blue = 0
        else:
            tmp_blue = 138.5177312231 * math.log(tmp_internal - 10) - 305.0447927307
            if tmp_blue < 0:
                blue = 0
            elif tmp_blue > 255:
                blue = 255
            else:
                blue = int(tmp_blue)
        
        return red, green, blue

    def _update_ct(self, event=None):
        """Update display after a change in the ct spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ct = self.ct.get()
            
            r,g,b = self._convert_K_to_RGB(ct)
            self.red.set(r)
            self.green.set(g)
            self.blue.set(b)
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            args = (r, g, b)
            hexa = rgb_to_hexa(*args)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h)
            self._update_preview()                        
            
    def led_off(self,_event=None):

        # switch off all LED

        message = "#L00 00 00 00 FF\n"  

        if self.serport:
            self.serport.write(message.encode())
            print("Message send to ARDUINO: ",message)
            cc=str(self.serport.readline())
            print("ARDUINO Feedback:",cc[2:][:-5])                

    def cancel(self,_event=None):

        # switch off all LED

        message = "#L00 00 00 00 FF\n"  

        if self.serport:
            self.serport.write(message.encode())
            print("Message send to ARDUINO: ",message)
            cc=str(self.serport.readline())
            print("ARDUINO Feedback:",cc[2:][:-5])
        
        self.destroy()


def askcolor(color="red", parent=None, title=_("MobaLED Color Tester"), led=0, ledcount=1,serport=None):
    """
    Open a ColorPicker dialog and return the chosen color.

    The selected color is retunred in RGB(A) and hexadecimal #RRGGBB(AA) formats.
    (None, None) is returned if the color selection is cancelled.

    Arguments:
        * color: initially selected color (RGB(A), hexa or tkinter color name)
        * parent: parent window
        * title: dialog title
        * alpha: alpha channel suppport
    """
    col = ColorPicker(parent, color, title,led=led,ledcount=ledcount,serport=serport)
    col.wait_window(col)
    res = col.get_color()
    if res:
        return res[0], res[3], res[4]
    else:
        return None, None
