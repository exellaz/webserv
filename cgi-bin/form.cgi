#!/usr/bin/python3

import os

session_text = os.environ.get("SESSION_DATA", "(no session)")

print("Content-Type: text/html\r\n")
print()
print(f"""<!DOCTYPE html>
<html>
<head>
    <title>Session Control</title>
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
        <button type="submit">Delete Session</button>
    </form>
</body>
</html>""")