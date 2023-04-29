import sys

name = sys.argv[1]
age = sys.argv[2]
gender = sys.argv[3]

if gender == "M":
    pronoun = "He"
else:
    pronoun = "She"

story = f"{name} is {age} years old. One day, while {pronoun} was on a hike, {pronoun} stumbled upon a hidden cave with hidden booty inside. He never left the cave again :D"

print(story)
