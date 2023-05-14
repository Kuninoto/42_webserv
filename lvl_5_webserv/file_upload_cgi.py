#!/usr/bin/env python3
import cgi
import cgitb
import os
import sys

cgitb.enable()

UPLOAD_FOLDER = 'uploads'

def main():
	print("Content-Type: text/html\n")

	form = cgi.FieldStorage(fp=sys.stdin)

	if 'file' not in form:
		print("Morty, there's no file in the request, get your sh*t together!")
		return

	file_item = form['file']
	filename = os.path.basename(file_item.filename)

	if not filename:
		print("Morty, you didn't select a file, what are you doing?")
		return

	# Create the uploads folder if it doesn't exist
	if not os.path.exists(UPLOAD_FOLDER):
		os.makedirs(UPLOAD_FOLDER)

	# Save the file
	file_path = os.path.join(UPLOAD_FOLDER, filename)
	with open(file_path, 'wb') as f:
		f.write(file_item.file.read())

	print(f"Wubba lubba dub dub! File '{filename}' uploaded and saved, Morty.")

if __name__ == '__main__':
	main()
