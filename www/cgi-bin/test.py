#!/usr/bin/python3
import os
from urllib.parse import parse_qs

env = os.environ
query_string = os.environ.get('QUERY_STRING', 'No query string')

parsed_query_string = parse_qs(query_string)

name = parsed_query_string.get('name', [''])[0]
message = parsed_query_string.get('message', [''])[0]

print ("HTTP/1.0 200 OK")
print ("Content-type:text/html\r\n\r\n")

print ("<html>")
print ("<head>")
print ("<title>Test CGI</title>")
print ("</head>")
print ("<body>")
print ("<ul>")
print ("<li> <h2> <span style='color:green'>name </span> : %s   </h2> </li>" % (name))
print ("<li> <h2> <span style='color:green'>message </span> : %s </h2> </li>" % (message))
print ("</ul>")
print ("<hr>")
print ("<h2> Envionment: </h2>")
print ("<ol>")
for key in env.keys():
    print ("<li> <span style='color:purple'>%20s</span> : %s </li>" % (key, env[key]))
print ("</ol>")
print ("</body>")
print ("</html>")