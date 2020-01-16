# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
#         SerialMonitorPage
#
# * Version: 1.00
# * Author: Harold Linke
# * Date: December 25th, 2019
# * Copyright: Harold Linke 2019
# *
# *
# * MobaLedCheckColors on Github: https://github.com/haroldlinke/MobaLedCheckColors
# *
# *
# * History of Change
# * V1.00 25.12.2019 - Harold Linke - first release
# *
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

import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
    overlay, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv, convert_K_to_RGB
from tkcolorpicker.gradientbar import GradientBar
from tkcolorpicker.colorsquare import ColorSquare
from tkcolorpicker.spinbox import Spinbox
from tkcolorpicker.limitvar import LimitVar

from mlcolorcheck.lightgradientbar import LightGradientBar
from mlcolorcheck.colorwheel import ColorWheel
from mlcolorcheck.DefaultConstants import COLORCOR_MAX, DEFAULT_PALETTE, LARGE_FONT, SMALL_FONT, VERY_LARGE_FONT, VERSION, PERCENT_BRIGHTNESS, BLINKFRQ, ARDUINO_WAITTIME
#from mlcolorcheck.dictFile import saveDicttoFile, readDictFromFile
import mlcolorcheck.dictFile as dictFile
from locale import getdefaultlocale
import re
import time

VERSION ="V01.17 - 25.12.2019"
LARGE_FONT= ("Verdana", 12)
VERY_LARGE_FONT = ("Verdana", 14)
SMALL_FONT= ("Verdana", 8)

PERCENT_BRIGHTNESS = 1  # 1 = Show the brightnes as percent, 0 = Show the brightnes as ">>>"# 03.12.19:

COLORCOR_MAX = 255

# --- Translation - not used
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha", "Configuration": "Einstellungen"
      }

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


class RightClickMenu(tk.Frame):   
    def __init__(self, parent, colorlabel, mainpage):
        self.master = parent
        self.mainpage = mainpage
        tk.Frame.__init__(self, self.master)  
        self.colorlabel = colorlabel
        self.create_widgets()

    def create_widgets(self):
        self.create_right_click_menu()       

    def create_right_click_menu(self):
        self.right_click_menu = tk.Menu(self.master, tearoff=0, relief='sunken')
        self.right_click_menu.add_command(label="aktuelle Farbe in Palette uebernehmen [CRTL-rechte Maustaste]", command=self.set_act_color)
        #self.right_click_menu.add_command(label="Rückgängig [CRTL-z]", command=self.menuUndo)
        #self.right_click_menu.add_command(label="alte Farbe in Palette uebernehmen", command=self.set_old_color)
        self.right_click_menu.add_separator()
        self.right_click_menu.add_command(label="Palettenfarbe zur aktuellen Farbe machen [Linke Maustaste]", command=self.palette_cmd)
        
        self.right_click_menu.add_command(label="Paletten Farbe auf Standard zuruecksetzen", command=self.reset_color)

    def palette_cmd(self):
        mainpage = self.mainpage
        current_color = self.mainpage.hexa.get()
        old_color_disp, brightness_text = self.mainpage.keycolor_to_dispcolor(current_color)
        self.mainpage.old_color_prev.configure(background=old_color_disp)
        self.mainpage.old_color_prev.configure(text="alte Farbe\n"+brightness_text)
        self.mainpage._old_color = hexa_to_rgb(current_color)
        
        label = self.colorlabel
        label.master.focus_set()
        label.master.configure(relief="sunken")
        text = label["text"]
        palettekey,dummy = text.split("\n")
        keycolor = self.mainpage.palette[palettekey]
        r,g,b = hexa_to_rgb(keycolor)
        self.mainpage.ct.set(0)
        args = (r, g, b)
        color = rgb_to_hexa(*args)
        h, s, v = rgb_to_hsv(r, g, b)
        self.mainpage.red.set(r)
        self.mainpage.green.set(g)
        self.mainpage.blue.set(b)
        self.mainpage.hue.set(h)
        self.mainpage.saturation.set(s)
        self.mainpage.value.set(v)
        self.mainpage.hexa.delete(0, "end")
        self.mainpage.hexa.insert(0, color.upper())
        hcor,scor,vcor = self.mainpage._correct_hsv_disp(h,s,v)
        self.mainpage.bar.set(hcor,scor,vcor)
        self.mainpage.square.set_hsv((hcor, scor, vcor))
        self.mainpage._update_preview()        
    
    def popup_text(self, event):
        self.right_click_menu.post(event.x_root, event.y_root)

    def set_act_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage = self.mainpage
        r = mainpage.red.get()
        g = mainpage.green.get()
        b = mainpage.blue.get()
        args = (r,g,b)
        color = rgb_to_hexa(*args)
        old_text = label["text"]
        linebreak=old_text.find("\n")
        key = old_text[:linebreak]
        old_color = mainpage.palette[key]
        
        color_disp,brightness_text = mainpage.keycolor_to_dispcolor(color)
        
        fontcolor = "#000000"
        label["foreground"] = fontcolor
        label["background"] = color_disp

        new_text = key + "\n "+ brightness_text #color
        label["text"] = new_text

        old_color_disp, brigthness_text = mainpage.keycolor_to_dispcolor(old_color)
        
        mainpage.old_color_prev.configure(background=old_color_disp)
        mainpage.old_color_prev.configure(text="alte Farbe\n"+brigthness_text)
        mainpage._old_color = hexa_to_rgb(old_color)
        mainpage.setPaletteColor(key,color)
        #mainpage.palette[key] = color
        mainpage._update_preview()    

    def set_old_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage = self.mainpage
        newcolor = mainpage._old_color
        r = newcolor[0]
        g = newcolor[1]
        b = newcolor[2]
        args = (r,g,b)
        color = rgb_to_hexa(*args)
        old_text = label["text"]
        linebreak=old_text.find("\n")
        key = old_text[:linebreak]
        old_color = mainpage.palette[key]
        
        color_disp,brightness_text = mainpage.keycolor_to_dispcolor(color)
        
        fontcolor = "#000000"
        label["foreground"] = fontcolor
        label["background"] = color_disp

        new_text = key + "\n "+ brightness_text #color
        label["text"] = new_text

        old_color_disp, brigthness_text = mainpage.keycolor_to_dispcolor(old_color)
        
        mainpage.old_color_prev.configure(background=old_color_disp)
        mainpage.old_color_prev.configure(text="alte Farbe\n"+brigthness_text)
        mainpage._old_color = hexa_to_rgb(old_color)
        mainpage.setPaletteColor(key,color)
        #mainpage.palette[key] = color
        mainpage._update_preview()    


    def reset_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage = self.mainpage
        old_text = label["text"]
        linebreak=old_text.find("\n")
        key = old_text[:linebreak]
        std_color = DEFAULT_PALETTE[key]
        color = std_color
        old_color = mainpage.palette[key]
        
        color_disp,brightness_text = mainpage.keycolor_to_dispcolor(color)
        
        fontcolor = "#000000"
        label["foreground"] = fontcolor
        label["background"] = color_disp

        new_text = key + "\n "+ brightness_text #color
        label["text"] = new_text

        old_color_disp, brigthness_text = mainpage.keycolor_to_dispcolor(old_color)
        
        mainpage.old_color_prev.configure(background=old_color_disp)
        mainpage.old_color_prev.configure(text="alte Farbe\n"+brigthness_text)
        mainpage._old_color = hexa_to_rgb(old_color)
        mainpage.palette[key] = color
        mainpage._update_preview()    

    def menuUndo(self):
        mainpage = self.mainpage
        mainpage.MenuUndo()
        
# ----------------------------------------------------------------
# Class ColorCheckPage
# ----------------------------------------------------------------

class ColorCheckPage(tk.Frame):

    # ----------------------------------------------------------------
    # ColorCheckPage __init__
    # ----------------------------------------------------------------

    def __init__(self, parent, controller):
        
        tk.Frame.__init__(self, parent)
        self.tabname = _("LED Farbtest")
        self.tabClassName = "ColorCheckPage"
        self.controller = controller
        self.parent = parent
        self.ledhighlight = False
        self.on_lednum = 0
        self.on_ledcount = 1
        self.on_ledon = False 
        self.paletteUndoList = []
        self.paletteRedoList = []        
        color = self.getConfigData("old_color")
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

        led = self.getConfigData("lastLed")

        ledcount = self.getConfigData("lastLedCount")

        serport = controller.arduino

        self.cor_red = self.getConfigData("led_correction_r")
        self.cor_green = self.getConfigData("led_correction_g")
        self.cor_blue = self.getConfigData("led_correction_b")

        title=_("MobaLedLib LED Farbentester " + VERSION)

        self.main_frame = ttk.Frame(self)
        self.main_frame.columnconfigure(1, weight=1)
        self.main_frame.rowconfigure(1, weight=1)
        
        # --- Colorwheel or Colorsquare ?
        if self.getConfigData("colorview") == 0:
            # --- GradientBar
            hue = col2hue(*self._old_color)
            bar = ttk.Frame(self, borderwidth=2, relief='groove')
            self.bar = GradientBar(bar, hue=hue, cr = int(self.cor_red), 
                                      cg=int(self.cor_green), 
                                      cb=int(self.cor_blue), width=200, highlightthickness=0)
            self.bar.pack()

            # --- ColorSquare
            square = ttk.Frame(self, borderwidth=2, relief='groove')
            self.square = ColorSquare(square, hue=hue, width=200, height=200,
                                      color=rgb_to_hsv(*self._old_color), 
                                      cr = int(self.cor_red), 
                                      cg=int(self.cor_green), 
                                      cb=int(self.cor_blue),
                                      highlightthickness=0)
            self.square.pack()

            
        else:
            # --- LightGradientBar
            hue = 0
            h,s,v=rgb_to_hsv(*self._old_color)
            bar = ttk.Frame(self, borderwidth=2, relief='groove')
            self.bar = LightGradientBar(bar, hue=h, value=v, width=200, highlightthickness=0)
            self.bar.pack()
            self.controller.ToolTip(self.bar, text='Helligkeit einstellen durch Mausklick', key="Lightbar",button_1=True)
        
            # --- ColorWheel
            square = ttk.Frame(self, borderwidth=2, relief='groove')
            self.square = ColorWheel(square, hue=hue, width=200, height=200,
                                      color=rgb_to_hsv(*self._old_color),
                                      highlightthickness=0, cr = int(self.cor_red), cg=int(self.cor_green), cb=int(self.cor_blue))
            self.square.pack()
            
            self.controller.ToolTip(self.square, key="ColorWheel",button_1=True)
            
        # --- color preview:  currently selected color 
        preview_frame = ttk.Frame(self.main_frame, relief="raised", borderwidth=1)
        preview_frame.grid(row=0, column=0, sticky="nw", pady=2)
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(old_color[:7])
        self.old_color_prev = tk.Label(preview_frame, background=old_color_disp,
                                  width=12, highlightthickness=0, height=2,
                                  padx=2, pady=2,text=_("alte Farbe")+"\n"+brightness_text)
        self.color_preview = tk.Label(preview_frame, width=12, height=2,
                                      pady=2, background=old_color_disp,
                                      padx=2, highlightthickness=0, text=_("aktuelle Farbe")+"\n"+brightness_text)
        #self.old_color_prev.bind("<1>", self._reset_preview)
        #self.old_color_prev.grid(row=0, column=0)
        self.color_preview.grid(row=1, column=0)
        self.color_preview.bind("<1>", self._update_preview)
        
        self.controller.ToolTip(self.old_color_prev,key="Vorherige Farbe")
        self.controller.ToolTip(self.color_preview, key="Aktuell eingestellte Farbe")
        

        # --- palette
        self.palette = self.getConfigData("palette")
        self.palette_frame = ttk.Frame(self.main_frame)
        self.palette_frame.grid(row=0, column=1, rowspan=2, sticky="ne")
        for i, key in enumerate(self.palette):
            keycolor = self.palette[key]
            color_disp,brightness_text = self.keycolor_to_dispcolor(keycolor)
            fontcolor = "#000000"
            text = key + "\n" + brightness_text
            f = ttk.Frame(self.palette_frame, borderwidth=1, relief="raised",
                          style="palette.TFrame")
            l = tk.Label(f, background=color_disp, width=12, height=2,text=text,fg=fontcolor)
            self.bind_right_click_menu_to_palettelabel(l)
            l.bind("<Button-1>", self._palette_cmd)
            l.bind("<Control-3>", self._palette_save_cmd)            
            self.controller.ToolTip(l, key=key)
             
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)

        # --- hsv

        col_frame = ttk.Frame(self)

        hsv_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        hsv_frame.pack(pady=(0, 4), fill="x")
        hsv_frame.columnconfigure(0, weight=1)
        self.hue = LimitVar(0, 360, self,turnaround=True)
        self.saturation = LimitVar(0, 100, self)
        self.value = LimitVar(0, 100, self)

        self.s_h = Spinbox(hsv_frame, from_=-1, to=361, width=5, name='spinbox',
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
        label_h= ttk.Label(hsv_frame, text=_('Hue'))
        label_h.grid(row=0, column=0, sticky='e', padx=4, pady=4)
        label_s = ttk.Label(hsv_frame, text=_('Saturation'))
        label_s.grid(row=1, column=0, sticky='e', padx=4, pady=4)
        label_v = ttk.Label(hsv_frame, text=_('Helligkeit'))
        label_v.grid(row=2, column=0, sticky='e', padx=4, pady=4)
        self.controller.ToolTip(self.s_h, text="Hue (Farbton) 0..360 [Ctrl-h/Alt-h]")
        self.controller.ToolTip(self.s_s, text="Saturation (Sättigung) 0..100 [Ctrl-s/Alt-s]")
        self.controller.ToolTip(self.s_v, text="Luminosity (Helligkeit) 0..100 [Ctrl-l/Alt-l]")
         
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
        ttk.Label(rgb_frame, text=_('Rot')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün')).grid(row=1, column=0, sticky='e',
                                                   padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Blau')).grid(row=2, column=0, sticky='e',
                                                  padx=4, pady=4)
        self.controller.ToolTip(self.s_red, text="Rot oder LED 1 0..255 [Ctrl-r/Alt-r]")
        self.controller.ToolTip(self.s_green, text="Grün oder LED2 0..255 [Ctrl-g/Alt-g]")
        self.controller.ToolTip(self.s_blue, text="Blau oder LED3 0..255 [Ctrl-b/Alt-b]")

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

        ttk.Label(ct_frame, text=_('Farbtemperature')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        self.controller.ToolTip(self.s_ct, text="Farbtemperature in Kelvin [Ctrl-t/Alt-t]\nnur Eingabe\nwird in RGB umgerechnet")

        # --- hexa
        hexa_frame = ttk.Frame(col_frame)
        hexa_frame.pack(fill="x")
        self.hexa = ttk.Entry(hexa_frame, justify="center", width=10, name='entry')
        self.hexa.insert(0, old_color.upper())
        ttk.Label(hexa_frame, text="HEXA").pack(side="left", padx=4, pady=(4, 1))
        self.hexa.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)
        self.controller.ToolTip(self.hexa, text="RGB in Hexadezimal")


        # --- ARDUINO Steuerung

        arduino_frame = ttk.Frame(self,relief="ridge", borderwidth=2,width=500)
        #self.arduino_status = tk.Label(arduino_frame, text=_('Nicht verbunden'), fg="black",width=20)
        #self.arduino_status.grid(row=0, column=2, sticky='', padx=4, pady=(10, 4))
        #self.controller.ToolTip(self.arduino_status, text="Zeigt den Status der Verbindung zum ARDUINO an: \nVerbunden - eine Verbidnug zum ARDUINO steht\nNicht verbunden - keine Verbindung zum ARDUINO")

        # --- ARDUINO LED
        led_frame = ttk.Frame(arduino_frame, relief="ridge", borderwidth=2)
        led_frame.columnconfigure(0, weight=1)

        self.lednum = LimitVar(0, 255, self)
        self.ledcount = LimitVar(1, 256, self)

        self.lednum.set(self.getConfigData("lastLed"))
        self.ledcount.set(self.getConfigData("lastLedCount"))

        self.s_led = Spinbox(led_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.lednum, command=self._update_led_num)
        self.s_ledcount = Spinbox(led_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledcount, command=self._update_led_count)
        self.s_led.delete(0, 'end')
        self.s_led.insert(0, led)
        self.s_led.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_ledcount.delete(0, 'end')
        self.s_ledcount.insert(1, ledcount)
        self.s_ledcount.grid(row=0, column=3, sticky='e', padx=4, pady=4)

        ttk.Label(led_frame, text=_('LED Adresse')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(led_frame, text=_('Anzahl')).grid(row=0, column=2, sticky='e',
                                                 padx=4, pady=4)

        led_frame.grid(row=0,column=0,columnspan=2,padx=10,pady=(10, 4),sticky="w")
        self.controller.ToolTip(led_frame, text="Adresse der LED, deren Farbe getested werden soll \n[Ctrl-Right/Left] \nAnzahl der LED, die dieselbe Farbe haben sollen, angeben\n[Ctrl-Up/Down] ")
        
        ledoff_button=ttk.Button(arduino_frame, text="Alle LED aus",width=20, command=self.led_off)
        ledoff_button.grid(row=0, column=5, padx=10, pady=(10, 4), sticky='w')
        self.controller.ToolTip(ledoff_button, text="Alle LED ausschalten und LED Liste löschen\n[CTRL-o]")

        blinkoff_button=ttk.Button(arduino_frame, text="Blinken Ein/Aus",width=20, command=self.blinking_on_off)
        blinkoff_button.grid(row=0, column=6, padx=10, pady=(10, 4), sticky='w')
        self.controller.ToolTip(blinkoff_button, text="Schaltet das Blinken der LED an/aus")

        # --- placement
        bar.grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        square.grid(row=1, column=0, padx=10, pady=(9, 0), sticky='n')
        col_frame.grid(row=0, rowspan=3, column=1, padx=(4, 10), pady=(10, 4), sticky="w")
        self.main_frame.grid(row=3, column=0, columnspan=3, pady=(4, 10), padx=10, sticky="nsew")
        arduino_frame.grid(row=4, column=0, columnspan=3,pady=(0, 10), padx=10)
#        button_frame.grid(row=5, column=0, columnspan=2,pady=(0, 10), padx=10)

        # --- bindings
        self.bar.bind("<ButtonRelease-1>", self._change_color, True)
        self.bar.bind("<B1-Motion>", self._change_color, True)
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
        #self.s_led.bind("<FocusOut>", self._update_led_num)
        self.s_led.bind("<Return>", self._update_led_num)
        self.s_led.bind("<Control-a>", self._select_all_entry)
        #self.s_ledcount.bind("<FocusOut>", self._update_led_count)
        self.s_ledcount.bind("<Return>", self._update_led_count)
        self.s_ledcount.bind("<Control-a>", self._select_all_entry)        
        
        self.controller.bind("<Control-h>",self.s_h.invoke_buttonup)
        self.controller.bind("<Alt-h>"    ,self.s_h.invoke_buttondown)
        self.controller.bind("<Control-s>",self.s_s.invoke_buttonup)
        self.controller.bind("<Alt-s>",self.s_s.invoke_buttondown)
        self.controller.bind("<Control-l>",self.s_v.invoke_buttonup)
        self.controller.bind("<Alt-l>",self.s_v.invoke_buttondown)
        self.controller.bind("<Control-r>",self.s_red.invoke_buttonup)
        self.controller.bind("<Alt-r>",self.s_red.invoke_buttondown)
        self.controller.bind("<Control-g>",self.s_green.invoke_buttonup)
        self.controller.bind("<Alt-g>",self.s_green.invoke_buttondown)
        self.controller.bind("<Control-b>",self.s_blue.invoke_buttonup)
        self.controller.bind("<Alt-b>",self.s_blue.invoke_buttondown)
        self.controller.bind("<Control-Right>",self.s_led.invoke_buttonup)
        self.controller.bind("<Control-Left>",self.s_led.invoke_buttondown)
        self.controller.bind("<Control-Up>",self.s_ledcount.invoke_buttonup)
        self.controller.bind("<Control-Down>",self.s_ledcount.invoke_buttondown)
        self.controller.bind("<Control-t>",self.s_ct.invoke_buttonup)
        self.controller.bind("<Alt-t>",self.s_ct.invoke_buttondown)
        self.controller.bind("<F1>",self.controller.call_helppage)
        self.controller.bind("<Alt-F4>",self.cancel)
        self.controller.bind("<Control-o>",self.led_off)
        self.controller.bind("<Control-z>",self.controller.MenuUndo)
        self.controller.bind("<Control-y>",self.controller.MenuRedo)

        self._update_color_hsv(_LEDUpdate=False)
 
    def cancel(self,_event=None):
        try:
            self.led_off()
        except:
            pass

        self.setConfigData("lastLed"     , self.lednum.get())
        self.setConfigData("lastLedCount", self.ledcount.get())
        self.setConfigData("old_color"   , self.hexa.get())
        self.setConfigData("palette"    , self.palette.copy())
        self.setParamData("color"    , self.hexa.get()) # current color
        self.setParamData("Lednum"   , self.lednum.get())
        self.setParamData("LedCount" , self.ledcount.get())
        self.setParamData("coltab"   , self.palette.copy())
        self.continueCheckDisconnectFile = False # stop thread onCheckDisconnectFile
        
    def tabselected(self):
        if self.controller.currentTabClass == "ConfigurationPage":
            self._update_cor_rgb()
            
        
        self.controller.currentTabClass = self.tabClassName
        
        
        print(self.tabname)
        pass        

    def getConfigData(self, key):
        return self.controller.getConfigData(key)
    
    def readConfigData(self):
        self.controller.readConfigData()
        
    def setConfigData(self,key, value):
        self.controller.setConfigData(key, value)

    def setParamData(self,key, value):
        self.controller.setParamData(key, value)

    def connect(self):
        pass

    def disconnect(self):
        pass
            
    def MenuUndo(self,_event=None):
        if self.paletteUndoList:
            undoEntry = self.paletteUndoList.pop()
            self.palette.update(undoEntry)
            self._palette_redraw_colors()
        pass
    
    def MenuRedo(self,_event=None):
        pass

    def bind_right_click_menu_to_palettelabel(self, palettelabel):
        self.popup = RightClickMenu(self.master, palettelabel,self)        
        palettelabel.bind("<Button-3>", self.popup.popup_text)

    def cb(self):
        self._update_cor_rgb()

    def get_color(self):
        """Return selected color, return an empty string if no color is selected."""
        return self.color


    def _correct_rgb_disp(self, r, g, b):
        crf = COLORCOR_MAX/int(self.cor_red)
        cgf = COLORCOR_MAX/int(self.cor_green)
        cbf = COLORCOR_MAX/int(self.cor_blue)
        rcor = int(r*crf)
        gcor = int(g*cgf)
        bcor = int(b*cbf)
        if rcor > 255: rcor = 255
        if gcor > 255: gcor = 255
        if bcor > 255: bcor = 255
        return rcor, gcor, bcor

    def _correct_hsv_disp(self, h,s,v):
        if self.getConfigData("colorview") == 0: # only correct when colorsquare is shown
            r, g, b = hsv_to_rgb(h,s,v)
            rcor,gcor,bcor = self._correct_rgb_disp(r,g,b)
            args = (rcor,gcor, bcor)
            h,s,v = rgb_to_hsv(*args)
        return h,s,v

    def keycolor_to_dispcolor(self, keycolor):
        r,g,b = hexa_to_rgb(keycolor)
        args = (r,g,b)
        h,s,v = rgb_to_hsv(*args)
        r2,g2,b2 = self._correct_rgb_disp(*args)
        #---  palette colors will be displayed with brightness = 100
        args2 = (r2,g2,b2)
        h2,s2,v2 = rgb_to_hsv(*args2)
        r3,g3,b3 = hsv_to_rgb(h2,s2,100)
        args3 = (r3,g3,b3)
        disp_color = rgb_to_hexa(*args3)
        if PERCENT_BRIGHTNESS:                                           # 03.12.19:
            brightness = str(v) + " %"
        else:
            brightness = ""
            v3 = v/10
            for j in range(10):
                if j<v3:
                    brightness = brightness + ">"
        return disp_color, brightness


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

    def _update_preview(self, _event=None,_LEDupdate=True):
        """Update color preview."""
        red=self.red.get()
        green= self.green.get()
        blue = self.blue.get()
        args = (red,green,blue)
        color = rgb_to_hexa(*args)
        color_disp,brightness_text = self.keycolor_to_dispcolor(color)
        self.color_preview.configure(background=color_disp)
        self.color_preview.configure(text="aktuelle Farbe\n"+brightness_text)
        if _LEDupdate:
            ledcount = self.ledcount.get()
            if ledcount >0:
                lednum = self.lednum.get()
                self._update_led(lednum, ledcount, red, green, blue, color)

    def _reset_preview(self, event):
        """Respond to user click on old_color item."""
        current_color = self.hexa.get()
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
        hcor,scor,vcor = self._correct_hsv_disp(h,s,v)
        self.bar.set(hcor, scor, vcor)
        self.square.set_hsv((hcor, scor, vcor))
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(current_color)
        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brightness_text)
        self._old_color = hexa_to_rgb(current_color)
        self._update_preview()

    def _palette_cmd(self, _event):
        """Respond to user click on a palette item."""
        
        # set old_color = current color
        current_color = self.hexa.get()
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(current_color)
        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brightness_text)
        self._old_color = hexa_to_rgb(current_color)
        
        label = _event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        text = _event.widget["text"]
        palettekey,dummy = text.split("\n")
        keycolor = self.palette[palettekey]
        r,g,b = hexa_to_rgb(keycolor)
        self.ct.set(0)
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
        hcor,scor,vcor = self._correct_hsv_disp(h,s,v)
        self.bar.set(hcor,scor,vcor)
        self.square.set_hsv((hcor, scor, vcor))
        self._update_preview()

    def _palette_save_cmd(self, event):
        """Respond to user ctrl right click on a palette item."""

        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        r=self.red.get()
        g= self.green.get()
        b = self.blue.get()
        args = (r,g,b)
        color = rgb_to_hexa(*args)
        old_text = event.widget["text"]
        linebreak=old_text.find("\n")
        key = old_text[:linebreak]
        old_color = self.palette[key]
        
        color_disp,brightness_text = self.keycolor_to_dispcolor(color)
        
        fontcolor = "#000000"
        event.widget["foreground"] = fontcolor
        event.widget["background"] = color_disp

        new_text = key + "\n "+ brightness_text #color
        event.widget["text"] = new_text

        old_color_disp, brigthness_text = self.keycolor_to_dispcolor(old_color)
        
        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brigthness_text)
        self._old_color = hexa_to_rgb(old_color)
        self.setPaletteColor(key,color)
        self._update_preview()
        
    def setPaletteColor(self, key, color):
        oldcolor= self.palette.get(key,"#000000")
        self.controller.setParamDataChanged()
        self.addtoUndoList(key,oldcolor)
        self.palette[key] = color

    def addtoUndoList(self, key, color):
        undoEntry = {}
        undoEntry[key] = color
        self.paletteUndoList.append(undoEntry)

    def _palette_redraw_colors(self):
        
        self.palette_frame.destroy()
        self.palette_frame = ttk.Frame(self.main_frame)
        self.palette_frame.grid(row=0, column=1, rowspan=2, sticky="ne")
        for i, key in enumerate(self.palette):
            keycolor = self.palette[key]
            color_disp,brightness_text = self.keycolor_to_dispcolor(keycolor)
            fontcolor = "#000000"
            text = key + "\n" + brightness_text
            f = ttk.Frame(self.palette_frame, borderwidth=1, relief="raised",
                          style="palette.TFrame")
            l = tk.Label(f, background=color_disp, width=12, height=2,text=text,fg=fontcolor)
            l.bind("<Button-1>", self._palette_cmd)
            l.bind("<Control-3>", self._palette_save_cmd)         
            
            self.bind_right_click_menu_to_palettelabel(l)
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)        
        
    def palette_reset_colors(self):
        
        self.palette = DEFAULT_PALETTE.copy()
        self._palette_redraw_colors()
        
    def savePalettetoFile(self, filepath):
        
        dictFile.saveDicttoFile(filepath, self.palette)

    def readPalettefromFile(self, filepath):
        
        palette = dictFile.readDictFromFile(filepath)
        if palette:
            self.palette = palette.copy()
        self._palette_redraw_colors()
    

    def _change_sel_color(self, event):
        """Respond to motion of the color selection cross."""
        (r, g, b), (h, s, v), color = self.square.get()
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.saturation.set(s)
        self.hue.set(h)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self._update_preview()

    def _change_color(self, event):
        """Respond to motion of the hsv/brightness cursor."""
        hv = self.bar.get()
        self.square.set_hue(hv)
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
            self.bar.set(h,s,v)
            self.square.set_hsv((h, s, v))
        else:
            self._update_color_rgb()
        self._update_preview()

    def _update_color_hsv(self, event=None, _LEDUpdate=True):
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
            self.bar.set(h,s,v)
            self._update_preview(_LEDupdate=_LEDUpdate)

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
            self.bar.set(h,s,v)
            self._update_preview()

    def _update_cor_rgb(self, event=None):
        """Update display after a change in the RGB spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            if ((self.cor_red != self.getConfigData("led_correction_r")) or
               (self.cor_green != self.getConfigData("led_correction_g")) or
               (self.cor_blue != self.getConfigData("led_correction_b"))):
                self.cor_red = self.getConfigData("led_correction_r")
                self.cor_green = self.getConfigData("led_correction_g")
                self.cor_blue = self.getConfigData("led_correction_b")               
                self._update_preview()
                self.square.set_colorcorrection(self.cor_red,self.cor_green,self.cor_blue)
                self._palette_redraw_colors()

    def _update_led_num(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            led = int(self.lednum.get())
            ledcount = int(self.ledcount.get())
            ledcnt_max = int(self.controller.get_maxLEDcnt())
            if led+ledcount > ledcnt_max:
                led = ledcnt_max-ledcount
                self.lednum.set(led)
            self._highlight_led(led, self.ledcount.get())

    def _update_led_count(self, event=None):
        """Update display after a change in the LED count spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            led = int(self.lednum.get())
            ledcount = int(self.ledcount.get())
            ledcnt_max = int(self.controller.get_maxLEDcnt())
            if led+ledcount > ledcnt_max:
                ledcount = ledcnt_max-led
                self.ledcount.set(ledcount)            
            self._highlight_led(self.lednum.get(), ledcount)
            
    def _update_ct(self, event=None):
        """Update display after a change in the ct spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ct = self.ct.get()
            r,g,b = convert_K_to_RGB(ct)
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
            self.bar.set(h,s,v)
            self._update_preview()

    def led_off(self,_event=None):
    # switch off all LED
        self.ledhighlight = False
        message = "#L00 00 00 00 FF\n"
        self.controller.send_to_ARDUINO(message)
        self.controller.ledtable.clear()
        
    def blinking_on_off(self,_event=None):
    # switch off all LED
        if self.ledhighlight:
            self._restore_led_colors(self.on_lednum,self.on_ledcount)
        else:
            lednum = self.lednum.get()
            ledcount = self.ledcount.get()
            self._highlight_led(lednum, ledcount)
        
    def _update_led(self, lednum, ledcount, red, green, blue, color_hex):
        self._update_ledtable(lednum, ledcount, color_hex)
        message = "#L" + '{:02x}'.format(lednum) + " " + '{:02x}'.format(red) + " " + '{:02x}'.format(green) + " " + '{:02x}'.format(blue) + " " + '{:02x}'.format(ledcount) + "\n"
        self.controller.send_to_ARDUINO(message)
        
    def _update_ledtable(self, lednum, ledcount, rgb_hex):
        for i in range(ledcount):
            lednum_str = '{:03}'.format(lednum+i)
            self.controller.ledtable[lednum_str] = rgb_hex
        self._restore_led_colors(lednum,ledcount)
    
    def _restore_led_colors(self, lednum, ledcount):
        if self.ledhighlight: # reset all colors
            self.ledhighlight = False
            for i in range(ledcount):
                lednum_str = '{:03}'.format(self.on_lednum+i)
                self._send_ledcolor_to_ARDUINO(lednum_str,1,self.controller.ledtable.get(lednum_str,"#000000"))
                time.sleep(ARDUINO_WAITTIME)    
                
    def _get_color_from_ledtable(self,lednum):
        lednum_str = '{:03}'.format(lednum)
        return self.controller.ledtable.get(lednum_str,"#000000")
        
    def _send_ledcolor_to_ARDUINO(self, lednum, ledcount, ledcolor):
        lednum_int = int(lednum)
        message = "#L" + '{:02x}'.format(lednum_int) + " " + ledcolor[1:3] + " " + ledcolor[3:5] + " " + ledcolor[5:7] + " " + '{:02x}'.format(ledcount) + "\n"
        self.controller.send_to_ARDUINO(message)
        time.sleep(ARDUINO_WAITTIME)
            
    def _highlight_led(self,lednum, ledcount):
        if self.ledhighlight: # onblink is already running, change only lednum and ledcount
            # reset all blinking led with their colors
            for i in range(self.on_ledcount):
                lednum_str = '{:03}'.format(self.on_lednum+i)
                self._send_ledcolor_to_ARDUINO(lednum_str,1,self.controller.ledtable.get(lednum_str,"#000000"))
                time.sleep(ARDUINO_WAITTIME)
            #set the blinking led to highlight
            self._send_ledcolor_to_ARDUINO(lednum, ledcount, "#FFFFFF")
            # save current lednum and led count
            self.on_lednum = lednum
            self.on_ledcount = ledcount
            self.on_ledon = True
        else:
            self.ledhighlight = True
            self.on_lednum = lednum
            self.on_ledcount = ledcount
            self.on_ledon = True       
            self.onblink_led()

         
    def onblink_led(self):
        if self.ledhighlight:
            if self.on_ledon:
                self._send_ledcolor_to_ARDUINO(self.on_lednum, self.on_ledcount, "#FFFFFF")
                self.on_ledon = False
                self.after(int(500/BLINKFRQ),self.onblink_led)
            else:
                self._send_ledcolor_to_ARDUINO(self.on_lednum, self.on_ledcount, "#000000")
                self.on_ledon = True
                self.after(int(500/BLINKFRQ),self.onblink_led)



