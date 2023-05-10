#!/usr/bin/env python3

import os
import cgi
import urllib.parse

# Get the form data from the client
form = cgi.FieldStorage()

# Extract the values of the 'name' and 'age' parameters from the form or query string
name = form.getvalue('name')
age = form.getvalue('age')

if name is None or age is None:
    # If the form data is not present, try to get it from the QUERY string
    query_string = os.environ.get("QUERY_STRING", "")
    query_params = urllib.parse.parse_qs(query_string)
    name = query_params.get('name', [None])[0]
    age = query_params.get('age', [None])[0]

# Print the values of the 'name' and 'age' parameters as HTML
print("<html>")
print("<head>")
print("<title>CGI Example</title>")
print("</head>")
print("<body>")
print(f"<h1>Hello, {name}!</h1>")
print(f"<p>You are {age} years old.</p>")
print("</body>")
print("</html>")
