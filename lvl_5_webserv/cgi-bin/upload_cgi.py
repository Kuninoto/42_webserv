#! /usr/bin/python3

import cgi
import os
import cgitb

# DEBUG

#import sys
# for line in sys.stdin:
#   print(line)

UPLOAD_DIR = "/upload"

cgitb.enable()
form = cgi.FieldStorage()

# Get filename here
fileitem = form['fileTest']

# Test if the file was uploaded
if fileitem.filename:
   open(os.getcwd() + UPLOAD_DIR + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded to ' + os.getcwd() + UPLOAD_DIR
else:
   message = 'Uploading Failed'

print("<h1> " + message + " </h1>")
