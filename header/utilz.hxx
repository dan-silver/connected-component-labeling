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
        float** pixel_count_array; //multi dimsional - first is channel index, then pixel count
        int number_of_bins, channels;

        Histogram(cimg_library::CImg<> &image, int number_of_bins) {
            this->image = image;
            this->number_of_bins = number_of_bins;
            this->channels = image.spectrum();

            //allocate the array
            pixel_count_array = new float*[this->channels];
            for(int channel = 0; channel < this->channels; channel++) {
                pixel_count_array[channel] = new float[number_of_bins];
                //initialize the pixel count array elements to zero
                for (int bin=0; bin < number_of_bins; bin++) {
                    pixel_count_array[channel][bin] = 0;
                }
            }


        }

        Histogram* placePixelValuesIntoBins() {
            int value;
            for (int channel=0; channel < this->channels; channel++) {            
                for (int r = 0 ; r < this->image.height(); r++) {
                    for (int c= 0 ; c < this->image.width(); c++) {
                        value = (int)image(c, r, 0, channel);

                        //calculate which bin to increment
                        int bin = floor((float)value/255 * (this->number_of_bins-1));

                        this->pixel_count_array[channel][bin]++;
                    }
                }
            }
            return this;
        }

        // creates an HTML page with an embeded chart
        Histogram* saveHistogramToFile(string chartTitle) {
            for (int channel=0; channel < this->channels; channel++) {

                ostringstream filename;
                filename << "../public/charts/" + chartTitle;
                filename << "_channel_";
                filename << channel;
                filename << ".html";
                string filename_str = filename.str();
                
                std::ofstream out(filename_str.c_str());
                out << "<link href='../c3.min.css' rel='stylesheet' type='text/css'><script src='../d3.min.js'></script><script src='../c3.min.js'></script>";
                out << "<h3>" << chartTitle << "</h3><div id='chart'></div>";
                out << "<script type='text/javascript'>";
                out << "  c3.generate({";
                out << "    bindto: '#chart',";
                out << "    data: {";
                out << "      columns: [";
                out << "        ['Number of Pixels Or Probability', ";
                
                // print the data into the HTML string
                for (int i=0; i<this->number_of_bins; i++) {
                    out << this->pixel_count_array[channel][i] << ",";
                }

                out << "]], types: {";
                out << "'Number of Pixels Or Probability': 'area'";
                out << "}}});</script>";
                out.close();
            }
            return this;
        }


        int* create_output_gray_map(int channel) {
            //array index = input gray value
            //array element value = output gray value
            int* output_gray_map = new int[number_of_bins];

            //calcuate the frequency sum
            int frequency_sum = 0;
            for (int i=0; i < this->number_of_bins; i++)
                frequency_sum += this->pixel_count_array[channel][i];
            
            //calculate the PDF
            float* pdf = new float[this->number_of_bins];
            for (int i=0; i < this->number_of_bins; i++) {
                pdf[i] = this->pixel_count_array[channel][i]/frequency_sum;
            }

            //calculate the CDF
            float* cdf = new float[this->number_of_bins];
            cdf[0] = 0;
            for (int i=1; i < this->number_of_bins; i++) {
                cdf[i] = cdf[i-1] + pdf[i];
            }


            // round(CDF*7)

            for (int i=0; i < this->number_of_bins; i++) {
                output_gray_map[i] = (int)(cdf[i] * 255);
            }

            return output_gray_map;
        }

        //performs histogram equalization
        Histogram* equalize(cimg_library::CImg<> &equalized_image) {
            equalized_image.assign(this->image.width(), this->image.height(), 1, this->channels);

            for (int channel=0; channel < this->channels; channel++) {
                int* input_output_gray_map = create_output_gray_map(channel);
                //loop through each pixel and replace it using the map
                int value;
                for (int r = 0 ; r < this->image.height(); r++) {
                    for (int c= 0 ; c < this->image.width(); c++) {
                        value = (int)image(c, r, 0, channel);
                        equalized_image(c, r, 0, channel) = input_output_gray_map[value];
                    }
                }

            }
            this->image = equalized_image;

            return this;
        }

        //rescale Y axis to [0, 1]
        Histogram* normalize() {
            for (int channel=0; channel<this->channels; channel++) {
                //get the maximum count in any bin
                int max_count = -1;
                for (int i=0; i<this->number_of_bins ;i++) {
                    if (this->pixel_count_array[channel][i] > max_count)
                        max_count = this->pixel_count_array[channel][i];
                }

                //go through the bins and divide by the maximum
                for (int i=0; i<this->number_of_bins ;i++) {
                    this->pixel_count_array[channel][i] /= max_count;
                }
            }

            return this;
        }

};

void plot_histogram(cimg_library::CImg<> &image, int number_of_bins) {
    ostringstream os;
    os << "Input Image " << " with " << number_of_bins << " bins";
    string title = os.str();
    os << " (normalized)";
    string normaled_title = os.str();

    Histogram(image, number_of_bins)
        .placePixelValuesIntoBins()
        ->saveHistogramToFile(title) //save charts before normalizing
        ->normalize() //rescale Y axis to [0, 1]
        ->saveHistogramToFile(normaled_title); //save normalized charts
}


// part B
// cimg_library::CImg<> run_histogram_equalization(cimg_library::CImg<> &image) {
//     // this time run histogram equalization
//     cimg_library::CImg<> equalized_image;
//     Histogram histogram = Histogram(image, 255);
//     for (int channel=0; channel<image.spectrum(); channel++) {
//         histogram.equalize(equalized_image)
//     }
//         ->placePixelValuesIntoBins()
//         ->saveHistogramToFile(equalized_title); //save equalized charts

//     return equalized_image;
// }

#endif