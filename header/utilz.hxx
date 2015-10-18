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
        float* pixel_count_array;
        int number_of_bins,
            channel;


        Histogram(cimg_library::CImg<> &image, int number_of_bins, int channel) {
            this->image = image;
            this->number_of_bins = number_of_bins;
            this->channel = channel;
            placePixelValuesIntoBins();
        }

        Histogram* placePixelValuesIntoBins() {
            this->pixel_count_array = new float[this->number_of_bins];
            for (int i=0; i<this->number_of_bins; i++) {
                pixel_count_array[i] = 0;
            }
            
            int value;
            for (int r = 0 ; r < this->image.height(); r++) {
                for (int c= 0 ; c < this->image.width(); c++) {
                    value = (int)image(c, r, 0, this->channel);

                    //calculate which bin to increment
                    int bin = floor((float)value/255 * (this->number_of_bins-1));

                    pixel_count_array[bin]++;
                }
            }
            return this;
        }

        Histogram* saveHistogramToFile(string chartTitle) {
            std::ofstream out(("../public/charts/" + chartTitle + ".html").c_str());
            out << "<link href='../c3.min.css' rel='stylesheet' type='text/css'><script src='../d3.min.js'></script><script src='../c3.min.js'></script>";
            out << "<h3>" << chartTitle << "</h3><div id='chart'></div>";
            out << "<script type='text/javascript'>";
            out << "  c3.generate({";
            out << "    bindto: '#chart',";
            out << "    data: {";
            out << "      columns: [";
            out << "        ['Number of Pixels With Value', ";
            
            // print the data into the HTML string
            for (int i=0; i<this->number_of_bins; i++) {
                out << this->pixel_count_array[i] << ",";
            }

            out << "]], types: {";
            out << "'Number of Pixels With Value': 'area'";
            out << "}}});</script>";
            out.close();
            return this;
        }

        Histogram* normalize() {
            //get the maximum count in any bin
            int max_count = -1;
            for (int i=0; i<this->number_of_bins ;i++) {
                if (this->pixel_count_array[i] > max_count)
                    max_count = this->pixel_count_array[i];
            }

            //go through the bins and divide by the maximum
            for (int i=0; i<this->number_of_bins ;i++) {
                this->pixel_count_array[i] /= max_count;
            }

            return this;
        }

};

void plot_histogram(cimg_library::CImg<> &image, int number_of_bins) {
    for (int channel=0; channel<image.spectrum(); channel++) {
	    ostringstream os;
        os << "Input Image Channel " << (channel+1) << " of " << image.spectrum() << " with " << number_of_bins << " bins";
        string title = os.str();
        os << " (normalized)";
        string normaled_title = os.str();

        Histogram(image, number_of_bins, channel)
            .saveHistogramToFile(title) //save charts before normalizing
            ->normalize()
            ->saveHistogramToFile(normaled_title); //save normalized charts
    }
}

#endif