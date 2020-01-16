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



        
# ----------------------------------------------------------------
# Class SoundCheckPage
# ----------------------------------------------------------------

class SoundCheckPage(tk.Frame):

    # ----------------------------------------------------------------
    # ColorCheckPage __init__
    # ----------------------------------------------------------------

    def __init__(self, parent, controller):
        
        tk.Frame.__init__(self, parent)
        self.tabname = _("Sound Test")
        self.tabClassName = "SoundCheckPage"
        self.controller = controller
        self.parent = parent
        
        self.on_lednum = 0
        
        self.on_ledon = False 

        led = self.getConfigData("lastLed")

        serport = controller.arduino

        title=_("MobaLedLib LED Farbentester " + VERSION)

        self.main_frame = ttk.Frame(self)
        self.main_frame.columnconfigure(1, weight=1)
        self.main_frame.rowconfigure(1, weight=1)

        # --- rgb
        
        col_frame = ttk.Frame(self)
        
        rgb_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        rgb_frame.pack(pady=4, fill="x")
        rgb_frame.columnconfigure(0, weight=1)
        self.red = LimitVar(0, 255, self)
        self.green = LimitVar(0, 255, self)
        self.blue = LimitVar(0, 255, self)

        self.s_red = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.red, command=self._update_color_red)
        self.s_green = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                          textvariable=self.green, command=self._update_color_green)
        self.s_blue = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                         textvariable=self.blue, command=self._update_color_blue)
        
        self.s_red.delete(0, 'end')
        self.s_red.insert(0, 0)
        self.s_green.delete(0, 'end')
        self.s_green.insert(0, 0)
        self.s_blue.delete(0, 'end')
        self.s_blue.insert(0, 0)
        self.s_red.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_green.grid(row=1, column=1, sticky='e', padx=4, pady=4)
        self.s_blue.grid(row=2, column=1, sticky='e', padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Rot')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün')).grid(row=1, column=0, sticky='e',
                                                   padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Blau')).grid(row=2, column=0, sticky='e',
                                                  padx=4, pady=4)
        self.controller.ToolTip(self.s_red, text="Sound Controller 1 0..255 [Ctrl-r/Alt-r]")
        self.controller.ToolTip(self.s_green, text="Sound Controller 2 0..255 [Ctrl-g/Alt-g]")
        self.controller.ToolTip(self.s_blue, text="Sound Controller 3 0..255 [Ctrl-b/Alt-b]")


        arduino_frame = ttk.Frame(self,relief="ridge", borderwidth=2,width=500)

        # --- ARDUINO LED
        led_frame = ttk.Frame(arduino_frame, relief="ridge", borderwidth=2)
        led_frame.columnconfigure(0, weight=1)

        self.lednum = LimitVar(0, 255, self)
        self.ledcount = LimitVar(100, 500, self)

        self.lednum.set(self.getConfigData("lastLed"))
        self.ledcount.set(self.getConfigData("lastSoundImpuls"))

        self.s_led = Spinbox(led_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.lednum, command=self._update_led_num)
        self.s_ledcount = Spinbox(led_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledcount, command=self._update_led_count)
        self.s_led.delete(0, 'end')
        self.s_led.insert(0, led)
        self.s_led.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_ledcount.delete(0, 'end')
        self.s_ledcount.insert(1, 200)
        self.s_ledcount.grid(row=0, column=3, sticky='e', padx=4, pady=4)

        ttk.Label(led_frame, text=_('Soundmodul Adresse')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(led_frame, text=_('Impulslänge')).grid(row=0, column=2, sticky='e',
                                                 padx=4, pady=4)

        led_frame.grid(row=0,column=0,columnspan=2,padx=10,pady=(10, 4),sticky="w")
        self.controller.ToolTip(led_frame, text="Adresse des Soundmoduls, dessen Sound getested werden soll \n[Ctrl-Right/Left] \nImpulslänge\n[Ctrl-Up/Down] ")
        
        # --- placement
        col_frame.grid(row=0, rowspan=3, column=1, padx=(4, 10), pady=(10, 4), sticky="w")
        self.main_frame.grid(row=3, column=0, columnspan=3, pady=(4, 10), padx=10, sticky="nsew")
        arduino_frame.grid(row=4, column=0, columnspan=3,pady=(0, 10), padx=10)
#        button_frame.grid(row=5, column=0, columnspan=2,pady=(0, 10), padx=10)

        # --- bindings
        self.s_red.bind('<FocusOut>', self._update_color_red)
        self.s_green.bind('<FocusOut>', self._update_color_green)
        self.s_blue.bind('<FocusOut>', self._update_color_blue)
        self.s_red.bind('<Return>', self._update_color_red)
        self.s_green.bind('<Return>', self._update_color_green)
        self.s_blue.bind('<Return>', self._update_color_blue)
        self.s_red.bind('<Control-a>', self._select_all_spinbox)
        self.s_green.bind('<Control-a>', self._select_all_spinbox)
        self.s_blue.bind('<Control-a>', self._select_all_spinbox)
        self.s_led.bind('<Control-a>', self._select_all_spinbox)
        self.s_ledcount.bind('<Control-a>', self._select_all_spinbox)

        self.s_led.bind("<Return>", self._update_led_num)
        self.s_led.bind("<Control-a>", self._select_all_entry)
        #self.s_ledcount.bind("<FocusOut>", self._update_led_count)
        self.s_ledcount.bind("<Return>", self._update_led_count)
        self.s_ledcount.bind("<Control-a>", self._select_all_entry)        
        

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
        self.controller.bind("<F1>",self.controller.call_helppage)
        self.controller.bind("<Alt-F4>",self.cancel)
        self.controller.bind("<Control-z>",self.controller.MenuUndo)
        self.controller.bind("<Control-y>",self.controller.MenuRedo)

         
    def cancel(self,_event=None):
        try:
            self.led_off()
        except:
            pass

        self.setConfigData("lastSound"     , self.lednum.get())
        self.setConfigData("lastSoundImpuls", self.ledcount.get())
        self.continueCheckDisconnectFile = False # stop thread onCheckDisconnectFile
        
    def tabselected(self):
        self.controller.currentTabClass = self.tabClassName

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
        pass
    
    def MenuRedo(self,_event=None):
        pass

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

    def _update_color_red(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self._send_sound_to_ARDUINO(self.red.get(), 0, 0)
            
    def _update_color_green(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self._send_sound_to_ARDUINO(0, self.green.get(), 0)
            
    def _update_color_blue(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self._send_sound_to_ARDUINO(0, 0, self.blue.get())

    def _update_led_num(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self._send_sound_to_ARDUINO(self.red.get(), self.green.get(), self.blue.get())

    def _update_led_count(self, event=None):
        """Update display after a change in the LED count spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self._send_sound_to_ARDUINO(self.red.get(), self.green.get(), self.blue.get())
          
    def _send_sound_to_ARDUINO(self, red, green, blue):
        soundmodul = '{:02x}'.format(self.lednum.get())
        sound1 = '{:02x}'.format(red)
        sound2 = '{:02x}'.format(green)
        sound3 = '{:02x}'.format(blue)
        soundImpulsLength = self.ledcount.get()
        message = "#L" + soundmodul + " " + sound1 + " " + sound2 + " " + sound3 + " " + '{:02x}'.format(1) + "\n"
        self.controller.send_to_ARDUINO(message)
        self.after(soundImpulsLength,self.onstopImpuls)
        
            
    def onstopImpuls(self):
        soundmodul = '{:02x}'.format(self.lednum.get())
        message = "#L" + soundmodul + " 00 00 00 01" + "\n"
        self.controller.send_to_ARDUINO(message)        



