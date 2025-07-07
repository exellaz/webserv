#!/usr/bin/python3

import os

session_text = os.environ.get("SESSION_DATA", "(no session)")

print("Content-Type: text/html\r\n")
print()
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Session Control</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 40px;
            padding: 20px;
            max-width: 600px;
            margin-left: auto;
            margin-right: auto;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }}

        h1 {{
            color: #333;
            font-size: 28px;
        }}

        h2 {{
            color: #555;
            margin-top: 30px;
        }}

        form {{
            margin-top: 10px;
        }}

        label {{
            display: block;
            margin-bottom: 6px;
            font-weight: bold;
        }}

        input[type="text"] {{
            width: 100%;
            padding: 10px;
            margin-bottom: 12px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }}

        button {{
            background-color: #007BFF;
            color: white;
            padding: 10px 18px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }}

        button:hover {{
            background-color: #0056b3;
        }}
    </style>
</head>
<body>
    <h1>Hello, {session_text}</h1>

    <h2>Update Session Data</h2>
    <form method="POST" action="/cgi-bin/session.cgi">
        <label for="data">New Session Value:</label>
        <input type="text" name="data" id="data">
        <button type="submit">Update</button>
    </form>

    <h2>Delete Session</h2>
    <form method="POST" action="/cgi-bin/session.cgi">
        <input type="hidden" name="delete" value="1">
        <button type="submit" style="background-color: #dc3545;">Delete Session</button>
    </form>
</body>
</html>""")
