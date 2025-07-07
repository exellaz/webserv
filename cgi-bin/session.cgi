#!/usr/bin/python3

import os
import sys
import urllib.parse

def parse_form():
    try:
        length = int(os.environ.get("CONTENT_LENGTH", 0))
        data = sys.stdin.read(length)
        return urllib.parse.parse_qs(data)
    except:
        return {}

def main():
    form = parse_form()
    headers = []
    body = ""

    # Example: Update session data
    if "data" in form:
        value = form["data"][0]
        headers.append("X-Session-Update: " + value)
        body = f"<p>Session updated to: {value}</p>"

    # Example: Delete session
    elif "delete" in form:
        headers.append("X-Session-Delete: yes")
        body = "<p>Session deleted.</p>"

    # Output CGI response
    print("Status: 200 OK")
    print("Content-Type: text/html")
    for header in headers:
        print(header)
    print()  # End of headers
    print("<html><body>")
    print(body)
    print("<a href=\"/cgi-bin/form.cgi\">Go back</a>")
    print("</body></html>")

if __name__ == "__main__":
    main()
