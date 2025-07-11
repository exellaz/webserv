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

    print(f"""<!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Session Action</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #f9f9f9;
                color: #333;
                padding: 40px;
                max-width: 600px;
                margin: auto;
                background: #fff;
                border-radius: 8px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            }}
            h1 {{
                font-size: 24px;
                color: #007BFF;
            }}
            p {{
                font-size: 16px;
            }}
            .back-link {{
                display: inline-block;
                margin-top: 20px;
                text-decoration: none;
                background-color: #007BFF;
                color: white;
                padding: 10px 16px;
                border-radius: 5px;
            }}
            .back-link:hover {{
                background-color: #0056b3;
            }}
        </style>
    </head>
    <body>
        <h1>Session Status</h1>
        <p>{body}</p>
        <a class="back-link" href="/cgi-bin/form.cgi">⟵ Go back</a>
    </body>
    </html>""")

if __name__ == "__main__":
    main()
