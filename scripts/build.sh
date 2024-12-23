gcc -Wall -Wextra -D CL_TARGET_OPENCL_VERSION=100 ../src/core/*.cpp ../src/utils/*.cpp ../src/main.cpp -lOpenCL -lm -lSDL2 -lSDL2_ttf -lstdc++ -std=c++17 -o ../build/main
cd ../build/
./main
