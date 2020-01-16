# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
# * Version: 1.21
# * Author: Harold Linke
# * Date: January 1st, 2020
# * Copyright: Harold Linke 2020
# *
# *
# * MobaLedCheckColors on Github: https://github.com/haroldlinke/MobaLedCheckColors
# *
# *
# * History of Change
# * V1.00 23.11.2019 - Harold Linke - first release
# * V1.09 29.11.2019
# * V1.10 03.12.2019 - Hardi
# *                  - Brightness could be shown in pecent if the constant PERCENT_BRIGHTNESS is set to 1
# *                  - Write a finish file at the end of the program. This is needed by the calling excel program.
# *                    Harold: Is this file also written in case of a crash ?
# *                  - Some more detailed error messages
# *       04.12.2019 - Added COM Ports 7..40 because on some systems such high com ports are used (Hardi: COM12)
# *                    Hint: For test purposes the COM Port could be changed in the device manager: "devmgmt.msc" in windows
# *                  - Send "#End" when the program is closed to restart the arduino. Prior if was still in the ColorChecker mode (LED was flashing fast)
# *                  - Moved the sending og the "#BEGIN" command down because the Arduino is restarted if the serial connection is opened
# *                    => The "#BEGIN" command was not received
# *                    With this change the first press to a color button is raccepted (Prior a second press was needed)
# *                  - The serial monitor could be started in the json file with "startpage": 2,
# *                  - Increased the size of the serial monitor window
# *                  - Added "End" button to the serial monitor
# * V1.13 13.12.2019 - Harold
# *                  - corrector error in reset color palette procedure, color palette was not copied correctly
# *                  - corrected old_color behavior, stores now the current color when a new color is selected from the palette
# *                  - color correction moved to configuration page to reduce size of window for smaller screens
# * V1.14 15.12.2019 - Harold
# *                  - program was nearly unreactiv, when ARDUINO connection was disconnected, corrected
# * V1.15 16.12.2019 - Harold
# *                  - Baudrate can be configured
# *                  - Collorcorrection moved to config page
# *                  - HSV is using English terms
# * V1.16 26-12-2019 - Harold
# *                  - complete restructuring of all parts
# *                  - check for response of ARDUINO - first response must include "MobaLedLib"
# *                  - use tabs to switch between pages
# *                  - menu for accessing functions
# *                  - right-click menu for palette colors
# *                  - reset to standard color isnow possible for each palette entry seperately
# *                  - Palette can now be saved to file and read back
# *                  - Baudrate fixed to 115000
# *                  - reduced size to comply with smaller screens
# *                  - colorwheel changed to conform with Philips Hue colorwheel
# *                  - tooltips added
# *                  - LED(s) now blinks in White when selected
# *                  - All colors for the LEDs are saved in an LED list. The list is send to the ARDUINO when the color is changed.
# * V1.17 28-12-2019 - BLINKFRQ changed to 1 Hz
# *                  - deleted Baudrate selection - fixed to 115000
# *                  - LEDListPage now shows the colors of the LEDs
# *                   
# * V1.18 28-12-2019 - Checks now for "#Color Test LED" during connection - if not received after 5 seocnds connection is interrupted (may be wrong ARDUINO answering
# *                  - receive max LED count from ARDUINO - only in ConfigPage - not dynamically updated in ColorCheckPage yet
# *                  - corrected calculation of BLINKFRQ
# *                  - Colorcorrection vaules are now taken into account immediatly without restart
# *                  - Colorsquare removed
# *                  - Shortcut key hints moved to tooltips
# *                  - new Status line at the bottom shows ARDUINO Status
# * V1.19 29-12-2019 - Neue Datei-Menu  Option „Beenden und Änderungen verwerfen.
# *                  - Colorcheckpage als Startseite wenn nichts angegeben ist
# *                  - Blinkende LED anpassen, wenn Anzahl geändert wird
# *                  - Bei Click auf die Aktuelle Farbe sollte die LED die aktuelle Farbe auch anzeigen.
# *                  - Vertausche Cursortasten für Startadresse und Anzahl 
# *                  - add button for blinking off
# *                  - CTRL-Right Mousebutton for save color in palette
# * V1.20 30-12-2019 - replace old_color with undo feature
# *                  - Abfrage beim Benenden ohne Speichern geändert - Soll die Palette (ColTab) gesichert werden?
# *                  - all LEDs are switched off at start-up
# *                  - error when ARDUINO connection to possible during startup with shortcuts and text entry not working corrected
# *                  - help page updated
# *                  - ERROR: LED selected after start of program automatically gets the current color - corrected
# * V1.21 01-01-2020 - ColTab Parameter von MobaLedLib wurden nicht richtig gelesen
# *                  - Fehlerkorrektur in V1.19 "LED selected after start of program automatically gets the current color - corrected" führte zu dem Fehler, dass die Anfangshelligkeit auf 0 gesetzt war - korrigiert
# * V1.22 02-01-2020 - Switch to HSV square did not work correctly
# *                  - Hue can now turnaround (after 360 it switches to 0)
# * V1.23 02-01-2020 - reads ledcnt_max from ARDUINO and checks if led-number + led count < ledcnt_max
# * V1.24 03-01-2020 - question to save palette only when palette ws changed
# *                  - window position will be saved correctly after cancel without save
# *                  - Buttob blinking off changed to on and off switch
# * V1.25 03-01-2020 - test chnaged from "MoDaLedLib" to „LEDs_AutoProg"
# *                  - "b'" removed from read string from serial interface
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
from tkinter import ttk,messagebox,filedialog
from mlcolorcheck.configfile import ConfigFile
from mlcolorcheck.dictFile import readDictFromFile, saveDicttoFile
from mlcolorcheck.ConfigurationPage import ConfigurationPage
from mlcolorcheck.SerialMonitorPage import SerialMonitorPage
from mlcolorcheck.ColorCheckPage import ColorCheckPage
from mlcolorcheck.LEDListPage import LEDListPage
from mlcolorcheck.SoundCheckPage import SoundCheckPage
from mlcolorcheck.tooltip import Tooltip
from mlcolorcheck.DefaultConstants import COLORCOR_MAX, CONFIG2PARAMKEYS, DEFAULT_CONFIG, DEFAULT_PALETTE, DEFAULT_PARAM, LARGE_FONT, SMALL_FONT, VERY_LARGE_FONT, VERSION, \
PARAM_FILENAME, CONFIG_FILENAME, DISCONNECT_FILENAME, CLOSE_FILENAME, HELPPAGE_FILENAME, FINISH_FILE, PERCENT_BRIGHTNESS, TOOLTIPLIST, SerialIF_teststring1, SerialIF_teststring2

from locale import getdefaultlocale
import os
import serial
import sys
import threading
import queue
import time
import logging
import webbrowser
from datetime import datetime


# --- Translation - not used
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha", "Configuration": "Einstellungen",
      "ROOM_COL0": "SetColTab Parameter: ROOM_COL0\nRaumfarbe 0 \nRechte Maustaste für Funktionen",
      "ROOM_COL1": "SetColTab Parameter: ROOM_COL1\nRaumfarbe 1 \nRechte Maustaste für Funktionen",
      "ROOM_COL2": "SetColTab Parameter: ROOM_COL2\nRaumfarbe 2 \nRechte Maustaste für Funktionen",
      "ROOM_COL3": "SetColTab Parameter: ROOM_COL3\nRaumfarbe 3 \nRechte Maustaste für Funktionen",
      "ROOM_COL4": "SetColTab Parameter: ROOM_COL4\nRaumfarbe 4 \nRechte Maustaste für Funktionen",
      "ROOM_COL5": "SetColTab Parameter: ROOM_COL5\nRaumfarbe 5 \nRechte Maustaste für Funktionen",
      "GAS_LIGHT D" : "SetColTab Parameter: GAS_LIGHT D\nSimuliert dunkles Gaslicht \nRechte Maustaste für Funktionen",
      "GAS LIGHT"   : "SetColTab Parameter: GAS LIGHT\nSimuliert Gaslicht \nRechte Maustaste für Funktionen",
      "NEON_LIGHT D": "SetColTab Parameter: NEON_LIGHT D\nSimuliert dunkles Neonlicht \nRechte Maustaste für Funktionen",
      "NEON_LIGHT M": "SetColTab Parameter: NEON_LIGHT M\nSimuliert mittel helles Neonlicht \nRechte Maustaste für Funktionen",
      "NEON_LIGHT"  : "SetColTab Parameter: NEON_LIGHT\nSimuliert Neonlicht \nRechte Maustaste für Funktionen",
      "ROOM_TV0 A"  : "SetColTab Parameter: ROOM_TV0 A\nRaumfarbe A - wenn TV0 aus ist \nRechte Maustaste für Funktionen",
      "ROOM_TV0 B"  : "SetColTab Parameter: ROOM_TV0 B\nRaumfarbe B - wenn TV0 aus ist \nRechte Maustaste für Funktionen",
      "ROOM_TV1 A"  : "SetColTab Parameter: ROOM_TV1 A\nRaumfarbe A - wenn TV1 aus ist \nRechte Maustaste für Funktionen",
      "ROOM_TV1 B"  : "SetColTab Parameter: ROOM_TV1 B\nRaumfarbe B - wenn TV1 aus ist \nRechte Maustaste für Funktionen",
      "SINGLE_LED"  : "SetColTab Parameter: SINGLE_LED \nEinzel LEDs \nRechte Maustaste für Funktionen",
      "SINGLE_LED D": "SetColTab Parameter: SINGLE_LED D\nEinzel LEDs dunkel \nRechte Maustaste für Funktionen"            
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

# ------------------------------

# ********************************************************************************
# *
# * definition of different tab dictionaries defining the relationship between tabname, pagename, page classes and index
# * in fact only one page would be needed, and all others could begenerate automatically, but for clarity it is done here explicitely

tabClassList = (ColorCheckPage, SerialMonitorPage, ConfigurationPage, LEDListPage,SoundCheckPage)

tabNameList  = ("LED Farbtest", "Serial Monitor", "Einstellungen", "LED Liste", "Sound Test")

tabClassName2Class = {
                    "ColorCheckPage"    : ColorCheckPage,
                    "SerialMonitorPage" : SerialMonitorPage,
                    "ConfigurationPage" : ConfigurationPage,
                    "LEDListPage"       : LEDListPage,
                    "SoundCheckPage"    : SoundCheckPage
                    }

tabClassName2Name = {
                    "ColorCheckPage"    : "LED Farbtest",
                    "SerialMonitorPage" : "Serial Monitor",
                    "ConfigurationPage" : "Einstellungen",
                    "LEDListPage"       : "LED Liste",
                    "SoundCheckPage"    : "Sound Test"
                    }

tabClassName2Index = {
                    "ColorCheckPage"    : "0",
                    "SerialMonitorPage" : "1",
                    "ConfigurationPage" : "2",
                    "LEDListPage"       : "3",
                    "SoundCheckPage"    : "4"
                    }

tabIndex2ClassName = {
                    "0": "ColorCheckPage",
                    "1" : "SerialMonitorPage",
                    "2" : "ConfigurationPage",
                    "3" : "LEDListPage",
                    "4" : "SoundCheckPage"
                    }

# for compatibility with the "old" startpagenumber in the config file
# if startpagenumber is in this list the corresponding start page is opened, otherwise "ColorCheckPage"
startpageNumber2Name = { 
    "0": "ConfigurationPage",
    "1": "ColorCheckPage",
    "2": "SerialMonitorPage"
}

defaultStartPage = "ColorCheckPage"

# ----------------------------------------------------------------
# Class LEDColorTest
# ----------------------------------------------------------------

class LEDColorTest(tk.Tk):
    
    # ----------------------------------------------------------------
    # LEDColorTest __init__
    # ----------------------------------------------------------------
    def __init__(self, *args, **kwargs):

        self.arduino = None
        self.queue = queue.Queue(maxsize=100)
        self.readConfigData()
        self.ledtable = {}
        self.currentTabClass = ""
        self.maxLEDcnt = 0
        self.paramDataChanged = False

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
        filemenu.add_command(label=_("Farbpalette von Datei lesen"), command=self.OpenFile)
        filemenu.add_command(label=_("Farbpalette speichern als ..."), command=self.SaveFileas)
        filemenu.add_separator()
        filemenu.add_command(label=_("LED Tabelle von Datei lesen"), command=self.OpenFileLEDTab)
        filemenu.add_command(label=_("LED Tabelle speichern als"), command=self.SaveFileLEDTab)
        filemenu.add_separator()
        filemenu.add_command(label=_("Beenden und Daten speichern"), command=self.ExitProg_with_save)
        filemenu.add_command(label=_("Beenden ohne Daten zu speichern"), command=self.ExitProg)

        colormenu = tk.Menu(menu)
        menu.add_cascade(label=_("Farbpalette"), menu=colormenu)
        colormenu.add_command(label=_("letzte Änderung Rückgängig machen [CTRL-Z]"), command=self.MenuUndo)
        #colormenu.add_command(label=_("Farbpalette Wiederherstellen [CRTL-Y]"), command=self.MenuRedo)        
        colormenu.add_command(label=_("von Datei lesen"), command=self.OpenFile)
        colormenu.add_command(label=_("speichern als ..."), command=self.SaveFileas)
        colormenu.add_separator()
        colormenu.add_command(label=_("auf Standard zurücksetzen"), command=self.ResetColorPalette)

        arduinomenu = tk.Menu(menu)
        menu.add_cascade(label=_("ARDUINO"), menu=arduinomenu)
        arduinomenu.add_command(label=_("Verbinden"), command=self.ConnectArduino)
        arduinomenu.add_command(label=_("Trennen"), command=self.DisconnectArduino)
        arduinomenu.add_command(label=_("Alle LED aus"), command=self.SwitchoffallLEDs)
        
        helpmenu = tk.Menu(menu)
        menu.add_cascade(label=_("Hilfe"), menu=helpmenu)
        helpmenu.add_command(label=_("Hilfe öffnen"), command=self.OpenHelp)
        helpmenu.add_command(label=_("Über..."), command=self.About)

        # --- define container for tabs

        self.container = ttk.Notebook(self)

        self.container.pack(side="top", fill="both", expand = True)
        self.container.grid_rowconfigure(0, weight=1)
        self.container.grid_columnconfigure(0, weight=1)

        self.tabdict = dict()
        
        for tabClass in tabClassList:
            frame = tabClass(self.container,self)
            tabClassName = frame.tabClassName
            self.tabdict[tabClassName] = frame
            self.container.add(frame, text=frame.tabname)
            
        self.continueCheckDisconnectFile = True
        self.onCheckDisconnectFile() 
        
        self.container.bind("<<NotebookTabChanged>>",self.TabChanged)
        
        startpagename = self.getStartPageClassName()
        
        #if self.getConfigData("startpage") == 1:
        #    self.showFramebyName("ColorCheckPage")
        #else:
        #    if self.getConfigData("startpage") == 2:                                      # 04.12.19:  
        #        self.showFramebyName("SerialMonitorPage")
        #    else:
        #        self.showFramebyName("ConfigurationPage")
        
        self.showFramebyName(startpagename)
        
        self.statusmessage = tk.Label(self, text=_('Status:'), fg="black",bd=1, relief="sunken", anchor="w")
        self.statusmessage.pack(side="bottom", fill="x")
        self.ToolTip(self.statusmessage, text="Zeigt den Status der Verbindung zum ARDUINO an: \nVerbunden - eine Verbidnug zum ARDUINO steht\nNicht verbunden - keine Verbindung zum ARDUINO")
        
        self.focus_set()
        self.wait_visibility()
                
        self.lift()
        self.grab_set()        


    def SaveFileas(self):
        filepath = filedialog.asksaveasfilename(filetypes=[("JSON files","*.json")],defaultextension=".json")
        if filepath:
            frame = self.tabdict["ColorCheckPage"]
            frame.savePalettetoFile(filepath) 

    def SaveFile(self):
        filepath = PARAM_FILENAME
        if filepath:
            frame = self.tabdict["ColorCheckPage"]
            frame.savePalettetoFile(filepath) 

    def OpenFile(self):
        filepath = filedialog.askopenfilename(filetypes=[("JSON files","*.json")],defaultextension=".json")
        if filepath:
            frame = self.tabdict["ColorCheckPage"]
            frame.readPalettefromFile(filepath)         

    def SaveFileLEDTab(self):
        filepath = filedialog.asksaveasfilename(filetypes=[("JSON files","*.json")],defaultextension=".json")
        if filepath:
            self.saveLEDTabtoFile(filepath)

    def OpenFileLEDTab(self):
        filepath = filedialog.askopenfilename(filetypes=[("JSON files","*.json")],defaultextension=".json")
        if filepath:
            self.readLEDTabfromFile(filepath)     

    def About(self):
        tk.messagebox("MobaCheckColor by Harold Linke")

    def OpenHelp(self):
        self.call_helppage()

    def ResetColorPalette(self):
        frame = self.tabdict["ColorCheckPage"]
        frame.palette_reset_colors()

    def MenuUndo(self,_event=None):
        for key in self.tabdict:
            self.tabdict[key].MenuUndo() 
    
    def MenuRedo(self,_event=None):
        for key in self.tabdict:
            self.tabdict[key].MenuRedo()

    def ConnectArduino(self):
        self.connect()

    def DisconnectArduino(self):
        self.disconnect()

    def SwitchoffallLEDs(self):
        # switch off all LED
        message = "#L00 00 00 00 FF\n"
        self.send_to_ARDUINO(message)        
        
    def ExitProg(self):
        self.cancel()
        
    def ExitProg_with_save(self):
        self.cancel_with_save()    

    def saveLEDTabtoFile(self, filepath):
        saveDicttoFile(filepath, self.ledtable)

    def readLEDTabfromFile(self, filepath):
        LEDTab = readDictFromFile(filepath)
        if LEDTab:
            self.ledtable.clear()
            self.ledtable = LEDTab.copy()
            frame = self.tabdict["ColorCheckPage"]
            frame.ledhighlight = True
            frame._update_preview() 

    # ----------------------------------------------------------------
    #  cancel_with_save
    # ----------------------------------------------------------------
    def cancel_with_save(self):
        for key in self.tabdict:
            self.tabdict[key].cancel()
            
        self.setConfigData("pos_x", self.winfo_x())
        self.setConfigData("pos_y", self.winfo_y())
        self.SaveConfigData()
        self.SaveParamData()
        open(FINISH_FILE, 'a').close()                                       # 03.12.19:
        self.disconnect()
        self.continueCheckDisconnectFile = False
        self.destroy()

    # ----------------------------------------------------------------
    #  cancel_without_save
    # ----------------------------------------------------------------
    def cancel_without_save(self):
        for key in self.tabdict:
            self.tabdict[key].cancel()
        self.setConfigData("pos_x", self.winfo_x())
        self.setConfigData("pos_y", self.winfo_y())
        if self.ParamData.data == {}: # ParamData is not used, interface is config data. In this case do not save config data - compatibility modus
            self.SaveConfigData()
        open(FINISH_FILE, 'a').close()                                       # 03.12.19:
        self.disconnect()
        self.continueCheckDisconnectFile = False
        self.destroy()

    # ----------------------------------------------------------------
    #  cancel
    # ----------------------------------------------------------------
    def cancel(self):
        if self.paramDataChanged:
            answer = tk.messagebox.askyesnocancel ('Das Programm wird beendet','Soll die Palette (ColTab) gesichert werden?',default='no')
            if answer == None:
                return # no cancelation
            
            if answer:
                self.cancel_with_save() 
            else:
                self.cancel_without_save()
        else:
            self.cancel_without_save()

    # ----------------------------------------------------------------
    # show_frame byName
    # ----------------------------------------------------------------
    def showFramebyName(self, pageName):
        frame = self.tabdict[pageName]
        if pageName == "ColorCheckPage":
            frame._update_cor_rgb()
        self.container.select(self.tabdict[pageName])

    # ----------------------------------------------------------------
    # Event TabChanged
    # ----------------------------------------------------------------        
    def TabChanged(self,_event=None):
        tab_name = self.container.select()
        tab = self.nametowidget(tab_name)
        tab.tabselected()

    # ----------------------------------------------------------------
    # setParamDataChanged
    # ----------------------------------------------------------------        
    def setParamDataChanged(self):
        self.paramDataChanged=True
        
    # ----------------------------------------------------------------
    # LEDColorTest connect ARDUINO
    # ----------------------------------------------------------------
    def connect(self):

        print("connect")
        port = self.getConfigData("serportname")
        self.ARDUINO_message = ""
        self.set_connectstatusmessage(_("Nicht Verbunden"),fg="black")
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
                
                self.arduino = None
                return False
            
            timeout_error = True
            try:
                text=""
                text = self.arduino.readline()
                timeout_error = False
                # check if feedback is from MobaLedLib
                text_string = text.decode('utf-8')
                
                if text_string.find(SerialIF_teststring1) == -1:
                    read_error = True
                else:
                    self.queue.put(text_string)
                    logging.info("Connect message: %s", text_string)
            except IOError:
                read_error = True
                
            if read_error:         
                messagebox.showerror("Error when reading Arduino answer",
                                     "Wrong answer from ARDUINO:\n"                                  # 02.01.20:  Ausgabe der empfangenen Message. Das Hilft evtl. bei der Fehlersuche
                                     " '" + text_string + "'\n"                                      # 02.01.20:  ToDo: Woher kommt das 'b am Anfang der Meldung? das "\n\r" muss auch nicht angezeigt werden
                                     "\n"
                                     "Check if the correct program is loaded to the arduino.\n"
                                     "It must be compiled with the compiler switch:\n"               # 02.01.20:  Old: "becompiled"
                                     "  #define RECEIVE_LED_COLOR_PER_RS232")
                
                self.arduino = None
                return False                

            if timeout_error:         # 03.12.19: more detailed error message
                messagebox.showerror("Timeout waiting for the Arduino answer",
                                     "ARDUINO is not answering.\n"
                                     "\n"
                                     "Check if the correct program is loaded to the arduino.\n"
                                     "It must becompiled with the compiler switch:\n"
                                     "  #define RECEIVE_LED_COLOR_PER_RS232")
                
                self.arduino = None
                return False
            else:
                self.connection_startup = True
                self.connection_startup_time = 50 # 5 seconds time to send the correct messages
                self.connection_startup_answer = False
                for key in self.tabdict:
                    self.tabdict[key].connect()

                if self.getConfigData("startpage") == 1: # Just send the begin if the LED page shown at the start 
                    message = "#BEGIN\n"                                     # 04.12.19: New location 
                    self.send_to_ARDUINO(message)
                self.set_connectstatusmessage(_("Verbunden - Warten auf korrekte Antwort von ARDUINO ..."),fg="orange")
                return True
            return False
        else:
            return False

    # ----------------------------------------------------------------
    # LEDColorTest disconnect ARDUINO
    # ----------------------------------------------------------------
    def disconnect(self):
        self.led_off()    
        print("disconnect")
        if self.arduino:
            for key in self.tabdict:
                self.tabdict[key].disconnect()
            
            message = "#END\n"
            self.send_to_ARDUINO(message)

            self.arduino.close()
            self.arduino = None
            self.set_connectstatusmessage(_("Nicht Verbunden"),fg="black")

    def checkconnection(self):
        textmessage = ""
        if self.connection_startup:
            # checks if in the first messages from ARDUINO the string "#Color Test LED" is included
            self.connection_startup_time -= 1 # is called every 100ms
            if self.connection_startup_time == 0:
            
                messagebox.showerror("Error when reading Arduino answer",
                                     "ARDUINO is not answering correctly.\n"
                                     "\n"
                                     "Check if the correct program is loaded to the arduino.\n"
                                     "It must be compiled with the compiler switch:\n"
                                     "  #define RECEIVE_LED_COLOR_PER_RS232")
                self.disconnect()
            else:
                while self.queue.qsize():
                    #print("process_serial: While loop")
                    try:
                        readtext = self.queue.get()
                        date_time = datetime.now()
                        d = date_time.strftime("%H:%M:%S")
                        readtext_str = readtext
                        textmessage = d + "  " + readtext_str
                        if not textmessage.endswith("\n"):
                            textmessage = textmessage +"\n"
                        if SerialIF_teststring2 in textmessage:
                            self.connection_startup = False                           
                            self.set_connectstatusmessage(_("Verbunden"),fg="green")
                            colon_loc = readtext_str.find(":")
                            maxLEDcnt_str = readtext_str[colon_loc+1:-5]
                            self.set_maxLEDcnt(int(maxLEDcnt_str))
                            #self.led_off()                                # 02.01.20:  Disabled because it generates sometimes a "Buffer overrun" on the Arduino
                            #self.led_off()                                #            In addition it doesn't work with out a "#begin"
                            break
                    except:
                        pass
        return textmessage
        
        
    def set_connectstatusmessage(self,status_text,fg="black"):
        self.statusmessage.configure(text="Status: " + status_text,fg=fg)

    def set_ARDUINOmessage(self,status_text,fg="black"):
        #self.statusmessage.configure(text=status_text,fg=fg)
        pass

    def send_to_ARDUINO(self, message):
        if self.arduino:
            try:
                self.arduino.write(message.encode())
                print("Message send to ARDUINO: ",message)
                self.queue.put( ">> " + str(message))
            except:
                print("Error write message to ARDUINO")
    
    def set_maxLEDcnt(self,maxLEDcnt):
        self.maxLEDcnt = maxLEDcnt
        
    def get_maxLEDcnt(self):
        if self.maxLEDcnt:            
            return self.maxLEDcnt
        else:
            return self.getConfigData("maxLEDcount")
        
    # ----------------------------------------------------------------
    # getframebyName
    # ----------------------------------------------------------------
    def getFramebyName (self, pagename):
        return self.tabdict[pagename]
    
    # ----------------------------------------------------------------
    # getStartPageClassName
    # ----------------------------------------------------------------
    def getStartPageClassName (self):
        startpagenumber = self.getConfigData("startpage")
        startpagename = startpageNumber2Name.get(startpagenumber,self.ConfigData.data.get("startpagename","ColorCheckPage"))
        return startpagename
    
    # ----------------------------------------------------------------
    # getStartPageClassIndex
    # ----------------------------------------------------------------
    def getStartPageClassIndex (self):
        startpagenumber = self.getConfigData("startpage")
        startpagename = startpageNumber2Name.get(startpagenumber,self.ConfigData.data.get("startpagename","ColorCheckPage"))
        startpageindex = tabClassName2Index.get(startpagename,"1")
        return startpageindex
    # ----------------------------------------------------------------
    # getTabNameList
    # ----------------------------------------------------------------
    def getTabNameList (self):

        return list(tabClassName2Name.values())
    
    # ----------------------------------------------------------------
    # getStartPageName
    # ----------------------------------------------------------------
    def getStartPageName(self, combolist_index):
        return tabIndex2ClassName.get(str(combolist_index),defaultStartPage)

    def call_helppage(self,event=None):
        webbrowser.open_new_tab(HELPPAGE_FILENAME)

    def getConfigData(self, key):
        newkey = CONFIG2PARAMKEYS.get(key,key) # translate configdata key into paramdata key
        return self.ParamData.data.get(newkey,self.ConfigData.data.get(key,DEFAULT_CONFIG.get(key))) # return Paramdata for the key if available else ConfigData
    
    def readConfigData(self):
        filedir = os.path.dirname(os.path.realpath(__file__))
        self.ConfigData = ConfigFile(DEFAULT_CONFIG, CONFIG_FILENAME,filedir=filedir)
        self.ParamData = ConfigFile(DEFAULT_PARAM, PARAM_FILENAME,filedir=filedir)
        
    def setConfigData(self,key, value):
        self.ConfigData.data[key] = value
    
    def setParamData(self,key, value):
        if self.ParamData.data:
            self.ParamData.data[key] = value

    def SaveConfigData(self):
        self.ConfigData.save()
        
    def SaveParamData(self):
        if self.ParamData.data:
            self.ParamData.save()
        
    def led_off(self,_event=None):
    # switch off all LED
        message = "#L00 00 00 00 FF\n"
        self.send_to_ARDUINO(message)
        
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


    def ToolTip(self, widget,text="", key="",button_1=False):
        if text:
            tooltiptext = text
        else:
            tooltiptext = _(TOOLTIPLIST.get(key,key))
        if tooltiptext:
            Tooltip(widget, text=tooltiptext,button_1=button_1)

#-------------------------------------------

format = "%(asctime)s: %(message)s"
logging.basicConfig(format=format, level=logging.INFO,
                    datefmt="%H:%M:%S")

app = LEDColorTest()

app.protocol("WM_DELETE_WINDOW", app.cancel)

if app.getConfigData("autoconnect"):
    app.connect()


app.mainloop()


