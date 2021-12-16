#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
#g++-11 -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -lGL -lglut -lpthread -o robotsv1
#MacOS
<<<<<<< HEAD
#g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -framework OpenGL -framework GLUT -o robotsv1
./robotsv1 16 16 7 3 > output.txt
=======
g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -framework OpenGL -framework GLUT -o robotsv1 -std=c++14
./robotsv1 16 16 4 3 > output.txt
>>>>>>> e042d7a8981031efc4ce2e8dbea29fa6ae536ca1
