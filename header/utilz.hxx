#ifndef _utilz__
#define _utilz__
#include "CImg.h"
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class Histogram {
    public:
        cimg_library::CImg<> image;

        Histogram(cimg_library::CImg<> &image) {
            this->image = image;
        }

        int* placePixelValuesIntoBins(int channel, int number_of_bins) {
            int* countArray = new int[number_of_bins];
            for (int i=0; i<number_of_bins; i++) {
                countArray[i] = 0;
            }
            
            int value;
            for (int r = 0 ; r < this->image.height(); r++) {
                for (int c= 0 ; c < this->image.width(); c++) {
                    value = (int)image(c, r, 0, channel);

                    countArray[value]++;
                }
            }
            return countArray;
        }

        void saveHistogramToFile(string chartTitle, int* counts, int number_of_bins) {
            std::ofstream out(("../public/" + chartTitle + ".html").c_str());
            out << "<link href='c3.min.css' rel='stylesheet' type='text/css'><script src='d3.min.js' charset='utf-8'></script><script src='c3.min.js'></script>";
            out << "<h3>" << chartTitle << "</h3><div id='chart'></div>";
            out << "<script type='text/javascript'>";
            out << "  var chart = c3.generate({";
            out << "    bindto: '#chart',";
            out << "    data: {";
            out << "      columns: [";
            out << "        ['Number of Pixels With Value', ";
            
            // print the data into the HTML string
            for (int i=0; i<number_of_bins; i++) {
                out << counts[i] << ",";
            }

            out << "]], types: {";
            out << "'Number of Pixels With Value': 'area'";
            out << "}}});</script>";
            out.close();
        }

};

void plot_histogram(cimg_library::CImg<> &image) {
    Histogram hist = Histogram(image);
    for (int i=0; i<image.spectrum(); i++) {
    	ostringstream os;
        os << "Input Image Channel " << (i+1) << " of " << image.spectrum();
        string title = os.str();
        int* theArray = hist.placePixelValuesIntoBins(i, 255);
        hist.saveHistogramToFile(title, theArray, 255);
    }
}

// // if the image has 1 channel, the image will not be converted
// void convert_to_grayscale(cimg_library::CImg<> &image,cimg_library::CImg<> &grayscale_image){
//     grayscale_image.assign(image.width(),image.height(),1,1);
//     grayscale_image.fill(0);
//     if(image.spectrum()==3 || image.spectrum() == 4) {
//         int R,G,B;
//         for (int r = 0 ; r< image.height(); r++) {
//             for (int c= 0 ; c < image.width(); c++) {
//                 R = (int)image(c,r,0,0);
//                 G = (int)image(c,r,0,1);
//                 B = (int)image(c,r,0,2);
//                 grayscale_image(c,r,0,0) = (float) (0.2989 * R + 0.5870 * G + 0.1140 * B);
//             }
//         }
//     } else if (image.spectrum() == 1) {
//         grayscale_image = image;
//     }
// }

#endif