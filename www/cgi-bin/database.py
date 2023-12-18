#!/usr/bin/env python3
import csv
import os
from urllib.parse import parse_qs

query_string = os.environ.get('QUERY_STRING', 'No query string')

parsed_query_string = parse_qs(query_string)

firstname = parsed_query_string.get('firstname', [''])[0]
lastname = parsed_query_string.get('lastname', [''])[0]

print("HTTP/1.1 200 OK")
print('Content-Type: text/html\r\n\r\n')
print('<html>')
print('<head>')
print('<title>Database</title>')
#add css
print('<style>')
print('body{')
print('background-color: #e6f7ff;')
print('}')
print('h1{')
print('color: #000000;')
print('text-align: center;')
print('}')
print('p{')
print('font-family: verdana;')
print('font-size: 20px;')
print('}')
print('</style>')

print('</head>')
print('<body>')
print('<h1>Database</h1>')
print('<p>First Name: {}</p>'.format(firstname))
print('<p>Last Name: {}</p>'.format(lastname))
print("<h3>status: </h3>")

if not os.path.isfile('database.csv'):
    with open('database.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([firstname, lastname])
        print('Name added to the database.')
else:
    with open('database.csv', 'r') as f:
        reader = csv.reader(f)
        if any((firstname, lastname) == (row[0], row[1]) for row in reader):
            print('Name already exists in the database.')
        else:
            with open('database.csv', 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow([firstname, lastname])
                print('Name added to the database.')
print('</body>')
print('</html>')
