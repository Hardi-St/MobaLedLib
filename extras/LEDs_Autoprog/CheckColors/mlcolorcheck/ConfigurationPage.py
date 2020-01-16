# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
#         ConfigPage
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
from tkinter import ttk,messagebox

from tkcolorpicker.spinbox import Spinbox
from tkcolorpicker.limitvar import LimitVar

from locale import getdefaultlocale

from mlcolorcheck.DefaultConstants import COLORCOR_MAX, DEFAULT_PALETTE, LARGE_FONT, SMALL_FONT, VERY_LARGE_FONT, VERSION


# --- Translation - not used
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha", "Configuration": "Einstellungen"}

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
# Class ConfigurationPage
# ----------------------------------------------------------------

class ConfigurationPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        self.tabname = "Einstellungen"
        self.tabClassName = "ConfigurationPage"
        self.controller = controller

        title_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        title_frame.columnconfigure(0, weight=1)

        label = ttk.Label(title_frame, text=_("Einstellungen"), font=LARGE_FONT)
        label.pack(padx=50,pady=(10,10))


        cfg_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        cfg_frame.columnconfigure(0, weight=1)

        label2text = tk.Label(cfg_frame, text="ARDUINO Baudrate / Port:")
        label2text.pack(side="left", padx=10, pady=(10,10))

        self.combo = ttk.Combobox(cfg_frame)
        self.combo["value"] = ("NO DEVICE", "COM1",  "COM2",  "COM3",  "COM4",  "COM5",  "COM6",  "COM7",  "COM8",  "COM9",  "COM10",
                                            "COM11", "COM12", "COM13", "COM14", "COM15", "COM16", "COM17", "COM18", "COM19", "COM20",
                                            "COM21", "COM22", "COM23", "COM24", "COM25", "COM26", "COM27", "COM28", "COM29", "COM30",
                                            "COM31", "COM32", "COM33", "COM34", "COM35", "COM36", "COM37", "COM38", "COM39", "COM40")   # 04.12.19: Added com ports 7..

        self.combo.current(self.getConfigData("serportnumber")) #set the selected item

        self.combo.pack(side="right", padx=10, pady=(10,10) )

        self.comboBd = ttk.Combobox(cfg_frame)
        #self.comboBd["value"] = (50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
        self.comboBd["value"] = (115200)


        self.comboBd.set(self.getConfigData("baudrate")) #set the baudrate

        #self.comboBd.pack(side="right", padx=10, pady=(10,10) )

        view_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        view_frame.columnconfigure(0, weight=1)

        label3text = tk.Label(view_frame, text="Farbauswahlansicht (Neustart notwendig):")
        label3text.pack(side="left", padx=10, pady=(10,10))

        self.comboview = ttk.Combobox(view_frame)
        self.comboview["value"] = ("HSV - Farbton, Sättigung, Helligkeit", "Farbrad und Helligkeit")
        self.comboview.current(self.getConfigData("colorview")) #set the selected colorview

        self.comboview.pack(side="right", padx=10, pady=(10,10))

        #self.combodir = ttk.Combobox(view_frame)
        #self.combodir["value"] = ("Wheel direction: right", "Wheel direction: left")
        #self.combodir.current(self.getConfigData("wheeldirection")) #set the selected colorview

        #self.combodir.pack(side="right", padx=10, pady=(10,10))
        page_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        page_frame.columnconfigure(0, weight=1)

        label4text = tk.Label(page_frame, text="Startseite:")
        label4text.pack(side="left", padx=10, pady=(10,10))

        startpageindex = self.controller.getStartPageClassIndex()
        pagelist = self.controller.getTabNameList()

        self.combopage = ttk.Combobox(page_frame)
        self.combopage["value"] = pagelist   # 04.12.19: Added: "Serieller Monitor" 
        
        self.combopage.current(int(startpageindex)) #set the selected colorview
        #self.combopage.set(self.getConfigData("startpagename"))
        self.combopage.pack(side="right", padx=10, pady=(10,10))


        # --- LED MaxCount

        maxcount_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        maxcount_frame.columnconfigure(0, weight=1)

        self.ledmaxcount = LimitVar(1, 255, self)

        self.ledmaxcount.set(self.getConfigData("maxLEDcount"))

        self.s_ledmaxcount = Spinbox(maxcount_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledmaxcount, command=self._update_led_count)
        self.s_ledmaxcount.delete(0, 'end')
        self.s_ledmaxcount.insert(1, self.getConfigData("maxLEDcount"))
        self.s_ledmaxcount.pack(side="right", padx=10, pady=(10,10))


        ttk.Label(maxcount_frame, text=_('Maximale LED Anzahl')).pack(side="left", padx=10, pady=(10,10))

        # --- LED Correction
        ledcor_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        ledcor_frame.columnconfigure(0, weight=1)
        self.s_ledcorvar = tk.IntVar()
#        self.s_ledcor = ttk.Checkbutton(ledcor_frame,text=_("LED Farbstichkorrektur %"),variable=self.s_ledcorvar,onvalue = 1, offvalue = 0, command=self.cb)
        self.s_ledcor = ttk.Label(ledcor_frame,text=_("LED Farbstichkorrektur 0 .. 255"))
        self.s_ledcor.grid(sticky='w', padx=4, pady=4, row=0,column=0)
        self.s_ledcorvar.set(self.getConfigData("use_led_correction"))

        ledcor_frame.grid(row=5,column=0,columnspan=3,padx=10,pady=(10, 4),sticky="w")

        rgb_frame = ttk.Frame(ledcor_frame, relief="ridge", borderwidth=2)
        rgb_frame.grid(row=0,column=1,columnspan=3,padx=10,pady=(10, 4),sticky="w")
        rgb_frame.columnconfigure(0, weight=1)
        self.c_red = LimitVar(0, COLORCOR_MAX, self)
        self.c_green = LimitVar(0, COLORCOR_MAX, self)
        self.c_blue = LimitVar(0, COLORCOR_MAX, self)

        self.cs_red = Spinbox(rgb_frame, from_=0, to=COLORCOR_MAX, width=5, name='spinbox',
                        textvariable=self.c_red, command=self._update_cor_rgb)
        self.cs_green = Spinbox(rgb_frame, from_=0, to=COLORCOR_MAX, width=5, name='spinbox',
                          textvariable=self.c_green, command=self._update_cor_rgb)
        self.cs_blue = Spinbox(rgb_frame, from_=0, to=COLORCOR_MAX, width=5, name='spinbox',
                         textvariable=self.c_blue, command=self._update_cor_rgb)
        self.cs_red.delete(0, 'end')
        self.cs_red.insert(0, self.getConfigData("led_correction_r"))
        self.cs_green.delete(0, 'end')
        self.cs_green.insert(0, self.getConfigData("led_correction_g"))
        self.cs_blue.delete(0, 'end')
        self.cs_blue.insert(0, self.getConfigData("led_correction_b"))
        self.cs_red.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.cs_green.grid(row=0, column=3, sticky='e', padx=4, pady=4)
        self.cs_blue.grid(row=0, column=5, sticky='e', padx=4, pady=4)

        ttk.Label(rgb_frame, text=_('Rot')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün')).grid(row=0, column=2, sticky='e',
                                                   padx=0, pady=4)
        ttk.Label(rgb_frame, text=_('Blau')).grid(row=0, column=4, sticky='e',
                                                  padx=4, pady=4)

        # --- Buttons
        button_frame = ttk.Frame(self)
#
        ttk.Button(button_frame, text=_("Geänderte Einstellungen Übernehmen"), command=self.save_config).pack(side="right", padx=10)

        # --- autoconnect checkbox
        autocn_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        autocn_frame.columnconfigure(0, weight=1)
        self.s_autocnvar = tk.IntVar()
        self.s_autocn = ttk.Checkbutton(autocn_frame,text=_("Automatisch verbinden"),variable=self.s_autocnvar,onvalue = 1, offvalue = 0, command=self.autocn)
        self.s_autocn.grid(sticky='w', padx=4, pady=4, row=0,column=0)
        self.s_autocnvar.set(self.getConfigData("autoconnect"))

        # --- placement
        title_frame.grid(row=1, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        cfg_frame.grid(row=2, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")

        view_frame.grid(row=5, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        page_frame.grid(row=6, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        maxcount_frame.grid(row=7, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        autocn_frame.grid(row=8, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        ledcor_frame.grid(row=9, columnspan=2, pady=(20, 30), padx=10)
        button_frame.grid(row=10, columnspan=2, pady=(20, 30), padx=10)

    def tabselected(self):
        self.controller.currentTabClass = self.tabClassName
        self.ledmaxcount.set(self.controller.get_maxLEDcnt())
        print(self.tabname)
        pass
        
    # ----------------------------------------------------------------
    # ConfigurationPage save_config
    # ----------------------------------------------------------------

    def save_config(self):

        self.setConfigData("pos_x",self.winfo_x())
        self.setConfigData("pos_y",self.winfo_y())
        self.setConfigData("serportnumber", self.combo.current())
        self.setConfigData("serportname", self.combo.get())
        self.setConfigData("maxLEDcount", self.s_ledmaxcount.get())
        self.setConfigData("colorview", self.comboview.current())
        #self.setConfigData("wheeldir", self.combodir.current())
        self.setConfigData("startpage", "-1") # not used anymore
        startindex = self.combopage.current()
        
        startpagename = self.controller.getStartPageName(startindex)
        self.setConfigData("startpagename", startpagename)
        self.setConfigData("baudrate", self.comboBd.get())
        self.setConfigData("led_correction_r", self.cs_red.get())
        self.setConfigData("led_correction_g", self.cs_green.get())
        self.setConfigData("led_correction_b", self.cs_blue.get())
        
    def cancel(self):
        self.save_config()
        
    def connect (self):
        pass
    
    def disconnect (self):
        pass    

    def getConfigData(self, key):
        return self.controller.getConfigData(key)
    
    
    def readConfigData(self):
        self.controller.readConfigData()
        
    def setConfigData(self,key, value):
        self.controller.setConfigData(key, value)

    def setParamData(self,key, value):
        self.controller.setParamData(key, value)

    def MenuUndo(self,_event=None):
        pass
    
    def MenuRedo(self,_event=None):
        pass



    # ----------------------------------------------------------------
    # ConfigurationPage _update_led_count
    # ----------------------------------------------------------------

    def _update_led_count(self, event=None):
        if event is None or event.widget.old_value != event.widget.get():
            maxledcount = self.s_ledmaxcount.get()

    def autocn(self,event=None):
        if self.s_autocnvar.get() == 1:
            self.setConfigData("autoconnect", True)
        else:
            self.setConfigData("autoconnect", False)
            
    def _update_cor_rgb(self, event=None):
        """Update display after a change in the RGB spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            self.setConfigData("led_correction_r", self.cs_red.get())
            self.setConfigData("led_correction_g", self.cs_green.get())
            self.setConfigData("led_correction_b", self.cs_blue.get())

