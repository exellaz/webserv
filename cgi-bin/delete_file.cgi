#!/usr/bin/python3.12

import os
import json
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning) # ignore deprecation warnings
import cgi

UPLOAD_DIR = "/home/welow/webserv-test/webserv/cgi-bin/storage"

print("Content-Type: application/json\r\n")

form = cgi.FieldStorage()
filename = form.getfirst("filename") # get the filename from the form data

if not filename: # check if filename is provided
    print(json.dumps({"error": "Missing filename"}))
    exit()

safe_name = os.path.basename(filename)  # prevent path injection
target_path = os.path.join(UPLOAD_DIR, safe_name) # construct the full path to the file

if os.path.exists(target_path): # check if the file exists
    try:
        os.remove(target_path) # delete the file
        print(json.dumps({"status": "success", "deleted": safe_name}))
    except Exception as e:
        print(json.dumps({"error": str(e)}))
else: # if the file does not exist
    print(json.dumps({"error": "File not found"}))
