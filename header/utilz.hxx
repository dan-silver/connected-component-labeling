#ifndef _utilz__
#define _utilz__
#include "CImg.h"

int find(int x, int parent[]) {
    int j = x;
    while (parent[j] != 0) {
        j = parent[j];
    }
    return j;
}

// union_ because union is a reserved word
void union_ (int x, int y, int parent[]) {
    int j = find(x, parent);
    int k = find(y, parent);

    if (j != k) {
        parent[k] = j;
    }
}


void flatten(int x, int parent[]) {
    int j = x;
    while (parent[j] != 0) {
        j = parent[j];
    }
    parent[x] = j;
}

void exec_ccl(cimg_library::CImg<> &input_binary_image, cimg_library::CImg<> &output_image){
    output_image.assign(input_binary_image.width(), input_binary_image.height(),1,1);
    output_image.fill(0);

    int pixel_labels[input_binary_image.width()][input_binary_image.height()] = {0};

    int parent[2000]; 

    int current_label = 1; //current label counter. starts at 1, increments when a new label is needed






    // 2 passes over the input image pixels

    // first pass
    std::cout<<"AAA"<<std::endl;
    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c = 0; c < input_binary_image.width(); c++) {

            // Grab the value for the current pixel "e"
            int val = (int) input_binary_image(c, r, 0, 0);
            std::cout<<val<<" ";
            if (val == 255) {
                continue;
            }

            // val != 0, => check 16 cases


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
                        // copy c_, a
                        union_(c_, a, parent);
                        // e = c_;
                        e = pixel_labels[c+1][r-1];
                        //TODO
                    } else if (a == 255) {
                        if (d == 0) {
                            // copy c, d
                            union_(c_, d, parent);
                            //TODO
                            e = c_;
                            e = pixel_labels[c+1][r-1];
                        } else if (d == 255) {
                            // copy c
                            e = c_;
                            e = pixel_labels[c+1][r-1];
                        }
                    }
                } else if (c_ == 255) {
                    if (a == 0) {
                        // copy a
                        e = a;
                        e = pixel_labels[c-1][r-1];
                    } else if (a == 255) {
                        if (d == 0) {
                            // copy d
                            e = d;
                            e = pixel_labels[c-1][r];
                        } else if (d == 255) {
                            // new label
                            e = current_label;
                            parent[current_label] = 0; //new tree in the forest
                            current_label++;
                        }
                    }
                }
            }

            pixel_labels[c][r] = e;
            std::cout<<"Pixel label e:"<<e<<std::endl;

        }
    }

    // second pass
    
    std::cout<<"BBB"<<std::endl;

    //first flatten the parent tree
    for (int i=1; i<current_label; i++) {
        if (parent[i] != 0) //don't flatten tree roots
          flatten(i, parent);
    }
    std::cout<<"CCC"<<std::endl;

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if (pixel_labels[c][r] == -1) {
                continue; //-1 is the default, so this pixel isn't in a component
            }

            int tree_root_label = find(pixel_labels[c][r], parent);
            pixel_labels[c][r] = tree_root_label;
        }
    }


    // color the image by their components
    int colors_r[9] = {255,254,253,253,253,241,217,166,127};
    int colors_g[9] = {245,230,208,174,141,105,72,54,39};
    int colors_b[9] = {235,206,162,107,60,19,1,3,4};

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if (pixel_labels[c][r] == -1) {
                continue; //-1 is the default, so this pixel isn't in a component
            }

            int component_index = pixel_labels[c][r];
            input_binary_image(c,r,0) = colors_r[component_index];
            input_binary_image(c,r,1) = colors_g[component_index];;
            input_binary_image(c,r,2) = colors_b[component_index];;
        }
    }

}

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

void binarize_image (cimg_library::CImg<> &image, int threshold) {
    for (int y = 0 ; y < image.height(); y++){
        for (int x= 0 ; x < image.width(); x++){
            bool pixelValue = (int)image(x, y, 0, 0) > threshold; // true = white, false = black
            image(x, y, 0, 0) = pixelValue ? 255 : 0;
        }
    }
}


#endif