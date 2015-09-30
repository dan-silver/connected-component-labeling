#ifndef _utilz__
#define _utilz__
#include "CImg.h"
#include <list>
#include <set>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time to seed srand*/ 
#include <map>

// helper class for part B only
class Component {
    public:
        Component(int label) {
            area = centroid_x = centroid_y = variance_x = variance_y = 0;
            this->label = label;
        }

        int label,
            area, //zeroth moment
            centroid_x,
            centroid_y,
            variance_y,
            variance_x,
            color[3];

        // void printInfo(int newIndex) {
        void printInfo() {
            std::cout<<"Region "<<this->label<<std::endl;
            // std::cout<<"Region "<<newIndex+1<<":"<<std::endl;
            std::cout<<"\t Area: "<<this->area<<std::endl;
            std::cout<<"\t Centroid: ("<<centroid_x<<", "<<centroid_y<<")"<<std::endl;
            std::cout<<"\t variance x: "<<variance_x<<std::endl;
            std::cout<<"\t variance y: "<<variance_y<<std::endl;
        }

        void setLabel(int label) {
            this->label = label;
        }

        void incrementArea() {
            area++;
        }

        void setRandomColor() {
            color[0] = rand() % 255;
            color[1] = rand() % 255;
            color[2] = rand() % 255;
        }
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

void createComponentMap(int current_label, int parent[], std::map<int,Component> &components) {
    for (int i=0; i<current_label; i++) {
        int label = parent[i];
        std::map<int,Component>::iterator it = components.find(label);
        if(it != components.end()) continue;
           //element found;

        components.insert( std::pair<int,Component>(label, Component(label)));
    }
}

int find(int x, int parent[]) {
    int j = x;
    while (parent[j] != -1) {
        j = parent[j];
    }
    return j;
}

// // For debugging
// void printParentArray(int parent[], int number_of_labels) {
//     std::cout<<"parent array"<<std::endl;

//     for (int i=0;i<number_of_labels;i++)
//         std::cout<<i<<" ";
//     std::cout<<std::endl;

//     for (int i=0;i<number_of_labels;i++)
//         std::cout<<parent[i]<<" ";
//     std::cout<<std::endl;
// }

// union_ because union is a reserved word
void union_ (int x, int y, int parent[]) {
    int j = find(x, parent);
    int k = find(y, parent);
    if (j != k)
        parent[k] = j;
}

void exec_ccl(cimg_library::CImg<> &input_binary_image, cimg_library::CImg<> &output_image) {
    srand (time(NULL));
    output_image.assign(input_binary_image.width(), input_binary_image.height(), 1, 3);
    output_image.fill(255);

    int pixel_labels[input_binary_image.width()][input_binary_image.height()] = {0};

    int parent[2000]; 

    int current_label = 0; //current label counter. increments when a new label is needed

    // 2 passes over the input image pixels for ccl

    // first pass
    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c = 0; c < input_binary_image.width(); c++) {

            // Grab the value for the current pixel "e"
            int val = input_binary_image(c, r, 0, 0);
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
                a = input_binary_image(c - 1,r - 1, 0, 0);
            else
                a = 255;
            
            if (r > 0)
                b = input_binary_image(c,r - 1, 0, 0);
            else
                b = 255;
            
            if (r > 0 && c + 1 < input_binary_image.width())
                c_ = input_binary_image(c + 1,r - 1, 0, 0);
            else
                c_ = 255;
            
            if (c > 0)
                d = input_binary_image(c - 1,r, 0, 0);
            else
                d = 255;

            // decision tree for 8-connected neighbors
            if (b == 0) {
                // copy b
                // e = b;
                e = pixel_labels[c][r-1];
            } else {
                if (c_ == 0) {
                    if (a == 0) {
                        // e = c_;
                        // copy c, a
                        union_(pixel_labels[c+1][r-1], pixel_labels[c-1][r-1], parent);
                        e = pixel_labels[c+1][r-1];
                    } else {
                        if (d == 0) {
                            // copy c, d
                            union_(pixel_labels[c+1][r-1], pixel_labels[c-1][r], parent);
                            // e = c_;
                            e = pixel_labels[c+1][r-1];
                        } else {
                            // copy c
                            // e = c_;
                            e = pixel_labels[c+1][r-1];
                        }
                    }
                } else {
                    if (a == 0) {
                        // copy a
                        // e = a;
                        e = pixel_labels[c-1][r-1];
                    } else {
                        if (d == 0) {
                            // copy d
                            // e = d;
                            e = pixel_labels[c-1][r];
                        } else {
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

    // second pass to update the labels


    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if ((input_binary_image(c, r, 0, 0)) == 255)
                continue; //pixel is in the background

            pixel_labels[c][r] = find(pixel_labels[c][r], parent);
        }
    }

    // The following code is for PAINTING the region colors (the two passes have already been completed)
    int number_of_components = getNumberOfComponents(parent, current_label);
    // loop through parent array, replace -1's with their index
    for (int i=0; i<current_label; i++) {
        if (parent[i] == -1)
            parent[i] = i;
    }

    std::map <int, Component> components; //{original label, Component}
    Component *component;
    createComponentMap(current_label, parent, components);

    //generate random colors for each component
    typedef std::map<int, Component>::iterator it_type;
    for(it_type iterator = components.begin(); iterator != components.end(); iterator++) {
        iterator->second.setRandomColor();
    }

    // color the image by their components
    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if ((input_binary_image(c, r, 0, 0)) == 255)
                continue; //background pixel

            int label = pixel_labels[c][r];

            std::map<int,Component>::iterator it = components.find(label);
            if(it != components.end()) {
                component = &it->second;



                output_image(c, r, 0) = component->color[0];
                output_image(c, r, 1) = component->color[1];
                output_image(c, r, 2) = component->color[2];
            }
        }

    }

    // 0th moment
    // loop over pixels and add them to the area of the components

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if ((input_binary_image(c, r, 0, 0)) == 255) {
                continue; //background pixel
            }
            // find the component (very fast) and increment the area
            int label = pixel_labels[c][r];

            std::map<int,Component>::iterator it = components.find(label);
            if(it != components.end()) {
                component = &it->second;
                component->incrementArea();
            }
        }
    }

    // 1st moment
    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if ((input_binary_image(c, r, 0, 0)) == 255) {
                continue; //background pixel
            }
            // find the component (very fast) and add to the 
            int label = pixel_labels[c][r];
            std::map<int,Component>::iterator it = components.find(label);
            if(it != components.end()) {
                component = &it->second;
                component->centroid_x += c;
                component->centroid_y += r;
            }
        }
    }

    // Loop over the components, and divide the sum of the coordinates by the area to finish the first moment calculation
    for(it_type iterator = components.begin(); iterator != components.end(); iterator++) {
        component = &iterator->second;
        if (component->area == 0) continue;
        component->centroid_x /= component->area;
        component->centroid_y /= component->area;
    }

    // 2nd moment

    for (int r = 0; r < input_binary_image.height(); r++) {
        for (int c= 0; c < input_binary_image.width(); c++) {
            if ((input_binary_image(c, r, 0, 0)) == 255) {
                continue; //background pixel
            }
            // find the component (very fast) and add to the 
            int label = pixel_labels[c][r];
            std::map<int,Component>::iterator it = components.find(label);
            if(it != components.end()) {
                component = &it->second;
                component->variance_x += (r - component->centroid_x) * (r - component->centroid_x);
                component->variance_y += (c - component->centroid_y) * (c - component->centroid_y);
            }

        }
    }
    // finish the variance equation by dividing by the area
    for(it_type iterator = components.begin(); iterator != components.end(); iterator++) {
        component = &iterator->second;
        if (component->area == 0) continue;
        component->variance_x /= component->area;
        component->variance_y /= component->area;
    }


    std::cout<<"Number of Components: "<<number_of_components<<std::endl;
    std::cout<<"Maximum number of intermediate labels: "<<current_label<<std::endl;

    for(it_type iterator = components.begin(); iterator != components.end(); iterator++) {
        component = &iterator->second;
        component->printInfo();
    }

}

void convert_to_grayscale(cimg_library::CImg<> &image,cimg_library::CImg<> &grayscale_image){
    grayscale_image.assign(image.width(),image.height(),1,1);
    grayscale_image.fill(0);
    if(image.spectrum()==3 || image.spectrum() == 4) {
        int R,G,B;
        for (int r = 0 ; r< image.height(); r++) {
            for (int c= 0 ; c < image.width(); c++) {
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


void invertImage (cimg_library::CImg<> &image) {
    for (int y = 0 ; y < image.height(); y++){
        for (int x= 0 ; x < image.width(); x++){
            int pixelValue = (int)image(x, y, 0);
            image(x, y, 0) = 255 - pixelValue;
        }
    }
}

bool imageIsBinary(cimg_library::CImg<> &image) {
    bool isBinary = true;
    for (int r = 0; r < image.height(); r++) {
        for (int c = 0; c < image.width(); c++) {
            int val = image(c,r,0,0);
            if (val != 0 && val != 255) {
                isBinary = false;
                break;
            }
        }
        if (!isBinary) break; //escape outer loop
    }
    return isBinary;
}

#endif