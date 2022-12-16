// Copyright (c) 2014-2018 Bauhaus-Universitaet Weimar
// This Software is distributed under the Modified BSD License, see license.txt.
//
// Virtual Reality and Visualization Research Group 
// Faculty of Media, Bauhaus-Universitaet Weimar
// http://www.uni-weimar.de/medien/vr

#include <lamure/pre/io/format_pc_bin.h>
#include <lamure/pre/surfel.h>
#include <stdexcept>

namespace lamure
{
namespace pre
{

void format_pc_bin::
read(const std::string &filename, surfel_callback_funtion callback)
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("unable to open file " + filename);
    }

    file.seekg(std::ios::beg, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(std::ios::beg, std::ios::beg);
    size_t num_surfels = file_size / sizeof(surfel);

    for(uint64_t i = 0; i < num_surfels; ++i)
    {
        vec3r pos;
        file.read((char *)&pos.x, 8);
        file.read((char *)&pos.y, 8);
        file.read((char *)&pos.z, 8);

        vec3b color;
        file.read((char *)&color.r, 1);
        file.read((char *)&color.g, 1);
        file.read((char *)&color.b, 1);

        callback(surfel(vec3r(pos[0], pos[1], pos[2]), vec3b(color[0], color[1], color[2])));
    }
    file.close();
}

void format_pc_bin::
write(const std::string &filename, buffer_callback_function callback)
{
    std::ofstream file(filename, std::ios::out | std::ios::ate | std::ios::binary);

    if(!file.is_open())
        throw std::runtime_error("Unable to open file: " + filename);

    surfel_vector buffer;
    size_t counter = 0;

    while(true)
    {
        bool ret = callback(buffer);
        if(!ret)
            break;

        for(int i = 0; i < buffer.size(); i++)
        {
            file.write((char *)&(buffer.at(i).pos().x), sizeof(real));
            file.write((char *)&(buffer.at(i).pos().y), sizeof(real));
            file.write((char *)&(buffer.at(i).pos().z), sizeof(real));

            file.write((char *)&(buffer.at(i).color().r), sizeof(uint8_t));
            file.write((char *)&(buffer.at(i).color().g), sizeof(uint8_t));
            file.write((char *)&(buffer.at(i).color().b), sizeof(uint8_t));
        }
        counter += buffer.size();
        
    }

    file.close();
    LOGGER_TRACE("Output surfels: " << counter);
}

}
}

