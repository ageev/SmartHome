#!/usr/bin/env python
# """Working with Yeelight"""
#

# Imports
import os
import sys
import socket
import json


# Module Constants
START_MESSAGE = "Yeelight WakeUp script"
PORT = 55443 

HOST = "192.168." 



# power = '{"id": 1, "method": "set_power", "params": ["on", "smooth", 350]}'
# off = '{"id": 1, "method": "set_power", "params": ["off", "smooth", 350]}'
# bright = '{"id": 1, "method": "set_bright", "params": [1, "smooth", 350]}'
# tempr = '{"id": 1, "method": "set_ct_abx", "params": [2700, "smooth", 350]}' #2700-6500
# bg_power = '{"id": 1, "method": "bg_set_power", "params": ["on", "smooth", 350]}'
# bg_bright = '{"id": 1, "method": "bg_set_bright", "params": [1, "smooth", 350]}'
# bg_colour = '{"id": 1, "method": "bg_set_rgb", "params": [14423100, "smooth", 350]}' #RGB color green - 65280, blue - 255, red 16711680. Gray RGB code = 128*65536+128*256+128
# {"id":1,"method":"adjust_bright","params":[-20, 500]}

# Module "Global" Variables
location = os.path.abspath(__file__)

def send_command_to_device(cmd, HOST, PORT):
# Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall(bytes(cmd, "utf-8")+b'\x0d\x0a')

        # Receive data from the server and shut down
        received = str(sock.recv(1024), "utf-8")

    print("Sent:     {}".format(cmd))
    print("Received: {}".format(received))

def get_cmd_json(param1, param2):
    command = {}
    command['id'] = 1
    command['method'] = param1
    command['params'] = param2
    return json.dumps(command)

# Module Functions and Classes
def main(*args):
    """My main script function.
    Displays the full patch to this script, and a list of the arguments passed
    to the script.
    """

    print(START_MESSAGE)
    print("Script Location:", location)
    print("Arguments Passed:", args)

    duration = 3000

    send_command_to_device(get_cmd_json("set_power", ["on", "sudden", 1]), HOST, PORT)
    send_command_to_device(get_cmd_json("set_bright", [1, "sudden", 1]), HOST, PORT)
    send_command_to_device(get_cmd_json("set_ct_abx", [2700, "sudden", 1]), HOST, PORT)
    send_command_to_device(get_cmd_json("bg_set_bright", [1, "sudden", 1]), HOST, PORT)
    send_command_to_device(get_cmd_json("adjust_bright", [100, duration]), HOST, PORT)
    send_command_to_device(get_cmd_json("bg_adjust_bright", [100, duration]), HOST, PORT)
    # send_command_to_device(get_cmd_json("set_ct_abx", [6500, "smooth", duration]), HOST, PORT)




    cmd_json = get_cmd_json

    {"id":1,"method":"adjust_bright","params":[-20, 500]}


# Check to see if this file is the "__main__" script being executed
if __name__ == '__main__':
    _, *script_args = sys.argv
    main(*script_args)
