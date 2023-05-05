import sys

# Get the filename from the command line arguments
filename = sys.argv[1]

# Write the string "I'm Mr. Meeseeks! Look at me!" to the file
with open(filename, 'w') as file:
    file.write("I'm Mr. Meeseeks! Look at me!")
