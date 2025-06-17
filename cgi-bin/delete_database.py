#!/usr/bin/python3
import os
import json
import sys

json_file = '/home/welow/webserv/cgi-bin/data.json'

# Read input data
content_length_str = os.environ.get('CONTENT_LENGTH', '0')
content_length = int(content_length_str) if content_length_str.isdigit() else 0
input_data = sys.stdin.read(content_length) if content_length > 0 else '{}'

try:
    data = json.loads(input_data)
    msg_to_delete = data.get('delete', '')
except Exception:
    msg_to_delete = ''

# Read all messages
if os.path.exists(json_file):
    try:
        with open(json_file, 'r') as f:
            messages = json.load(f)
        if not isinstance(messages, list):
            messages = []
    except Exception:
        messages = []
else:
    messages = []

# Delete the message if it exists
if msg_to_delete in messages:
    messages.remove(msg_to_delete)
    with open(json_file, 'w') as f:
        json.dump(messages, f)
    result = f"Message '{msg_to_delete}' deleted successfully."
else:
    result = f"Message '{msg_to_delete}' not found."

# Output JSON
print("Content-Type: application/json\n")
print(json.dumps({
    "result": result,
    "database": messages
}))