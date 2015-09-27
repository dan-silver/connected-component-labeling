#ifndef _utilz__
#define _utilz__
#include "CImg.h"

// Part A
void convert_to_grayscale(cimg_library::CImg<> &image,cimg_library::CImg<> &grayscale_image){
    grayscale_image.assign(image.width(),image.height(),1,1);
    grayscale_image.fill(0);
    if(image.spectrum()==3 || image.spectrum() == 4){
        int R,G,B;
        for (int r = 0 ; r< image.height(); r++){
            for (int c= 0 ; c < image.width(); c++){
                R = (int)image(c,r,0,0);
                G = (int)image(c,r,0,1);
                B = (int)image(c,r,0,2);
                grayscale_image(c,r,0,0) = (float) (0.2989 * R + 0.5870 * G + 0.1140 * B);
            }
        }
    } else if(image.spectrum()==1) {
        grayscale_image=image;
    }

}

#endif