#!/usr/bin/python3
# filepath: /home/welow/cgi-bin/get_database.py

import json # call json functions to handle JSON data
import os # call operating system functions to handle files and directories

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
json_file = os.path.join(SCRIPT_DIR, "data.json")

def load_message():
	if not os.path.exists(json_file): # Check if the file exists
		return []
	with open(json_file, 'r') as f: # Open the file in read mode
		try:
			data = json.load(f) # Load the JSON data
			if isinstance(data, list): # Check if the data is a list
				return data # Return the list of messages
			else:
				return [] # Return an empty list if the data is not a list
		except Exception: # Handle any exceptions that occur during loading
			return [] # Return an empty list if an error occurs

print("Content-Type: application/json\n")

messages = load_message()
print(json.dumps({'messages': messages})) #json.dumps() converts the Python object to a JSON string
#example output: {"messages": [{"name": "John", "message": "Hello, world!"}, {"name": "Jane", "message": "Hi there!"}]}