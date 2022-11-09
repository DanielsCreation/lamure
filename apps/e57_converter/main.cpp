// Copyright (c) 2014-2018 Bauhaus-Universitaet Weimar
// This Software is distributed under the Modified BSD License, see license.txt.
//
// Virtual Reality and Visualization Research Group 
// Faculty of Media, Bauhaus-Universitaet Weimar
// http://www.uni-weimar.de/medien/vr

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <lamure/types.h>
#ifdef PRE_FORMAT_E57_H_
#include <e57/E57Foundation.h>
#include <e57/E57Simple.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>


struct surfel {
    float x, y, z;
    uint8_t r, g, b;
};


int main(int argc, char *argv[]) {

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    namespace po = boost::program_options;
    const std::string exec_name = (argc > 0) ? boost::filesystem::basename(argv[0]) : "";

    // define command line options
    const std::string details_msg = "\nFor details use -h or --help option.\n";
    po::variables_map vm;
    po::positional_options_description pod;
    po::options_description od_hidden("hidden");
    po::options_description od_cmd("cmd");
    po::options_description od("Usage: " + exec_name + " INPUT" + " OUTPUT\n\n");

    pod.add("user_input", -1);
    po::store(po::command_line_parser(argc, argv).options(od_cmd).positional(pod).run(), vm);
    po::notify(vm);

    const auto user_input = vm["user_input"].as<std::vector<std::string>>();
    const auto input_dir = boost::filesystem::canonical(user_input[0]);
    const auto output_file = boost::filesystem::absolute(user_input[1]);

    if(user_input.size() != 2)
    {
        std::cerr << "Convestion mode needs one input dir "
                     "and one output file to be specified"
                  << details_msg;
        return EXIT_FAILURE;
    }

    boost::filesystem::path p(input_dir);
    std::vector<std::string> files;

    //format_e57::getFilenames(&input_dir, &files);

    //for(auto it = files.begin(); it != files.end(); it++)

    int counter = 0;
    for(auto it = files.begin(); counter != 5; it++)
    {
        {
            std::chrono::steady_clock::time_point start_reading = std::chrono::steady_clock::now();
            printf("reading...\n");
            const char *file = (*it).c_str();
            // format_e57::read(file, vec);
            std::chrono::steady_clock::time_point end_reading = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end_reading - start_reading).count() << " Sekunden f�r das Lesen von Nr.: " << counter << std::endl;

            std::chrono::steady_clock::time_point start_writing = std::chrono::steady_clock::now();
            printf("writing...\n");
            // format_e57::write(output_file, vec);
            std::chrono::steady_clock::time_point end_writing = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end_writing - start_writing).count() << " Sekunden f�r das Schreiben von Nr.: " << std::endl;
            counter++;
        }
    }
}
