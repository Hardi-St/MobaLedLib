# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
#         LEDListPage
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
from mlcolorcheck.configfile import ConfigFile

from mlcolorcheck.DefaultConstants import COLORCOR_MAX, DEFAULT_PALETTE, LARGE_FONT, SMALL_FONT, VERY_LARGE_FONT, VERSION, PERCENT_BRIGHTNESS
from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
    overlay, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv, convert_K_to_RGB

from locale import getdefaultlocale

SCROLLHEIGHT = 500 # pixel

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

class LEDListPage(tk.LabelFrame):
    
    def __init__(self, parent, controller):
        self.controller = controller
        self.parent = parent
        self.tabname = "LED Liste"
        self.tabClassName = "LEDListPage"
        
        self.cor_red = 255
        self.cor_green = 255
        self.cor_blue = 255
        
        tk.Frame.__init__(self, parent)
        self.create_content(parent,controller)


    def create_content(self, parent, controller):
        # widget definitions ===================================
        self.canvas = Canvas_1(self, parent, controller)
        self.canvas.grid(row=0, sticky='nesw', column=1)
        scrollbar = tk.Scrollbar(self)
        scrollbar.grid(row=0, sticky='ns')

        self.canvas.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.canvas.yview)


    def tabselected(self):
        self.controller.currentTabClass = self.tabClassName
        print(self.tabname)
        self.canvas.delete()
        self.create_content(self.parent, self.controller)
        pass        

    def getConfigData(self, key):
        return self.controller.getConfigData(key)
    
    def readConfigData(self):
        self.controller.readConfigData()
        
    def setConfigData(self,key, value):
        self.controller.setConfigData(key, value)

    def setParamData(self,key, value):
        self.controller.setParamData(key, value)

    def cancel(self):
        pass
        
    def connect (self):
        pass
    
    def disconnect (self):
        pass

    def MenuUndo(self,_event=None):
        pass
    
    def MenuRedo(self,_event=None):
        pass



class Canvas_1(tk.Canvas):

    def __init__(self, master, parent, controller):
        self.controller = controller
        tk.Canvas.__init__(self,master)
        # widget definitions ===================================
        self.frame = Frame_1(self,master,controller)
        coords = (0,0)
        item = self.create_window(*coords)
        self.itemconfig(item,anchor = 'nw',window = self.frame)


class Frame_1(tk.Frame):

    def __init__(self,master,parent, controller):
        tk.Frame.__init__(self,master)
        self.controller = controller
        
        self.cor_red = self.getConfigData("led_correction_r")
        self.cor_green = self.getConfigData("led_correction_g")
        self.cor_blue = self.getConfigData("led_correction_b")   
        # widget definitions ===================================

        self.canvas = self.master
        self.canvas.bind('<Configure>',self.canvas_configure)
        self.bind('<Configure>',self.frame_configure)

        # Daten zum Anzeigen === dafür dann Tabelle reisetzen
        tk.Label(self,text='LED Liste (noch in Bearbeitung)',font=LARGE_FONT).grid(row=0,columnspan=4, sticky="e")

        maxwidth=0
        for value in self.master.config().values():
            maxwidth = max(maxwidth,len(str(value)))

        row = 1
        for key in sorted(self.controller.ledtable):
            tk.Label(self,text="LED " + key).grid(row=row,column=0,sticky='e')
           
            keycolor = self.controller.ledtable.get(key)
            #tk.Label(self,text=keycolor).grid(row=row,column=1,sticky='e')
            
            color_disp,brightness_text = self.keycolor_to_dispcolor(keycolor)
            fontcolor = "#000000"
            
            r,g,b = hexa_to_rgb(keycolor)
            text =  "("+'{:03}'.format(r)+","+'{:03}'.format(g)+","+'{:03}'.format(b)+")" + "\n" + brightness_text
            f = ttk.Frame(self, borderwidth=1, relief="raised", style="palette.TFrame")
            l = tk.Label(f, background=color_disp, width=12, height=2,text=text,fg=fontcolor)            
            l.pack()
            f.grid(row=row, column=1, padx=2, pady=2,sticky="e")            
            
            #entry = tk.Entry(self,width=maxwidth)
            #entry.grid(row=row,column=1)
            #entry.insert(0,value)
            row +=1

        # ====================================
    def getConfigData(self, key):
        return self.controller.getConfigData(key)


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
 
        
        
        
        
    # was man auch mit dem Canvas tut, er muß sich immer an die Breite des Frame anpassen - an die tatsächliche Breite
    def canvas_configure(self,event=None):
        if self.canvas['width'] != self.winfo_width():
            self.canvas['width'] = self.winfo_width()

    # hier ist der layoutmanager noch nicht fertig, daher den canvas an die vorraussichtliche Größe des frames anpassen
    def frame_configure(self,event=None):
        # Scroll region des canvas so groß wie man für den Frame braucht
        self.canvas.config(scrollregion="0 0 %s %s" % (self.winfo_reqwidth(), self.winfo_reqheight()))
        # breite des canvas so breit wie man für den frame braucht
        self.canvas.config(width=self.winfo_reqwidth())
        # wenn die benötigte frame höhe größer als SCROLLHEIGHT pixel ist, dann canvas höhe auf SCROLLHEIGHT pixel beschränken
        if self.winfo_reqheight() > SCROLLHEIGHT:
            self.canvas.config(height=SCROLLHEIGHT)
        # sonst canvas höhe so hoch wie man für den frame braucht
        else:
            self.canvas.config(height=self.winfo_reqheight())
            
