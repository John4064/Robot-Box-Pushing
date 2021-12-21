#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
#g++-11 -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -lGL -lglut -lpthread -o robotsv1
#MacOS
g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -framework OpenGL -framework GLUT -o robotsv1 -std=c++11

./robotsv1 18 18 8 3