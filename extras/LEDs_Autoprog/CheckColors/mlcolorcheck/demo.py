import traceback, tkinter as tk, tkinter.ttk as ttk, tkinter.font as tkFont
import ToolTips

# demo GUI
try :
    # create the application window
    app_window = tk.Tk()
    app_window.title("Tooltip Demo")
    app_window.rowconfigure(0, weight=1)
    app_window.rowconfigure(1, weight=2)
    app_window.rowconfigure(3, weight=2)
    app_window.columnconfigure(0, weight=1)
    app_window.columnconfigure(2, weight=1)

    # local variables used to store pointers to the widgets we want to have tooltips on
    # and the tooltip text for those widgets
    # the index on both variables should match so that, for example, the first widget on the list
    # should have it's tooltip text as the first item on the other list
    widgets = []
    tooltip_text = []

    # create a custom font, used by the demo widgets and the tooltips
    font_obj = tkFont.Font(family="Courier", size=12)

    # add widgets to the window to demonstrate the tooltips at work
    # add a reference to each widget and it's tooltip text in the corresponding lists
    # making sure to match the keys of both lists

    # add a label with more information
    ttk.Label(app_window, text="Try resizing the window to see the changes to the tooltip!", font=font_obj).grid(row=0, column=0, columnspan=3, sticky="N,W")

    # NW corner
    widget = ttk.Label(app_window, text="Hover me!", font=font_obj)
    widget.grid(row=1, column=0, sticky="N,W")
    widgets.append(widget)
    tooltip_text.append("This is a tooltip that is very long and should not fit on the screen.\nAs such it will be split into several lines, with each line using as much width as possible!")
    # NE corner
    widget = ttk.Label(app_window, text="Hover me!", font=font_obj)
    widget.grid(row=1, column=2, sticky="N,E")
    widgets.append(widget)
    tooltip_text.append("This is a tooltip!")
    # SW corner
    widget = ttk.Label(app_window, text="Hover me!", font=font_obj)
    widget.grid(row=3, column=0, sticky="S,W")
    widgets.append(widget)
    tooltip_text.append("This is a tooltip!")
    # SE corner
    widget = ttk.Label(app_window, text="Hover me!", font=font_obj)
    widget.grid(row=3, column=2, sticky="S,E")
    widgets.append(widget)
    tooltip_text.append("This is a tooltip that is very long and should not fit on the screen. As such it will be split into several lines, with each line using as much width as possible!")

    # center
    widget = ttk.Label(app_window, text="Hover me!", font=font_obj)
    widget.grid(row=2, column=1, sticky="S,E")
    widgets.append(widget)
    tooltip_text.append("This is a really big tooltip text that will not fit inside the window at the default position and thus will need to be broken into several lines and will need to breack the anchor restriction as well as reducing the font size")

    # instantiate the ToolTips class, providing the list of widgets, tooltip strings and font (optional parameter)
    tooltip_obj = ToolTips.ToolTips(widgets, tooltip_text, font_obj)

    # start the GUI event listener loop
    app_window.mainloop()
except Exception as e :
    traceback.print_exc()
    print("\n")
