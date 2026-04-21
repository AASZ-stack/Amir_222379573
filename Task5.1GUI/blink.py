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
selected_room_light = StringVar() 
selected_room_light.set("none")

#Function
def selectLivingRoom():
    living_room_led.on()
    bathroom_led.off()
    closet_led.off()

def selectBathroom():
    bathroom_led.on()
    living_room_led.off()
    closet_led.off()

def selectCloset():
    closet_led.on()
    living_room_led.off()
    bathroom_led.off()

def close():
    living_room_led.off()
    bathroom_led.off()
    closet_led.off()
    RPi.GPIO.cleanup()
    win.destroy()

##Buttons
###Living room button
livingRoomButton = Radiobutton(
    win, 
    text = "Living room light",
    font = myFont,
    variable = selected_room_light,
    value = "livingroom",
    command = selectLivingRoom,
    height = 1,
    width = 24,
)
livingRoomButton.grid(row = 0, column = 1)

###Bathroom button
bathroomButton = Radiobutton(
    win, 
    text = "Bathroom light", 
    font = myFont, 
    variable = selected_room_light,
    value = "bathroom",
    command = selectBathroom,  
    height = 1, 
    width = 24
) 
bathroomButton.grid(row = 2, column = 1)

###Closet button
closetButton = Radiobutton(
    win, 
    text = "Closet light", 
    font = myFont, 
    variable = selected_room_light,
    value = "closet",
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