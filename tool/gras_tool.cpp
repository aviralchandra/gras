// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "gras_tool.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <vector>
#include <string>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    //variables to be set by po
    std::vector<std::string> sources;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("print-gras-root-dir", "print the GRAS root install directory")
        ("print-library-mod-dir", "print the GRAS library module directory")
        ("print-cmake-mod-dir", "print the GRAS cmake module directory")
        ("print-grc-blocks-dir", "print the GRC blocks directory")
        ("sources", po::value(&sources), "list of grc, python, c++ sources" )
    ;

    //setup positional program options
    po::positional_options_description p;
    p.add("sources", -1);

    //setup the variables map and parse
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(desc).positional(p).run(), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help"))
    {
        std::cout << boost::format("GRAS Tool %s") % desc << std::endl;
        return EXIT_FAILURE;
    }

    //print directories if queried for by the args
    if (vm.count("print-gras-root-dir")) std::cout << gras::get_gras_root_path().string() << std::endl;
    if (vm.count("print-library-mod-dir")) std::cout << gras::get_library_module_install_path().string() << std::endl;
    if (vm.count("print-cmake-mod-dir")) std::cout << gras::get_cmake_module_install_path().string() << std::endl;
    if (vm.count("print-grc-blocks-dir")) std::cout << gras::get_grc_blocks_install_path().string() << std::endl;

    BOOST_FOREACH( const std::string& i, sources )
    {
        std::cout << i << std::endl;
    }

    return EXIT_SUCCESS;
}