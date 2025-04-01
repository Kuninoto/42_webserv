#!/usr/bin/env python3
import cgi
import cgitb; cgitb.enable()
import os
import sys

UPLOAD_FOLDER = sys.argv[1]

def main():
	form = cgi.FieldStorage()

	if "filename" not in form:
		print("<h1> there's no 'filename' in the request </h1>")
		return

	file_item = form["filename"]
	filename = os.path.basename(file_item.filename)

	if not filename:
		print("<h1> No file was selected </h1>")
		return

	# Create the uploads folder if it doesn't exist
	if not os.path.exists(UPLOAD_FOLDER):
		os.makedirs(UPLOAD_FOLDER)

	# Save the file
	file_path = os.path.join(UPLOAD_FOLDER, filename)
	with open(file_path, 'wb') as f:
		f.write(file_item.file.read())

	print(f"File '{filename}' successfully uploaded and saved")
	print("<a href=\"/\">Back</a>")

if __name__ == '__main__':
	main()
