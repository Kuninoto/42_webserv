import sys

input_data = sys.stdin.read()

with open('newfile.txt', 'w') as file:
    file.write(input_data)

print("File created successfully.")