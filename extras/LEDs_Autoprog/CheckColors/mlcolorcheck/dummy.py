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
from tkinter import ttk,messagebox
from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
    overlay, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv, convert_K_to_RGB
from tkcolorpicker.alphabar import AlphaBar
from tkcolorpicker.gradientbar import GradientBar
from tkcolorpicker.lightgradientbar import LightGradientBar
from tkcolorpicker.colorsquare import ColorSquare
from tkcolorpicker.colorwheel import ColorWheel
from tkcolorpicker.spinbox import Spinbox
from tkcolorpicker.limitvar import LimitVar
from mlcolorcheck.configfile import ConfigFile
from locale import getdefaultlocale
import re
import math
import os
import serial
import sys
import threading
import queue
import time
import logging
import concurrent.futures
import random
import webbrowser
from datetime import datetime
import json

VERSION ="V01.17 - 25.12.2019"
LARGE_FONT= ("Verdana", 12)
VERY_LARGE_FONT = ("Verdana", 14)
SMALL_FONT= ("Verdana", 8)

PARAM_FILENAME = 'MobaLedTest_param.json'
CONFIG_FILENAME = 'MobaLedTest_config.json'
DISCONNECT_FILENAME = 'MobaLedTest_disconnect.txt'
CLOSE_FILENAME = 'MobaLedTest_close.txt'
HELPPAGE_FILENAME = "MobaLedCheckColorsHelp01.htm"

FINISH_FILE = "Finished.txt"                                                 # 03.12.19:

PERCENT_BRIGHTNESS = 1  # 1 = Show the brightnes as percent, 0 = Show the brightnes as ">>>"# 03.12.19:

COLORCOR_MAX = 255

DEFAULT_PALETTE = {
                    "ROOM_COL0"   : "#0F0D03",
                    "ROOM_COL1"   : "#162C1B",
                    "ROOM_COL2"   : "#9B4905",
                    "ROOM_COL3"   : "#271201",
                    "ROOM_COL4"   : "#1E0000",
                    "ROOM_COL5"   : "#4F2707",
                    "GAS_LIGHT D" : "#323232",
                    "GAS LIGHT"   : "#FFFFFF",
                    "NEON_LIGHT D": "#14141B",
                    "NEON_LIGHT M": "#464650",
                    "NEON_LIGHT"  : "#F5F5FF",
                    "ROOM_TV0 A"  : "#323214",
                    "ROOM_TV0 B"  : "#46461E",
                    "ROOM_TV1 A"  : "#323208",
                    "ROOM_TV1 B"  : "#323208",
                    "SINGLE_LED"  : "#FFFFFF",
                    "SINGLE_LED D": "#323232"
                    }

DEFAULT_CONFIG = {
                    "serportnumber": 0,
                    "serportname": "No Device",
                    "baudrate": 57600,         # baudrate, possible values: 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200
                    "maxLEDcount": 255,
                    "lastLedCount": 1,
                    "lastLed": 0,
                    "pos_x": 100,
                    "pos_y": 100,
                    "colorview": 1,
                    "startpage": 0,
                    "led_correction_r": COLORCOR_MAX,
                    "led_correction_g": int(69*COLORCOR_MAX/100),
                    "led_correction_b": int(94*COLORCOR_MAX/100),
                    "use_led_correction": True,
                    "old_color": "#FFFFFF",
                    "palette": {
                        "ROOM_COL0"   : "#0F0D03",
                        "ROOM_COL1"   : "#162C1B",
                        "ROOM_COL2"   : "#9B4905",
                        "ROOM_COL3"   : "#271201",
                        "ROOM_COL4"   : "#1E0000",
                        "ROOM_COL5"   : "#4F2707",
                        "GAS_LIGHT D" : "#323232",
                        "GAS LIGHT"   : "#FFFFFF",
                        "NEON_LIGHT D": "#14141B",
                        "NEON_LIGHT M": "#464650",
                        "NEON_LIGHT"  : "#F5F5FF",
                        "ROOM_TV0 A"  : "#323214",
                        "ROOM_TV0 B"  : "#46461E",
                        "ROOM_TV1 A"  : "#323208",
                        "ROOM_TV1 B"  : "#323208",
                        "SINGLE_LED"  : "#FFFFFF",
                        "SINGLE_LED D": "#323232"
                    },
                    "autoconnect": False
                }

DEFAULT_PARAM = {}

CONFIG2PARAMKEYS = {
                    "old_color"   :"color",
                    "lastLed"     :"Lednum",
                    "lastLedCount":"LedCount", 
                    "serportname" :"comport",
                    "palette"     :"coltab", 
                    }



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
# Class LEDColorTest
# ----------------------------------------------------------------

class LEDColorTest(tk.Tk):
    
    def SaveFile(self):
        print ("Save File!")

    def OpenFile(self):
        name = filedialog.askopenfilename()
        print (name)

    def About(self):
        print ("MobaCheckColor by Harold Linke")

    def OpenHelp(self):
        print ("Öffne Hilfe")
        self.call_helppage()

    def ResetColorPalette(self):
        print ("ResetColorPalette")

    def ConnectArduino(self):
        print ("ConnectArduino")
        self.connect()

    def DisconnectArduino(self):
        print ("DisconnectArduino")
        self.disconnect()

    def SwitchoffallLEDs(self):
        print ("SwitchoffallLEDs")
        # switch off all LED
        message = "#L00 00 00 00 FF\n"
        self.send_to_ARDUINO(message)        
        
    def ExitProg(self):
        print ("Exit Prog!")
        self.cancel()
        
    # ----------------------------------------------------------------
    # LEDColorTest __init__
    # ----------------------------------------------------------------
    def __init__(self, *args, **kwargs):

        self.arduino = None
        self.queue = None
        self.readConfigData()
        
        tk.Tk.__init__(self, *args, **kwargs)
        screen_width = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()
        if self.getConfigData("pos_x") < screen_width and self.getConfigData("pos_y") < screen_height:

            self.geometry('+%d+%d' % (self.getConfigData("pos_x"), self.getConfigData("pos_y")))
        else:
            self.geometry("+100+100")

        tk.Tk.wm_title(self, "MobaLedLib " + VERSION)

        self.title("MobaLedLib " + VERSION)
        self.transient(self.master)
        self.resizable(False, False)
        self.rowconfigure(1, weight=1)

        self.color = ""
        style = ttk.Style(self)
        style.map("palette.TFrame", relief=[('focus', 'sunken')],
                  bordercolor=[('focus', "#4D4D4D")])
        self.configure(background=style.lookup("TFrame", "background"))
         
        menu = tk.Menu(self)
        self.config(menu=menu)
        filemenu = tk.Menu(menu)
        menu.add_cascade(label=_("Datei"), menu=filemenu)
        filemenu.add_command(label=_("Farbpalette lesen"), command=self.OpenFile)
        filemenu.add_command(label=_("Farbpalette sichern"), command=self.SaveFile)
        filemenu.add_separator()
        filemenu.add_command(label=_("Beenden und zurück zu Excel"), command=self.ExitProg)

        colormenu = tk.Menu(menu)
        menu.add_cascade(label=_("Farben"), menu=colormenu)
        colormenu.add_command(label=_("Farbenpalette zurücksetzen"), command=self.ResetColorPalette)

        arduinomenu = tk.Menu(menu)
        menu.add_cascade(label=_("ARDUINO"), menu=arduinomenu)
        arduinomenu.add_command(label=_("Verbinden"), command=self.ConnectArduino)
        arduinomenu.add_command(label=_("Trennen"), command=self.DisconnectArduino)
        arduinomenu.add_command(label=_("Alle LED aus"), command=self.SwitchoffallLEDs)
        
        helpmenu = tk.Menu(menu)
        menu.add_cascade(label=_("Hilfe"), menu=helpmenu)
        helpmenu.add_command(label=_("Hilfe öffnen"), command=self.OpenHelp)
        helpmenu.add_command(label=_("Über..."), command=self.About)

        # --- define container for configuration and colortest frame

        self.container = ttk.Notebook(self)

        self.container.pack(side="top", fill="both", expand = True)
        self.container.grid_rowconfigure(0, weight=1)
        self.container.grid_columnconfigure(0, weight=1)

        self.tabdict = dict()

        for key in tabpages:
            #F = tabpages[key]
            frame = tabpages[key](self.container,self)
            #frame = F(container, self)
            #self.frames[F] = frame
            self.tabdict[key] = frame
            self.container.add(frame, text=key)            

        self.continueCheckDisconnectFile = True
        self.onCheckDisconnectFile() 
        
        self.container.bind("<<NotebookTabChanged>>",self.TabChanged)
        
        if self.getConfigData("startpage") == 1:
            self.show_frame(ColorCheckPage)
        else:
            if self.getConfigData("startpage") == 2:                                      # 04.12.19:  
                self.show_frame(SerialMonitorPage)
            else:
                self.show_frame(ConfigurationPage)
        
    # ----------------------------------------------------------------
    # show_frame
    # ----------------------------------------------------------------
    def show_frame(self, cont):

        for key in tabpages:
            if cont == tabpages[key]:
                break
        
        frame = self.tabdict[key]
        if cont == ColorCheckPage:
            frame._update_cor_rgb()
 
        self.container.select(self.tabdict[key])

    # ----------------------------------------------------------------
    # Event TabChnaged
    # ----------------------------------------------------------------        
    def TabChanged(self,_event=None):
        tab_name = self.container.select()
        tab = self.nametowidget(tab_name)
        tab.tabselected()


    # ----------------------------------------------------------------
    #  cancel
    # ----------------------------------------------------------------
    def cancel(self):
        for key in self.tabdict:
            self.tabdict[key].cancel()
            
        self.setConfigData("pos_x", self.winfo_x())
        self.setConfigData("pos_y", self.winfo_y())
        #sdConfig.save()
        self.SaveConfigData()
        open(FINISH_FILE, 'a').close()                                       # 03.12.19:
        self.disconnect()
        self.continueCheckDisconnectFile = False
        self.destroy()


    # ----------------------------------------------------------------
    # LEDColorTest connect ARDUINO
    # ----------------------------------------------------------------
    def connect(self):

        global serialport
        
        print("connect")
        port = self.getConfigData("serportname")
        self.ARDUINO_message = ""
        self.set_pipeline(pipeline)
        timeout_error = False
        read_error =False
        
        if self.arduino and self.arduino.isOpen():
            self.disconnect()

        if port!="NO DEVICE" and not self.arduino:
            try:
                self.arduino = serial.Serial(port,baudrate=self.getConfigData("baudrate"),timeout=2,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS)
                print("connected to: " + self.arduino.portstr)
            except:                 # 03.12.19: more detailed error message
                messagebox.showerror("Error connecting to the serial port " + self.getConfigData("serportname"),
                                     "Serial Interface to ARDUINO could not be opened!\n"
                                     "\n"
                                     "Check if an other program is using the serial port\n"
                                     "This could be the Ardiono IDE/serial monitor or an\n"
                                     "other instance of this program.")
                self.ARDUINO_message = "ERROR! Connection could not be opened"
                port = "NO DEVICE"
                self.arduino = None
                serialport = None
                return False
            
            serialport = self.arduino
            timeout_error = True
            try:
                text=""
                text = serialport.readline()
                timeout_error = False
                self.queue.put(text)
                logging.info("Connect message: %s", text)
            except:
                read_error = True
                
                if read_error:         
                    messagebox.showerror("Error when reading Arduino answer",
                                         "ARDUINO is not answering correctly.\n"
                                         "\n"
                                         "Check if the correct program is loaded to the arduino.\n"
                                         "It must becompiled with the compiler switch:\n"
                                         "  #define RECEIVE_LED_COLOR_PER_RS232")
                    self.ARDUINO_message = "ARDUINO is not answering"
                    port = "NO DEVICE"
                    self.arduino = None
                    serialport = None
                    return False                

            if timeout_error:         # 03.12.19: more detailed error message
                messagebox.showerror("Timeout waiting for the Arduino answer",
                                     "ARDUINO is not answering.\n"
                                     "\n"
                                     "Check if the correct program is loaded to the arduino.\n"
                                     "It must becompiled with the compiler switch:\n"
                                     "  #define RECEIVE_LED_COLOR_PER_RS232")
                self.ARDUINO_message = "ARDUINO is not answering"
                port = "NO DEVICE"
                self.arduino = None
                serialport = None
                return False
            else:
                for key in self.tabdict:
                    self.tabdict[key].connect() 
            
                if self.getConfigData("startpage") == 1: # Just send the begin if the LED page shown at the start 
                    message = "#BEGIN\n"                                     # 04.12.19: New location 
                    self.send_to_ARDUINO(message)

                return True
 
            return False
        else:
            return False

    def set_pipeline(self,queue):
        self.queue = queue

    # ----------------------------------------------------------------
    # LEDColorTest disconnect ARDUINO
    # ----------------------------------------------------------------
    def disconnect(self):
        
        global serialport

        print("disconnect")
        if self.arduino:
            for key in self.tabdict:
                self.tabdict[key].disconnect()
                
            message = "#END\n"
            self.send_to_ARDUINO(message)

            self.arduino.close()
            self.arduino = None
            serialport = None

    # ----------------------------------------------------------------
    # getframe
    # ----------------------------------------------------------------
    def getframe(self, page):
        for key in tabpages:
            if page == tabpages[key]:
                break
        return self.tabdict[key]              

    # ----------------------------------------------------------------
    # LEDColorTest disconnect ARDUINO
    # ----------------------------------------------------------------
    def send_to_ARDUINO(self, message):

        if self.arduino:
            try:
                self.arduino.write(message.encode())
                print("Message send to ARDUINO: ",message)
                self.queue.put( ">> " + str(message))
            except:
                print("Error write message to ARDUINO")

    def call_helppage(self,event=None):
        webbrowser.open_new_tab(HELPPAGE_FILENAME)

    def onCheckDisconnectFile(self):
        # checks every 1 Second if the file DISCONNECT_FILENAME is found.
        # if yes: then disconnect ARDUINO - is used by external program to request a disconnect of the ARDUINO
        #logging.info("onCheckDisconnectFile running")
        if os.path.isfile(DISCONNECT_FILENAME):
            self.led_off()
            self.disconnect()

            # delete the file to avoid a series of disconnects
            try:
                os.remove(DISCONNECT_FILENAME)
            except:
                pass

        if os.path.isfile(CLOSE_FILENAME):
            self.led_off()
            self.disconnect()
            self.cancel()

            # delete the file to avoid a series of disconnects
            try:
                os.remove(CLOSE_FILENAME)
            except:
                pass
        
        if self.continueCheckDisconnectFile:
            self.after(1000, self.onCheckDisconnectFile)

    def getConfigData(self, key):
        newkey = CONFIG2PARAMKEYS.get(key,key) # translate configdata key into paramdata key
        return self.ParamData.data.get(newkey,self.ConfigData.data.get(key,DEFAULT_CONFIG.get(key))) # return Paramdata for the key if available else ConfigData
    
    def readConfigData(self):
        self.ConfigData = ConfigFile(DEFAULT_CONFIG, CONFIG_FILENAME)
        self.ParamData = ConfigFile(DEFAULT_PARAM, PARAM_FILENAME)
        
    def setConfigData(self,key, value):
        self.ConfigData.data[key] = value
    
    def setParamData(self,key, value):
        self.ParamData.data[key] = value

    def SaveConfigData(self):
        self.ConfigData.save()
        self.ParamData.save()


# ----------------------------------------------------------------
# Class ConfigurationPage
# ----------------------------------------------------------------

class ConfigurationPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        self.tabname = "Einstellungen"
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

        self.comboBd.pack(side="right", padx=10, pady=(10,10) )

        view_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        view_frame.columnconfigure(0, weight=1)

        label3text = tk.Label(view_frame, text="Farbauswahlansicht (Neustart notwendig):")
        label3text.pack(side="left", padx=10, pady=(10,10))

        self.comboview = ttk.Combobox(view_frame)
        self.comboview["value"] = ("HSV - Farbton, Sättigung, Helligkeit", "Farbrad und Helligkeit")
        self.comboview.current(self.getConfigData("colorview")) #set the selected colorview

        self.comboview.pack(side="right", padx=10, pady=(10,10))

        page_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        page_frame.columnconfigure(0, weight=1)

        label4text = tk.Label(page_frame, text="Startseite:")
        label4text.pack(side="left", padx=10, pady=(10,10))

        self.combopage = ttk.Combobox(page_frame)
        self.combopage["value"] = ("Konfiguration", "LED Farbauswahl", "Serieller Monitor")   # 04.12.19: Added: "Serieller Monitor" 
        self.combopage.current(self.getConfigData("startpage")) #set the selected colorview

        self.combopage.pack(side="right", padx=10, pady=(10,10))


        # --- LED MaxCount

        maxcount_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        maxcount_frame.columnconfigure(0, weight=1)

        self.ledmaxcount = LimitVar(1, 256, self)

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
        self.setConfigData("startpage", self.combopage.current())
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

            
class SerialMonitorPage(tk.Frame):
    def __init__(self, parent, controller):
        self.controller = controller
        self.tabname = "Serial Monitor"
        tk.Frame.__init__(self, parent)
        title_frame = ttk.Frame(self)

        ttk.Label(title_frame, text="Simple Serial Monitor").pack(side="left", padx=4, pady=(4, 1))
        title_frame.grid(row=0, column=0,padx=10, pady=(10, 4))

        input_frame = ttk.Frame(self)

        self.input = ttk.Entry(input_frame, justify="center", width=30, name='entry')
        self.input.insert(0, "")
        ttk.Label(input_frame, text="Senden").pack(side="left", padx=4, pady=(4, 1))
        self.input.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)
        self.send_button = ttk.Button(input_frame, text=_("Senden"),width=10, command=self.send)
        self.send_button.pack(side="left", padx=4, pady=(4, 1))

        input_frame.grid(row=1, column=0,padx=10, pady=(10, 4))
        
        self.input.bind("<Return>",self.send)

        frameLabel = tk.Frame(self, padx=40, pady =40)

        self.text = tk.Text(frameLabel, wrap='word', bg=self.cget('bg'), height=30, width=80)         # 04.12.19: Old: height=10, width=50
        frameLabel.grid(row=2, column=0,padx=10, pady=(10, 4))

        # add a vertical scroll bar to the text area
        scroll=tk.Scrollbar(frameLabel)
        self.text.configure(yscrollcommand=scroll.set)
        scroll.config(command=self.text.yview)

        #pack everything
        self.text.pack(side=tk.LEFT)
        scroll.pack(side=tk.RIGHT,fill=tk.Y)

        # binding
        self.input.bind("T",self.dummy)
        self.input.bind("t",self.dummy)

        button_frame = ttk.Frame(self)

        button_width = 20
 
        self.queue = queue.Queue()
        self.monitor_serial = False

    def tabselected(self):
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

    
    
    def dummy(self,event):
        print("dummy")
        

    def send(self,event=None):
        message = self.input.get() +"\r\n"
        self.controller.send_to_ARDUINO(message)

    def process_serial(self):

        #print("process_serial: Start")
        if self.controller.queue:
            while self.controller.queue.qsize():
                #print("process_serial: While loop")
                try:
                    readtext = self.controller.queue.get()
                    date_time = datetime.now()
                    d = date_time.strftime("%H:%M:%S")
                    textmessage = d + "  " + str(readtext)
                    if not textmessage.endswith("\n"):
                        textmessage = textmessage +"\n"
                    self.text.insert("end", textmessage)
                    self.text.yview("end")
                    frame = self.controller.frames[ColorCheckPage]
                    frame.set_ARDUINO_message(textmessage)
                except:
                    pass
        if self.monitor_serial:
            self.after(100, self.process_serial)

    def start_process_serial(self):
        global ThreadEvent
        ThreadEvent.set()
        time.sleep(2)
        ThreadEvent.clear()
        self.monitor_serial = True
        thread = SerialThread(self.controller.queue,self.controller.arduino)
        thread.start()
        self.process_serial()

    def stop_process_serial(self):
        global ThreadEvent
        self.monitor_serial = False
        ThreadEvent.set()
        time.sleep(1)
        
    def cancel(self):
        self.stop_process_serial()
        
    def connect (self):
        self.start_process_serial()
    
    def disconnect (self):
        self.stop_process_serial()


class RightClickMenu(tk.Frame):   
    def __init__(self, parent, colorlabel):
        self.master = parent
        tk.Frame.__init__(self, self.master)  
        self.colorlabel = colorlabel
        self.create_widgets()

    def create_widgets(self):
        self.create_right_click_menu()       

    def create_right_click_menu(self):
        self.right_click_menu = tk.Menu(self.master, tearoff=0, relief='sunken')
        self.right_click_menu.add_command(label="aktuelle Farbe in Palette uebernehmen", command=self.set_act_color)
        self.right_click_menu.add_command(label="alte Farbe in Palette uebernehmen", command=self.set_old_color)
        self.right_click_menu.add_separator()
        self.right_click_menu.add_command(label="Farbe auf Standard zuruecksetzen", command=self.reset_color)

    def popup_text(self, event):
        self.right_click_menu.post(event.x_root, event.y_root)

    def set_act_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage=label.master.master.master.master
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
        mainpage.palette[key] = color
        mainpage._update_preview()    

    def set_old_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage=label.master.master.master.master
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
        mainpage.palette[key] = color
        mainpage._update_preview()    


    def reset_color(self):
        label = self.colorlabel #event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        mainpage=label.master.master.master.master
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

        
# ----------------------------------------------------------------
# Class ColorCheckPage
# ----------------------------------------------------------------

class ColorCheckPage(tk.Frame):

    # ----------------------------------------------------------------
    # ColorCheckPage __init__
    # ----------------------------------------------------------------

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.tabname = "LED Farbtest"
        
        self.controller = controller
        self.parent = parent
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
        
        # --- Colorwheel or Colorquare ?
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

            # --- ColorWheel
            square = ttk.Frame(self, borderwidth=2, relief='groove')
            self.square = ColorWheel(square, hue=hue, width=200, height=200,
                                      color=rgb_to_hsv(*self._old_color),
                                      highlightthickness=0, cr = int(self.cor_red), cg=int(self.cor_green), cb=int(self.cor_blue))
            self.square.pack()

 
        # --- color preview: initial color and currently selected color side by side
        preview_frame = ttk.Frame(self.main_frame, relief="raised", borderwidth=1)
        preview_frame.grid(row=0, column=0, sticky="nw", pady=2)
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(old_color[:7])
        self.old_color_prev = tk.Label(preview_frame, background=old_color_disp,
                                  width=12, highlightthickness=0, height=2,
                                  padx=2, pady=2,text=_("alte Farbe")+"\n"+brightness_text)
        self.color_preview = tk.Label(preview_frame, width=12, height=2,
                                      pady=2, background=old_color_disp,
                                      padx=2, highlightthickness=0, text=_("aktuelle Farbe")+"\n"+brightness_text)
        self.old_color_prev.bind("<1>", self._reset_preview)
        self.old_color_prev.grid(row=0, column=0)
        self.color_preview.grid(row=1, column=0)

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
            l.bind("<Button-1>", self._palette_cmd)
            #l.bind("<Button-3>", self._palette_save_cmd)
            
            self.bind_right_click_menu_to_palettelabel(l)
            
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)

        # --- hsv

        col_frame = ttk.Frame(self)

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
        ttk.Label(hsv_frame, text=_('Hue (Farbton) [H/h]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Saturation (Sättigung) [S/s]')).grid(row=1, column=0, sticky='e',
                                                        padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Value (Helligkeit) [V/v]')).grid(row=2, column=0, sticky='e',
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
        ttk.Label(hexa_frame, text="HEXA").pack(side="left", padx=4, pady=(4, 1))
        self.hexa.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)


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
        # --- ARDUINO Steuerung

        arduino_frame = ttk.Frame(self,relief="ridge", borderwidth=2,width=500)
#        ttk.Label(arduino_frame, text=_('ARDUINO Steuerung')).grid(row=0, column=0, columnspan=3, sticky='', padx=4, pady=4)
        self.arduino_status = tk.Label(arduino_frame, text=_('Nicht verbunden'), bg="lightgray",width=20)
        self.arduino_status.grid(row=0, column=2, sticky='', padx=4, pady=(10, 4))

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

        ttk.Label(led_frame, text=_('LED Adresse [+/-]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(led_frame, text=_('Anzahl [A/a]')).grid(row=0, column=2, sticky='e',
                                                 padx=4, pady=4)

        led_frame.grid(row=1,column=0,columnspan=2,padx=10,pady=(10, 4),sticky="w")
        ttk.Button(arduino_frame, text=_("Alle LED aus [CTRL-c]"),width=20, command=self.led_off).grid(row=1, column=2, padx=10, pady=(10, 4), sticky='')


        # --- LED Color correction
        ledcor_frame = ttk.Frame(arduino_frame, relief="ridge", borderwidth=2)
        ledcor_frame.columnconfigure(0, weight=1)

        ledcor_frame.grid(row=5,column=0,columnspan=3,padx=10,pady=(10, 4),sticky="w")

        ttk.Label(ledcor_frame, width = 50, text=_('ARDUINO Message:')).grid(row=0, column=0, columnspan=4, sticky='w', rowspan=3, padx=4, pady=4)
        self.s_ledcorvalue = ttk.Label(ledcor_frame, text=_(' '))
        self.s_ledcorvalue.grid(row=0, column=2, columnspan=1, sticky='w', padx=4, pady=4)
        self.ledcorvalue = " "

        # --- placement
        bar.grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        square.grid(row=1, column=0, padx=10, pady=(9, 0), sticky='n')
        col_frame.grid(row=0, rowspan=2, column=1, padx=(4, 10), pady=(10, 4))
        self.main_frame.grid(row=3, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="nsew")
        arduino_frame.grid(row=4, column=0, columnspan=2,pady=(0, 10), padx=10)
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
        self.s_led.bind("<FocusOut>", self._update_color_rgb)
        self.s_led.bind("<Return>", self._update_color_rgb)
        self.s_led.bind("<Control-a>", self._select_all_entry)
        self.s_ledcount.bind("<FocusOut>", self._update_color_rgb)
        self.s_ledcount.bind("<Return>", self._update_color_rgb)
        self.s_ledcount.bind("<Control-a>", self._select_all_entry)        
        
        self.controller.bind("H",self.s_h.invoke_buttonup)
        self.controller.bind("h",self.s_h.invoke_buttondown)
        self.controller.bind("S",self.s_s.invoke_buttonup)
        self.controller.bind("s",self.s_s.invoke_buttondown)
        self.controller.bind("V",self.s_v.invoke_buttonup)
        self.controller.bind("v",self.s_v.invoke_buttondown)
        self.controller.bind("R",self.s_red.invoke_buttonup)
        self.controller.bind("r",self.s_red.invoke_buttondown)
        self.controller.bind("G",self.s_green.invoke_buttonup)
        self.controller.bind("g",self.s_green.invoke_buttondown)
        self.controller.bind("B",self.s_blue.invoke_buttonup)
        self.controller.bind("b",self.s_blue.invoke_buttondown)
        self.controller.bind("+",self.s_led.invoke_buttonup)
        self.controller.bind("-",self.s_led.invoke_buttondown)
        self.controller.bind("A",self.s_ledcount.invoke_buttonup)
        self.controller.bind("a",self.s_ledcount.invoke_buttondown)
        self.controller.bind("T",self.s_ct.invoke_buttonup)
        self.controller.bind("t",self.s_ct.invoke_buttondown)
        self.controller.bind("<F1>",self.controller.call_helppage)
        #self.bind("<Escape>",self.cancel)                                     
        self.controller.bind("<Control-c>",self.led_off)

        self._update_color_hsv()

        self._update_preview()

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
        self.setParamData("cor_color", self.ledcorvalue.upper()[-7:]) # corrected color
        self.setParamData("Lednum"   , self.lednum.get())
        self.setParamData("LedCount" , self.ledcount.get())
        self.setParamData("coltab"   , self.palette.copy())

        self.continueCheckDisconnectFile = False
        time.sleep (1)
        
    def tabselected(self):
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
        self.arduino_status.configure(bg="green",text=_("Verbunden"))

        max_len = 40
        if len(self.controller.ARDUINO_message) > max_len:
            message = self.controller.ARDUINO_message[:max_len]
        else:
            message = self.controller.ARDUINO_message

        self.s_ledcorvalue.configure(text=message)

    def disconnect(self):
        #self.controller.disconnect()
        self.arduino_status.configure(bg="lightgray",text=_("Nicht Verbunden"))

    def set_ARDUINO_message(self,readtext):
        self.s_ledcorvalue.configure(text=readtext)

    def bind_right_click_menu_to_palettelabel(self, palettelabel):
        self.popup = RightClickMenu(self.master, palettelabel)        
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
         
        return disp_color,brightness


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
        red=self.red.get()
        green= self.green.get()
        blue = self.blue.get()
        args = (red,green,blue)
        color = rgb_to_hexa(*args)
        color_disp,brightness_text = self.keycolor_to_dispcolor(color)
        self.color_preview.configure(background=color_disp)
        self.color_preview.configure(text="aktuelle Farbe\n"+brightness_text)
        ledcount = self.ledcount.get()
        if ledcount >0:
            lednum = self.lednum.get()
            message = "#L" + '{:02x}'.format(self.lednum.get()) + " " + '{:02x}'.format(red) + " " + '{:02x}'.format(green) + " " + '{:02x}'.format(blue) + " " + '{:02x}'.format(self.ledcount.get()) + "\n"
            self.controller.send_to_ARDUINO(message)

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

    def _palette_cmd(self, event):
        """Respond to user click on a palette item."""
        
        # set old_color = current color
        current_color = self.hexa.get()
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(current_color)
        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brightness_text)
        self._old_color = hexa_to_rgb(current_color)
        
        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        text = event.widget["text"]
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
        """Respond to user right click on a palette item."""

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
        self.palette[key] = color
        self._update_preview()

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
            
            # l.bind("<Button-3>", self._palette_save_cmd)
            self.bind_right_click_menu_to_palettelabel(l)
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)        
        
    def _palette_reset_colors(self):
        
        self.palette = DEFAULT_PALETTE.copy()
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
            self.bar.set(h,s,v)
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
            self.bar.set(h,s,v)
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
            led = self.lednum.get()
            self._update_preview()

    def _update_led_count(self, event=None):
        """Update display after a change in the LED count spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ledcount = self.ledcount.get()
            self._update_preview()

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
        message = "#L00 00 00 00 FF\n"
        self.controller.send_to_ARDUINO(message)


class SerialThread(threading.Thread):
    def __init__(self, p_queue, p_serialport):
        global serialport
        threading.Thread.__init__(self)
        self.queue = p_queue
        serialport = p_serialport

    def run(self):
        global serialport
        logging.info("SerialThread started")
        if serialport:

            while not ThreadEvent.is_set() and serialport:
                # print("Event:",ThreadEvent.is_set())
                # print("Serialport:", serialport)
                # logging.info("SerialThread while loop")
                #logging.info("SerialThread running")
                try:
                    text = serialport.readline()
                except:
                    pass
                if len(text)>0:
                    try:
                        self.queue.put(text)
                    except:
                        pass
                    logging.info("SerialThread got message: %s", text)

        logging.info("SerialThread received event. Exiting")


#-------------------------------------------

tabpages = { "LED Farbtest"   : ColorCheckPage,
             "Serial Monitor" : SerialMonitorPage,
             "Einstellungen"  : ConfigurationPage
             }

ThreadEvent = None
serialport = None

format = "%(asctime)s: %(message)s"
logging.basicConfig(format=format, level=logging.INFO,
                    datefmt="%H:%M:%S")

ThreadEvent = threading.Event()
pipeline = queue.Queue(maxsize=20)

app = LEDColorTest()
app.protocol("WM_DELETE_WINDOW", app.cancel)

serialport = app.arduino

if app.getConfigData("autoconnect"):
    app.connect()

app.mainloop()

#app.disconnect()                            # 04.12.19: send the "#END" Message to restart the arduino

ThreadEvent.set()
