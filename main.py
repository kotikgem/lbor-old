import gd
import os
import time
import keyboard
import json
from pynput.mouse import Listener, Button
from numba import njit, prange

njit(fastmath=True, parallel=True)

memory = gd.memory.get_memory()

que = input("record or play?")

def record():
	valuess = []
	print("start!")
	while True:
		while memory.is_in_level() == True:
			xpos = memory.get_x_pos()
			ypos = memory.get_y_pos()
			valuess.append((xpos, ypos))
			if keyboard.is_pressed("ctrl"):
				quitt = True
				break
		if quitt == True:
			break
	f = open('macro_break_physics.json', 'wb')
	new_json = {
		'macro': tuple(valuess)
	}
	# i = 0
	# while i != len(valuess):
	# 	f.write(str(valuess[i]) + '\n')
	# 	i = i + 1
	f.write(json.dumps(new_json).encode())
	f.close()
	print("ende")



def play():
	print("start!")
	valuess = []
	# f = open('macro_break_physics.txt', 'r')
	# i = 0
	# while i != len(valuess):
	# 	f.write(valuess[i] + '\n')
	# 	i++
	f= open('macro_break_physics.json', 'rb')
	valuess = json.loads(f.read().decode())['macro']
	i = 0
	quitt = False
	keyboard.send("space")
	while True:
		while memory.is_in_level() == True:
			while memory.x_pos >= valuess[i][0]:
				xpos = memory.get_x_pos()
				ypos = memory.get_y_pos()
				# print((xpos, ypos))
				# if [xpos, ypos] in valuess:
				# 	pos = valuess.index((xpos, ypos))
				memory.set_y_pos(valuess[i][1])
				print(valuess[i][1])
				i = i + 1
				if keyboard.is_pressed("ctrl"):
					quitt = True
					break
			if quitt == True:
				break
		if quitt == True:
				break
	print("ende")

if(que == "record"):
	record()
if(que == "play"):
	play()