#include <iostream> //
#include "utilz.hxx" //contains the rgb2gray function

int main(int argc, char *argv[]) {

    //empty templates meaning defauls which in CImg is type double
    cimg_library::CImg<> input_image, output_image;

    //print out the usage if the user enters the wrong number of cmd inputs
    if(argc<2) {
        std::cout<<"Usage: CCL [-invert] input_image output_image"<<std::endl;
    }

    if(argc>1) {
        //start the clock for determining execution times
        //lets try to load the image as specified by the user..
        //throw an exception otherwise
        try {
            input_image.load(argv[1]);
        } catch (std::exception &err) {
            std::cout<<"Error loading image.."<<std::endl;
            std::cout<<err.what()<<std::endl;
        }


        input_image.display("Input Image");
        // check if the image is binary
        // check if any pixel value isn't 0 or 255
        bool isBinary = true;
        for (int r = 0; r < input_image.height(); r++) {
            for (int c = 0; c < input_image.width(); c++) {
                int val = input_image(c,r,0,0);
                if (val != 0 && val != 255) {
                    isBinary = false;
                    break;
                }
            }
            if (!isBinary) break; //escape outer loop
        }

        if (!isBinary)
            binarize_image(input_image, 127);

        
        exec_ccl(input_image, output_image);
        output_image.display("Labeled Components");
        // grayscale_image.save("gray.jpg");

    }
    //all programs must be written the Unix way
    //ANSI C++ standards will be applied to all assignments
    return 0;

}