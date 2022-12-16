// Copyright (c) 2014-2018 Bauhaus-Universitaet Weimar
// This Software is distributed under the Modified BSD License, see license.txt.
//
// Virtual Reality and Visualization Research Group 
// Faculty of Media, Bauhaus-Universitaet Weimar
// http://www.uni-weimar.de/medien/vr

#include <lamure/pre/io/format_bin.h>
#include <lamure/pre/node_serializer.h>
#include <stdexcept>

namespace lamure
{
namespace pre
{

void format_bin::
read(const std::string &filename, surfel_callback_funtion callback)
{
    
    shared_surfel_file input_file_disk_access = std::make_shared<surfel_file>();
    surfel_disk_array input;

    input_file_disk_access->open(filename);
    size_t input_size = input_file_disk_access->get_size();
    input = surfel_disk_array(input_file_disk_access, 0, input_size);
    input_file_disk_access->close();

    for(size_t k = 0; k < input_size; ++k)
    {
        surfel surf = input.read_surfel(k);
        vec3r pos = surf.pos();
        vec3b color = surf.color();
        real radius = surf.radius();
        vec3f normal = surf.normal();

        /*std::cout << "pos: " << surf.pos() << std::endl;
        std::cout << "color: " << "(" << int(surf.color().r) << " " << int(surf.color().g) << " " << int(surf.color().b) << ")" << std::endl;
        std::cout << "radius: " << surf.radius() << std::endl;
        std::cout << "normal: " << surf.normal() << std::endl;
        std::cin.ignore();*/

        callback(surfel(vec3r(pos.x, pos.y, pos.z), vec3b(color.r, color.g, color.b), radius, vec3f(normal.x, normal.y, normal.z)));
    }

    LOGGER_INFO("Total number of surfels: " << input.length());
    
    //input.close();

    /*
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
        file.read((char *)&color.x, 1);
        file.read((char *)&color.y, 1);
        file.read((char *)&color.z, 1);

        float radius;
        file.read((char *)&radius, 8);

        vec3f normal;
        file.read((char *)&normal.x, 4);
        file.read((char *)&normal.y, 4);
        file.read((char *)&normal.z, 4);

        //std::cout << "pos: " << "(" << pos.x << " " << pos.y << " " << pos.z << ")" << std::endl;
        //std::cout << "color: " << "(" << int(color.r) << " " << int(color.g) << " " << int(color.b) << ")" << std::endl;
        //std::cout << "radius: " << radius << std::endl;
        //std::cout << "normal: " << "(" << normal.x << " " << normal.y << " " << normal.z << ")" << std::endl;
        

        callback(surfel(vec3r(pos.x, pos.y, pos.z), vec3b(color.r, color.g, color.b), radius, vec3f(normal.x, normal.y, normal.z)));
    }
    file.close();
    */
}

void format_bin::
write(const std::string &filename, buffer_callback_function callback)
{
    surfel_file file;
    //std::ofstream file(filename, std::ios::out | std::ios::binary);

    surfel_vector buffer;
    size_t count = 0;

    file.open(filename, true);
    while(true)
    {
        bool ret = callback(buffer);
        if(!ret)
            break;

        file.append(&buffer);
        count += buffer.size();
    }
    file.close();
    /*
    if(!file.is_open())
        throw std::runtime_error("Unable to open file: " + filename);
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

            file.write((char *)&(buffer.at(i).radius()), sizeof(real));

            file.write((char *)&(buffer.at(i).normal().x), sizeof(float));
            file.write((char *)&(buffer.at(i).normal().y), sizeof(float));
            file.write((char *)&(buffer.at(i).normal().z), sizeof(float));

            std::cout << "pos: " << buffer.at(i).pos() << std::endl;
            std::cout << "color: " << "(" << int(buffer.at(i).color().r) << " " << int(buffer.at(i).color().g) << " " << int(buffer.at(i).color().b) << ")" << std::endl;
            std::cout << "radius: " << buffer.at(i).radius() << std::endl;
            std::cout << "normal: " << buffer.at(i).normal() << std::endl;

            std::cout << "sizeof(buffer.at(i)): " << sizeof(buffer.at(i)) << std::endl;
            std::cout << "sizeof(buffer.at(i))/buffer.size(): " << sizeof(buffer.at(i)) / buffer.size() << std::endl;
            std::cout << "buffer.size(): " << buffer.size() << std::endl;
            std::cout << "sizeof(buffer.at(i).pos()): " << sizeof(buffer.at(i).pos()) << std::endl;
            std::cout << "sizeof(buffer.at(i).color()): " << sizeof(buffer.at(i).color()) << std::endl;
            std::cout << "sizeof(buffer.at(i).radius()): " << sizeof(buffer.at(i).radius()) << std::endl;
            std::cout << "sizeof(buffer.at(i).normal()): " << sizeof(buffer.at(i).normal()) << std::endl;
            std::cin.ignore();
        }
        
        count += buffer.size();
    }
    
    file.close();
    */
    LOGGER_TRACE("Output surfels: " << count);
}

}
}

