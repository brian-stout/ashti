#!/usr/bin/env python3
import os.path


"""
    time_string() is a function that given a certain amount of seconds, divides those
        seconds into chunks of hours, minutes, and seconds, then returns the string
"""

def time_string(seconds):
    hours = 0
    minutes = 0

    if seconds >= 3600:
        hours = seconds // 3600
        seconds -= hours * 3600

    if seconds >= 60:
        minutes = seconds // 60
        seconds -= minutes * 60

    return_string = str(hours) + "h " + str(minutes) + "m " + str(seconds) + "s "
    return return_string


"""
    generate_img_tag() when given a src name generate an image tag based on the parameters
        provided to it.  The function first looks to see if the image exists in the
        profile_pictures directory and if it isn't generates a tag to a default image.

        For now, generate_img_tag only supports .jpgs.
"""

def generate_img_tag(src, alt='0', width="200px", height="200px"):

    if alt[0] == '0':
        alt = src

    src += ".jpg"
    path = "./profile_pictures/" + src

    if(os.path.isfile(path)):
        print_string = "<img src='/profile_pictures/" + src + "' alt='" + alt
        print_string += "' style='width:" + width + ";height=" + height + ";'>"
    else:
        print_string = "<img src='/profile_pictures/question-mark.jpg' alt='" + alt
        print_string += "' style='width:25%;height:25%;'> "

    print(print_string)
