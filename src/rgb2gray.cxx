#include "utilz.hxx" //contains the rgb2gray function
#include <iostream> //
#include <ctime> //for timing

int main(int argc, char *argv[]) {

    //empty templates meaning defauls which in CImg is type double
    cimg_library::CImg<> color_image;
    cimg_library::CImg<> grayscale_image;

    //print out the usage if the user enters the wrong number of cmd inputs
    if(argc<2) {
        std::cout<<"Usage: executable_name image filename"<<std::endl;
    }

    if(argc>1) {
        //start the clock for determining execution times
        std::clock_t start;
        start = std::clock();
        //lets try to load the image as specified by the user..
        //throw an exception otherwise
        try {
            color_image.load(argv[1]);
            std::cout << "Time taken to load the image: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
        } catch (std::exception &err) {
            std::cout<<"Error loading image.."<<std::endl;
            std::cout<<err.what()<<std::endl;
        }

        //call the function to convert
        start = std::clock();
        convert_to_grayscale(color_image, grayscale_image);

        std::cout << "Time taken to convert the rgb image to grayscale: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

        grayscale_image.display("Grayscale Image with the standard formula");
        grayscale_image.save("gray.jpg");

    }
    //all programs must be written the Unix way
    //ANSI C++ standards will be applied to all assignments
    return 0;

}