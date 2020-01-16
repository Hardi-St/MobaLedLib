# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# 															 #
# Python ToolTips for Tkinter v1.0.0						 #
# 															 #
# Copyright 2016, PedroHenriques 							 #
# http://www.pedrojhenriques.com 							 #
# https://github.com/PedroHenriques 						 #
# 															 #
# Free to use under the MIT license.			 			 #
# http://www.opensource.org/licenses/mit-license.php 		 #
# 															 #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

import math, tkinter as tk, tkinter.ttk as ttk, tkinter.font as tkFont

class ToolTips :
    """This class will display a tooltip around a widget that is being hovered over.
    The constructor receives a list of widgets, a list of strings and an optional Tkinter font object.
    The lists should match their indexes so that the tooltip string for the first widget is in index zero, and so on.

    There are several class variables that can be used to customize the styling of the tooltips.
    """

    # class variables with the fallback font information, to be used in the tooltip text
    tt_fallback_font_family_ = "Helvetica"
    tt_fallback_font_size_ = 12

    # class variables with the background and foreground colors
    bg_color_ = "#ffffe0"
    fg_color_ = "#000000"

    # class variables used to control the vertical space between the tooltip and the event widget
    vertical_margin_ = 0

    def __init__(self, widgets, tooltip_text, font=None) :
        # check if the 2 lists have the same number of items, and if not raise an exception
        if (len(widgets) != len(tooltip_text)) :
            raise ValueError("The number of widgets supplied does not match the number of tooltip strings provided.")

        # instance variable pointing to a list of widgets to be managed by this instance
        self.widgets = widgets

        # instance variable pointing to a list of strings to be used by the supplied widgets
        self.tooltip_text = tooltip_text

        # instance variable to flag whether a font was supplied or not
        if (font == None) :
            self.font_supplied = False
        else :
            self.font_supplied = True

        # instance variable pointing to a font object, to be used on the tooltip text
        self.font = font

        # instance variable with the font object for the tooltip text
        # starts with the fallback font
        self.tt_font = tkFont.Font(family=self.tt_fallback_font_family_, size=self.tt_fallback_font_size_)

        # loop through each widget and set the necessary binds
        for widget in self.widgets :
            # set the binds
            self.setWidgetBinds(widget)

        # instance variable where the tooltip widget will be stored
        self.tt_widget = None
        # instance variable where the tooltip's text will be stored
        self.tt_text = ""

    # this method will set the <Enter>, <Leave> and <Button-1> binds to the widgets related to this instance
    def setWidgetBinds(self, widget) :
        widget.bind("<Enter>", self.showToolTips, add="+")
        widget.bind("<Leave>", self.hideToolTips, add="+")
        widget.bind("<Button-1>", self.hideToolTips, add="+")

    # this method will be called when widgets with tooltips are hovered over
    def showToolTips(self, event) :
        # get a reference to the event widget
        widget_ref = event.widget

        # check if we were able to grab a pointer to a widget and that we have that widget in
        # widget list supplied to the constructor
        if (widget_ref == None or widget_ref not in self.widgets) :
            # either we couldn't grab a pointer to the event widget
            # or that widget is not in the list of widgets provided
            # to the constructor, so bail out
            print("The hovered widget is not in the list of widgets provided to this instance.")
            return(False)

        # grab this widget's tooltip text from the list
        try :
            self.tt_text = self.tooltip_text[self.widgets.index(widget_ref)]
        except (IndexError, ValueError) :
            # either widget_ref couldn't be found in self.widgets
            # or the tooltip text couldn't be found for the widget's index, so bail out
            print("An error occured while trying to find the tooltip text for the hovered widget.")
            return(False)

        # grab the event widget's top master (will be used for both measuring the position of the tooltip
        # and will be the direct master for the tooltip)
        top_master = widget_ref.winfo_toplevel()

        # grab the event widget's current width and height
        widget_width = widget_ref.winfo_width()
        widget_height = widget_ref.winfo_height()

        # local variables used to position the tooltip
        # starting at the NW corner of the event widget
        x = widget_ref.winfo_x()
        y = widget_ref.winfo_y()

        # loop through all the masters of the event widget, until we reach top_master
        # for each master, add it's x and y relative to it's master
        # by the end, we'll have the x and y coords of the event widget in relation to top_master
        w_master = top_master.nametowidget(widget_ref.winfo_parent()) # event widget's master
        while w_master != top_master :
            # update the x and y coords of the
            x += w_master.winfo_x()
            y += w_master.winfo_y()

            # grab next master in the hierarchy
            w_master = top_master.nametowidget(w_master.winfo_parent())

        # local variables used to store the final position of the tooltip
        # we'll start with the values of x and y, but might be changed in the loop below
        final_x = x
        final_y = y

        # create the tooltip font based on the event widget's font
        self.setFont(widget_ref)

        # create the tooltip label widget and initial width + height
        self.handleTooltipWidget(top_master)

        # grab top master's width (index 2) and height (index 3), through it's bbox data
        tm_bbox = top_master.bbox()

        # local control variables
        update_x = False
        found_tt_place = False

        # calculate the maximum x coordinate for the tooltip, which is the lowest between
        # the window width and the east border of the event widget
        # this is relevant, for example, in the case where the event widget is partially outside the window
        # (by using a horizontal scrollbar), meaning that the event widget's east border is outisde the window
        # and thus we can't anchor the tooltip to it, but rather to the widnow's east border
        max_x = min(x + widget_width, tm_bbox[2])

        # calculate the minimum x coordinate for the tooltip, which starts on the event widget's W border
        min_x = x

        while (not found_tt_place and self.tt_font_size > 1) :
            # by default we assume we'll find a position for the tooltip on this iteration
            found_tt_place = True

            # at this point, the tooltip's NW corner matches the event widget's NW corner
            # check if there's enough width in top_master to fit the tooltip in the default horizontal position
            # i.e., growing to the right
            if (min_x + self.tt_width > tm_bbox[2]) :
                # there isn't enough width to fit the tooltip
                # check if we can place the tooltip to the left of the event widget
                # i.e., make the E border of the tooltip match the E border of the event widget
                if (max_x - self.tt_width >= 0) :
                    # it fits to the left
                    # make the tooltip's E border match the event widget's E border
                    final_x = max_x - self.tt_width
                else :
                    # we need to add line breaks to the tooltip's text
                    # start by checking which side has more width to work with
                    if (min_x/tm_bbox[2] <= 0.5) :
                        # the tooltip is in the left half of the top_master, so there is more width to the right of the event widget
                        # set the tooltip to go as far left as the window's E border
                        max_x = tm_bbox[2]
                    else :
                        # the tooltip is in the right half of the top_master, so there is more width to the left of the event widget
                        # set the tooltip to go as far right as the window's W border
                        min_x = 0
                        # flag as needing to update the tooltip's X position after all the text changes
                        update_x = True

                    # calculate the tooltip's maximum width
                    tt_max_width = max_x - min_x

                    # calculate the len of the tooltip text
                    tt_text_len = len(self.tt_text)
                    # calculate the average width of each character, for the font in use by this widget
                    tt_char_width = self.tt_width / tt_text_len
                    # calculate the index increment for the loop below, rounded down ==> #characters per line
                    # NOTE: by using math.ceil(tt_char_width) it greatly reduces the chance of the tooltip overflowing the window, but it also leads to some space not being used by the tooltip (a safety margin).
                    #		use just tt_char_width to maximize space usage...but in some cases it will lead to overflows.
                    #		This is due to the calculations using an average pixels per character, which in variable width fonts is not 100% accurate. In fixed width fonts this should not be an issue.
                    increment = math.floor(tt_max_width / math.ceil(tt_char_width))
                    # first slice will be from index 0 to the 1st \n index
                    start_index = 0
                    end_index = increment
                    # variable with the new tooltip text
                    new_tt_text = ""

                    # loop through the tooltip text and add the \n when needed
                    while end_index < tt_text_len :
                        # grab this iteration's slice of the tooltip text
                        tt_text_slice = self.tt_text[start_index:end_index]

                        # check if there are any \n added by the developer in this slice
                        if ("\n" in tt_text_slice) :
                            # split the slice into sub slices, based on \n
                            tt_sub_slices = tt_text_slice.split("\n")

                            # if there are more than 1 sub slices and the last one is not empty
                            # i.e., the slice doesn't end with a \n, then don't run the last sub slice
                            # now, since it doesn't end with a manual \n and one will be added if we run it now
                            if (len(tt_sub_slices) > 1 and tt_sub_slices[-1] != "") :
                                tt_sub_slices = tt_sub_slices[:-1]

                            # reset the end index value, since we might not reach the end of this slice
                            end_index = start_index

                            # loop through each sub slice and if it doesn't exceed the max character number
                            # for the available width, add it to the tooltip text as is
                            # ends when all sub slices are dealt with OR we find one that overflows the available width
                            # in which case we'll process it on the next iteration of the parent loop
                            for tt_sub_slice in tt_sub_slices :
                                # check if it overflows and if it does BREAK
                                if (len(tt_sub_slice) > increment) :
                                    break

                                # at this point this sub slice doesn't overflow
                                # add it to the tooltip text, plus the \n
                                new_tt_text += tt_sub_slice + "\n"

                                # advance the end index value by the sub slice's len + 1 for the \n
                                end_index += len(tt_sub_slice) + 1
                        else :
                            # there are no manual \n to deal with in this slice
                            new_tt_text += tt_text_slice

                            # if the next character in the source string is a \n, then don't add this iteration's
                            # automatic \n, else add it. This avoids ending up with 2 \n in a row, when only 1 was intended
                            if ("\n" not in self.tt_text[end_index:end_index+1]) :
                                # add this iteration's \n
                                new_tt_text += "\n"

                        # advance to the index values
                        start_index = end_index
                        end_index += increment

                    # if there is any remainder text, add it to the tooltip text
                    # else, the tooltip text will end on a \n which needs to be removed
                    if (start_index != tt_text_len - 1) :
                        # the last index added to the tooltip's text was not the final index, so
                        # add the final slice of the tooltip text
                        new_tt_text += self.tt_text[start_index:tt_text_len]
                    else :
                        # there is no more text to add, so remove the ending \n
                        new_tt_text = new_tt_text[:-1]

                    # update the label's text
                    self.tt_widget.configure(text=new_tt_text)
                    # update the tooltip's width and height
                    self.tt_width = self.tt_widget.winfo_reqwidth()
                    self.tt_height = self.tt_widget.winfo_reqheight()
            else :
                final_x = min_x

            # at this point if the tooltip has more than 1 line, it is using as much width as possible,
            # taking into account the safety margin for variable width fonts

            # check if there's enough height in top_master to fit the tooltip in the default vertical position
            # i.e., below the event widget
            if (y + widget_height + self.vertical_margin_ + self.tt_height > tm_bbox[3]) :
                # there isn't enough height to fit the tooltip
                # check if we can place the tooltip to the top of the event widget
                if (y - self.tt_height - self.vertical_margin_ >= 0) :
                    # it fits to the top
                    # move the tooltip up
                    final_y = y - self.tt_height - self.vertical_margin_
                else :
                    # it doesn't fit below or above the event widget

                    # start by removing the default behaviour of having the tooltip anchored to the event widget's
                    # east border, making the tooltip go from the window's W to the E border
                    # if that isn't enough start reducing the font size used by the tooltip text by 1 point per loop iteration
                    # the loop will end when we reach a situation where the tooltip fits on the screen OR the font
                    # size goes all the way down to 1 point, at which point we conclude that there is no way to
                    # fit the tooltip on the screen given the text in it, the event widget's position on the window
                    # and the window's size

                    # if we're already using the entire window's width, then work the font size
                    if (max_x == tm_bbox[2] and min_x == 0) :
                        # update the tooltip's font to reduce the font size by 1 point
                        self.tt_font_size -= 1
                        self.tt_font.configure(size = max(self.tt_font_size, 1))

                        # recalculate the requested width and height with the new font size
                        self.handleTooltipWidget(top_master)
                    else :
                        # update the max_x to go all the way to the window's E border
                        # instead of the event widget's E border
                        max_x = tm_bbox[2]
                        min_x = 0

                    # signal the loop that we didn't find a position for the tooltip yet
                    found_tt_place = False
            else :
                # there is enough height below the event widget to fit the tooltip
                # so move the tooltip down
                final_y = y + widget_height + self.vertical_margin_

            # if the tooltip text was changed such that the width changed, then recalculate the
            # tooltip's X position
            if (update_x) :
                # move the tooltip to have it's E border matching the event widget's E border
                # or the window's E border, depending on the case
                final_x = max_x - self.tt_width

        # draw the tooltip
        self.tt_widget.place(relx=final_x/tm_bbox[2], rely=final_y/tm_bbox[3])

    # this method will be called when widgets with tooltips sto being hovered over
    # or "entered" by any other means (ex: tab selecting)
    def hideToolTips(self, event) :
        # if there are no active tooltips bail out
        if (self.tt_widget == None) :
            return

        # remove the tooltip from the screen and any other tkinter internal references
        self.tt_widget.destroy()

        # free the variables
        self.tt_widget = None
        self.tt_text = ""

    # this method will create the tooltip widget, if one doesn't exist already, and will calculate
    # the required width and height of the tooltip text without any line breaks, which is used to later calculate
    # the average character width for the given font and the given tooltip text
    def handleTooltipWidget(self, top_master) :
        # if the tooltip widget doesn't exist create it, otherwise update the dimensions
        if (self.tt_widget == None) :
            # create the tooltip label widget
            # NOTE: we start with an altered version of the intended text, because of the need to calculate the required width
            #		of the tooltip widget without any line breaks. If they are left in that calculation it will skew the
            #		the calculation (later below) of the average #characters in each line that fit the window size.
            #		The correct text will be added shortly below.
            self.tt_widget = ttk.Label(top_master, text=self.tt_text.replace("\n", " "), background=self.bg_color_, foreground=self.fg_color_, font=self.tt_font)
        else :
            # update the tooltip's text to recalculate the requested dimensions below
            self.tt_widget.configure(text=self.tt_text.replace("\n", " "))

        # move the tooltip from being on top of the event widget to a reasonable relative location
        # by default the tooltip's NW corner will be close to the event widget's SW corner
        # however this might need to be adjusted, if it would make the tooltip overflow the program's window
        # calculate the width and height of the tooltip (using the required dimension since it hasn't been drawn to screen yet, meaning there are no actual dimensions yet)
        self.tt_width = self.tt_widget.winfo_reqwidth()
        self.tt_height = self.tt_widget.winfo_reqheight()

        # now that we have calculated the initial required dimensions, we can update the widget's text to the intended one
        self.tt_widget.configure(text=self.tt_text)

    # this method creates the font based on the event widget's font family and size
    def setFont(self, event_widget) :
        # grab the event widget's font info
        try :
            if (self.font_supplied) :
                # a font was supplied to the constructor, so create a copy of it's current values
                self.tt_font["family"] = self.font["family"]
                self.tt_font["size"] = self.font["size"]
            else :
                # a font wasn't supplied to the constructor, try getting the event widget's font
                ew_font_info = event_widget["font"].actual()
                self.tt_font["family"] = ew_font_info["family"]
                self.tt_font["size"] = ew_font_info["size"]
        except Exception :
            # we couldn't create a tkFont object
            # most likely the event widget is using a custom tkFont object which can't be accessed and edited from here
            # or the event widget doesn't have a "font" attribute
            # use the fallback font
            self.tt_font["family"] = self.tt_fallback_font_family_
            self.tt_font["size"] = self.tt_fallback_font_size_

        # grab the tt_font's font size
        self.tt_font_size = self.tt_font["size"]
