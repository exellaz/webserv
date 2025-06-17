#!/usr/bin/python3.12

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning) # Ignore deprecation warnings for CGI usage
import cgi
import os
import sys

print("Content-Type: text/plain\r\n")
print()

UPLOAD_DIR = "/home/welow/webserv/cgi-bin/storage"

form = cgi.FieldStorage()
if "file" not in form or not getattr(form["file"], "filename", None):
    print("No file uploaded.")
    sys.exit(0)
file_item = form["file"]
filename = os.path.basename(file_item.filename)
name, ext = os.path.splitext(filename)
counter = 1
new_filename = filename
filepath = os.path.join(UPLOAD_DIR, new_filename)

# Increment counter until a unique filename is found
while os.path.exists(filepath):
    new_filename = f"{name}({counter}){ext}"
    filepath = os.path.join(UPLOAD_DIR, new_filename)
    counter += 1

#with open(filepath, "wb") as f:
#    f.write(file_item.file.read())

# write and read in chunks to avoid memory issues (for large files)
with open(filepath, "wb") as f:
	while True:
		chunk = file_item.file.read(8192)  # Read in chunks to avoid memory issues
		if not chunk:
			break
		f.write(chunk)

print(f"File '{new_filename}' uploaded successfully!")