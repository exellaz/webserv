#!/usr/bin/python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning) # Ignore deprecation warnings for CGI usage
import cgi  # this library available in Python 3.12 (to handle CGI form data)
import os

# Parse query parameters
form = cgi.FieldStorage()  # Create an instance of FieldStorage to handle form data
name = form.getvalue("name", "world")  # Default to "world" if no name is provided
# Check if the script is running in a CGI environment
request_method = os.environ.get("REQUEST_METHOD")  # Get the request method
query_string = os.environ.get("QUERY_STRING")  # Get the query string
script_name = os.environ.get("SCRIPT_NAME")  # Get the script name

# Return headers and HTML
# if request_method != "GET":
#     print("Status: 405 Method Not Allowed")
#     print("Content-Type: text/plain\r\n")
#     print("\r\n")
#     print("Method Not Allowed")
#     exit()
print("Content-Type: text/html\r\n")
print("\r\n")
print("<html><body>")
print("<h1>Hello, %s!</h1>" % name)
print("<p><b>Request Method:</b> %s</p>" % request_method)
print("<p><b>Query String:</b> %s</p>" % query_string)
print("<p><b>Script Name:</b> %s</p>" % script_name)
print("</body></html>")