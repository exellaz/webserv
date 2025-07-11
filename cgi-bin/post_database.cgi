#!/usr/bin/python3

import signal
import sys # call shell commands and read input data
import json # call json functions to handle JSON data
import os # call operating system functions to handle files and directories

def signal_handler(signum, frame):
    os._exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
json_file = os.path.join(SCRIPT_DIR, "data.json")

def save_message(message):
	# Load existing messages or start a new list
	if os.path.exists(json_file): # Check if the file exists
		try:
			with open(json_file, 'r') as f: # Open the file in read mode
				messagesList = json.load(f) # Load the JSON data
			if not isinstance(messagesList, list): # Check if the data is not a list
				messagesList = ["null"] # Initialize an empty list if the data is not a list
		except Exception: # Handle any exceptions that occur during loading
			messagesList = ["null"] # Return an empty list
	else: # If the file does not exist, start with an empty list
		messagesList = ["null"] # Initialize an empty list

	# Append the message you input to the list
	messagesList.append(message)

	# Save the updated list
	with open(json_file, 'w') as f: # Open the file in write mode
		json.dump(messagesList, f) # Save the new data to the json file

print("Content-Type: application/json\n")

content_length_str = os.environ.get('CONTENT_LENGTH', '0')
content_length = int(content_length_str) if content_length_str.isdigit() else 0
input_data = sys.stdin.read(content_length) if content_length > 0 else '{}' # Read the input data from stdin
try:
	data = json.loads(input_data) # Parse the input data as JSON
	message = data.get('message', 'null') # Get the 'message' field from the input data
	save_message(message) # Save the message to the database
	print(json.dumps({'message': message})) # Return the saved message as a JSON response
except Exception: # Handle any exceptions that occur during parsing or saving
	print(json.dumps({'message': 'ERROR'})) # Return an empty message if an error occurs