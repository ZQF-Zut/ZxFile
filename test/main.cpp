#include <print>
#include <iostream>
#include <stdexcept>
#include <ZxFile/ZxFile.h>


auto main() -> int
{
    try
    {
        ZQF::ZxFile ofs{ "123.txt", ZQF::ZxFile::OpenMod::WriteSafe };
        ofs << 12;

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }

}
