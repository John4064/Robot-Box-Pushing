#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -lGL -lglut -o robotsv1
#MacOS
#g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -framework OpenGL -framework GLUT -o robotsv1
./robotsv1 16 16 4 3