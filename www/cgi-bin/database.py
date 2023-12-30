#!/usr/bin/env python3
import csv
import os
from urllib.parse import parse_qs

dbfile = "www/cgi-bin/database.csv"
query_string = os.environ.get('QUERY_STRING', 'No query string')

parsed_query_string = parse_qs(query_string)

firstname = parsed_query_string.get('firstname', [''])[0]
lastname = parsed_query_string.get('lastname', [''])[0]

request_method = os.environ.get('REQUEST_METHOD')

# print("HTTP/1.1 200 OK")
# print('Content-Type: text/html\r\n\r\n')
print('<html>')
print('<head>')
print('<title>Database</title>')

print('<style>')
print('body { background-color: #e6f7ff ; } ')
print('h1 { color: #000000 ; text-align: center; } ')
print('p { font-family: verdana; font-size: 20px; }')
print('</style>')

print('</head>')

print('<body>')
print('<h1>Webserv - Database</h1>')

if request_method == 'POST':
    print('<p>First Name: {}</p>'.format(firstname))
    print('<p>Last Name: {}</p>'.format(lastname))
    print("<h3>status: </h3>")

    if not os.path.isfile(dbfile):
        with open(dbfile, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([firstname, lastname])
            print('Name added to the database.')
    else:
        with open(dbfile, 'r') as f:
            reader = csv.reader(f)
            if any((firstname, lastname) == (row[0], row[1]) for row in reader):
                print("<span style='color:red'>Name already exists in the database.</span>")
            else:
                with open(dbfile, 'a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([firstname, lastname])
                    print("<span style='color:green'>Name added to the database.</span>")

if request_method == 'GET':
    if not os.path.isfile(dbfile):
        print("<span style='color:red'>Database is empty.</span>")
    else:
        with open(dbfile , 'r') as f:
            reader = csv.reader(f)
            print("<table>")
            print("<tr>")
            print("<th>First Name</th>")
            print("<th>Last Name</th>")
            print("</tr> ")
            for row in reader:
                print("<tr>")
                print("<td>{}</td>".format(row[0]))
                print("<td>{}</td>".format(row[1]))
                print("</tr>")
            print("</table>")
        print("<style>")
        print("table {")
        print("font-family: arial, sans-serif;")
        print("border-collapse: collapse;")
        print("width: 100%;")
        print("}")
        print("td, th {")
        print("border: 1px solid #dddddd;")
        print("text-align: left;")
        print("padding: 8px;")
        print("}")
        print("tr:nth-child(even) {")
        print("background-color: #dddddd;")
        print("}")
        print("</style>")


print('</body>')    
print('</html>')
