#John Parkhurst & David Shawver

rm -f "robotsv1"

#Linux
<<<<<<< HEAD
g++-11 -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp ../Source/Robot.cpp -lm -lGL -lglut -lpthread -o robotsv1
#MacOS
#g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -framework OpenGL -framework GLUT -o robotsv1
./robotsv1 16 16 4 3 > output.txt
=======
#g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -lGL -lglut -o robotsv1
#MacOS
g++ -Wall ../Source/main.cpp ../Source/gl_frontEnd.cpp -lm -framework OpenGL -framework GLUT -o robotsv1
./robotsv1 16 16 4 3
>>>>>>> 23fc6f5ff170a60d5d0e6bd72352f9ece1327a70
