#!/usr/bin/env python3

import os
import cgi
import http.cookies

print("Content-Type: text/html")    

form = cgi.FieldStorage()          

cookie_string = os.environ.get('HTTP_COOKIE')
cookie = http.cookies.SimpleCookie()
cookie.load(cookie_string)

#if name and color in cookie use that, else use name and color in form

if "name" in form:
    name = form["name"].value
else:
    name = "world"


if "color" in form:
    color = form["color"].value
else:
    color = "white"

if "visited" in cookie:
    color = cookie.get("color").value
    name = cookie.get("name").value
    print()
    print(f"""
    <!DOCTYPE html>
    <html>
    <head>
    <title>Cookie</title>
    </head>
    <body style="background-color: {color} ;">
    <h1>Hi again, {name}!</h1>
    </body>
    </html>
    """)
else:
    cookie["visited"] = "yes"
    print(f"Set-Cookie: {cookie['visited'].OutputString()}")
    cookie["name"] = name
    print(f"Set-Cookie: {cookie['name'].OutputString()}")
    cookie["color"] = color
    print(f"Set-Cookie: {cookie['color'].OutputString()}")
    print()
    print(f"""
    <!DOCTYPE html>
    <html>
    <head>
    <title>Cookie</title>
    </head>
    <body style="background-color: {color} ;">
    <h1>Hi, {name} for the first time!</h1>
    </body>
    </html>
    """)
