#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
g++-11 -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -lGL -lglut -lpthread -o robotsv1
#MacOS
#g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -framework OpenGL -framework GLUT -o robotsv1
./robotsv1 16 16 4 3 > output.txt