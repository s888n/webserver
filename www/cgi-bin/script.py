#!/usr/bin/env python3
import cgi
import cgitb
import os
from urllib.parse import parse_qs

cgitb.enable()

body = os.environ.get('QUERY_STRING')
parsed_body = parse_qs(body)

username = parsed_body.get("username")[0]
password = parsed_body.get("password")[0]
 # Don't forget to set the content type
print()
print("<html><body>")
print("username: " + str(username))
print("<br/>")
print("password: " + str(password))
if username == "hamza" and password == "younsi":
    print("<h1>Login successful!</h1>")
else:
    print("<h1>Login failed.</h1>")
print("</body></html>")