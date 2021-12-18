#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
g++-11 -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -lGL -lglut -lpthread -o robotsv1
#MacOS


./robotsv1 20 20 20 3 > output.txt