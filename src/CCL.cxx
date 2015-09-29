#include <iostream> //
#include "utilz.hxx" //contains the rgb2gray function

int main(int argc, char *argv[]) {

    cimg_library::CImg<> input_image, grayscale_image, output_image;
    char* input_image_str;
    char* output_image_str;
    bool invert = false; 

    //print out the usage if the user enters the wrong number of cmd inputs
    // arg count of 4 is allowed only if the command starts with the -invert flag
    if((argc != 3 && argc != 4) || (argc == 4 && std::string(argv[1]) != "-invert")) {
        std::cout<<"Usage: CCL [-invert] input_image output_image"<<std::endl;
        return 1;
    }


    if (argc == 3) {
        input_image_str = argv[1];
        output_image_str = argv[2];
    } else {
        input_image_str = argv[2];
        output_image_str = argv[3];
        invert = true;
    }

    //lets try to load the image as specified by the user..
    //throw an exception otherwise
    try {
        input_image.load(input_image_str);
    } catch (std::exception &err) {
        std::cout<<"Error loading image.."<<std::endl;
        std::cout<<err.what()<<std::endl;
    }

    //first make sure the image has 1 channel
    std::cout<<"AAA"<<std::endl;
    convert_to_grayscale(input_image, grayscale_image);
    std::cout<<"BBB"<<std::endl;
    if (input_image.spectrum()==3 || input_image.spectrum() == 4) {
        input_image = grayscale_image;
    }

    //invert if the flag was set
    if (invert) {
        std::cout<<"CCC"<<std::endl;
        invertImage(input_image);
        std::cout<<"DDD"<<std::endl;
    }

    // check if the image is binary
    // check if any pixel value isn't 0 or 255
    std::cout<<"EEE"<<std::endl;
    if (!imageIsBinary(input_image))
        binarize_image(input_image, 127);
    std::cout<<"FFF"<<std::endl;

    input_image.display("Input Image");

    
    exec_ccl(input_image, output_image);
    output_image.display("Labeled Components");
    output_image.save(output_image_str);

    //all programs must be written the Unix way
    //ANSI C++ standards will be applied to all assignments
    return 0;

}