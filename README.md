# Little Computer Emulator

Little computer emulator is a personal project aimed to create an emulator of a simple CPU architecture and the set of tools that can be used to write programs for that CPU. The project is primarily designed to be used in school and universities to show the path that a program takes to get from the source code to the result of the execution.

## How to build

You will need CMake 3.22 or later, make (or any other build system) and any C++ compiler with C++20 support installed on your computer. At the moment the project was only tested with clang on Linux, but I do not expect any issues with building it on any other platform that meets all the requirements stated above.

To build:
1. Clone the project:

    ```
    git clone https://github.com/Poseydon42/little-computer-emulator.git
    ```

2. Run CMake:

    ```
    mkdir build
    cd build
    cmake ..
    ```

3. Build the project

    ```
    make
    ```

## License

The project is distributed under MIT license. You can read the full license test in [LICENSE.md]