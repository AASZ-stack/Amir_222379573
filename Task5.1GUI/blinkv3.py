#Libraries 
from tkinter import *
import tkinter.font
from gpiozero import LED
import RPi.GPIO
RPi.GPIO.setmode(RPi.GPIO.BCM)

#Hardware
##Blue LED / connected to GPIO 17
living_room_led = LED(17)
##Yellow LED / connected to GPIO 27
bathroom_led = LED(27)
##Red LED / connected to GPIO 22
closet_led = LED(22) 

#GUI
win = Tk()
win.title("Linda's house lighting control")
myFont = tkinter.font.Font(family = 'Arial', size = 15, weight = "bold")
living_room_state = BooleanVar()
bathroom_state = BooleanVar()
closet_state = BooleanVar()

#Function
def selectLivingRoom():
    if living_room_led.is_lit:
        living_room_led.off()
    else:
        living_room_led.on()

def selectBathroom():
    if bathroom_led.is_lit:
        bathroom_led.off()
    else:
        bathroom_led.on()

def selectCloset():
    if closet_led.is_lit:
        closet_led.off()
    else:
        closet_led.on()

def close():
    living_room_led.off()
    bathroom_led.off()
    closet_led.off()
    RPi.GPIO.cleanup()
    win.destroy()

##Buttons
###Living room button
livingRoomButton = Checkbutton(
    win, 
    text = "Living room light",
    font = myFont,
    variable = living_room_state,
    command = selectLivingRoom,
    height = 1,
    width = 24,
)
livingRoomButton.grid(row = 0, column = 1)

###Bathroom button
bathroomButton = Checkbutton(
    win, 
    text = "Bathroom light", 
    font = myFont, 
    variable = bathroom_state,
    command = selectBathroom,  
    height = 1, 
    width = 24
) 
bathroomButton.grid(row = 2, column = 1)

###Closet button
closetButton = Checkbutton(
    win, 
    text = "Closet light", 
    font = myFont, 
    variable = closet_state,
    command = selectCloset,
    height = 1, 
    width = 24
)
closetButton.grid(row = 3, column = 1)

###Exist button
exitButton = Button(
    win, 
    text = "Exit", 
    font = myFont, 
    command = close, 
    bg = 'red', 
    height = 1, 
    width = 12
)
exitButton.grid(row = 4, column = 1)

win.protocol("WM_DELETE_WINDOW", close) #for LEDS to turn off if user closes the window with x instead of exit
win.mainloop() #loop GUI