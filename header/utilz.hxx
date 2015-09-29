#ifndef _utilz__
#define _utilz__
#include "CImg.h"
#include <list>
#include <stdlib.h>     /* srand, rand */

// helper class for part B only
class region {
    int label;
    int area; //zeroth moment
    int centroid_x;
    int centroid_y;
};

int getNumberOfComponents(int parent[], int number_of_labels) {
    // simply count the number of -1s
    // a -1 means that it's a unique tree in the forest

    int number_of_components = 0;
    for (int i=0; i<number_of_labels; i++)
        if (parent[i] == -1)
            number_of_components++;

    return number_of_components;
}

int find(int x, int parent[]) {
    int j = x;
    while (parent[j] != -1) {
        j = parent[j];
    }
    return j;
}

// union_ because union is a reserved word
void union_ (int x, int y, int parent[]) {
    int j = find(x, parent);
    int k = find(y, parent);

    if (j != k)
        parent[k] = j;
}

void exec_ccl(cimg_library::CImg<> &input_binary_image, cimg_library::CImg<> &output_image){
    output_image.assign(input_binary_image.width(), input_binary_image.height(),1,3);
    output_image.fill(255);

    int pixel_labels[input_binary_image.width()][input_binary_image.height()] = {0};

    int parent[2000]; 

    int current_label = 0; //current label counter. starts at 1, increments when a new label is needed

    // 2 passes over the input image pixels

    // first pass
    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c = 0; c < input_binary_image.width(); c++) {

            // Grab the value for the current pixel "e"
            int val = (int) input_binary_image(c, r, 0, 0);
            if (val == 255) {
                continue;
            }

            // val != 255, => check 16 cases

            // read the nearby pixels
            //   ------------
            //   | a | b | c|
            //   ------------
            //   | d | e |
            //   ---------

            int a, b, c_, d, e; //c_ because c already used in loop
            if (c > 0 && r > 0)
                a = (int) input_binary_image(c - 1,r - 1, 0, 0);
            else
                a = 255;
            
            if (r > 0)
                b = (int) input_binary_image(c,r - 1, 0, 0);
            else
                b = 255;
            
            if (r > 0 && c + 1 < input_binary_image.width())
                c_ = (int) input_binary_image(c + 1,r - 1, 0, 0);
            else
                c_ = 255;
            
            if (c > 0)
                d = (int) input_binary_image(c - 1,r, 0, 0);
            else
                d = 255;

            // decision tree for 8-connected neighbors
            if (b == 0) {
                // copy b
                // e = b;
                e = pixel_labels[c][r-1];
            } else if (b == 255){
                if (c_ == 0) {
                    if (a == 0) {
                        // e = c_;
                        e = pixel_labels[c+1][r-1];
                    } else if (a == 255) {
                        if (d == 0) {
                            // copy c, d
                            union_(pixel_labels[c+1][r-1], pixel_labels[c-1][r], parent);
                            // e = c_;
                            e = pixel_labels[c+1][r-1];
                        } else if (d == 255) {
                            // copy c
                            // e = c_;
                            e = pixel_labels[c+1][r-1];
                        }
                    }
                } else if (c_ == 255) {
                    if (a == 0) {
                        // copy a
                        // e = a;
                        e = pixel_labels[c-1][r-1];
                    } else if (a == 255) {
                        if (d == 0) {
                            // copy d
                            // e = d;
                            e = pixel_labels[c-1][r];
                        } else if (d == 255) {
                            // new label
                            e = current_label;
                            parent[current_label] = -1; //new tree in the forest
                            current_label++;
                        }
                    }
                }
            }

            pixel_labels[c][r] = e;

        }
    }

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if (((int) input_binary_image(c, r, 0, 0)) == 255) {
                continue; //pixel is in the background
            }
            pixel_labels[c][r] = find(pixel_labels[c][r], parent);
        }
    }

    // The following code is for PAINTING the region colors (the two passes have already been completed)
    int number_of_components = getNumberOfComponents(parent, current_label);
    // color the image by their components

    //generate random colors for each component
    // I realize that not all 2000 colors will be used, but this simplifies the following section
    int colors_r[2000],
        colors_g[2000],
        colors_b[2000];

    for (int i=0; i<2000; i++) {
        colors_r[i] = rand() % 255;
        colors_g[i] = rand() % 255;
        colors_b[i] = rand() % 255;
    }

    // loop through parent array, replace 0's with their index
    for (int i=0; i<current_label; i++) {
        if (parent[i] == -1)
            parent[i] = i;
    }

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if (((int) input_binary_image(c, r, 0, 0)) == 255) {
                continue; //background pixel
            }

            int label = parent[pixel_labels[c][r]];
            output_image(c, r, 0) = colors_r[label];
            output_image(c, r, 1) = colors_g[label];
            output_image(c, r, 2) = colors_b[label];
        }
    }

    std::cout<<"Number of Components = "<<number_of_components<<std::endl;
}

void convert_to_grayscale(cimg_library::CImg<> &image,cimg_library::CImg<> &grayscale_image) {
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

void binarize_image (cimg_library::CImg<> &image, int threshold) {
    for (int y = 0 ; y < image.height(); y++){
        for (int x= 0 ; x < image.width(); x++){
            bool pixelValue = (int)image(x, y, 0, 0) > threshold; // true = white, false = black
            image(x, y, 0, 0) = pixelValue ? 255 : 0;
        }
    }
}


#endif