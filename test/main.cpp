#include <print>
#include <iostream>
#include <stdexcept>
#include <ZxFile/ZxFile.h>


auto main() -> int
{
    try
    {
        constexpr auto my_str = "1231313";
        std::span<const char> my_sp{ my_str,7 };
        ZQF::ZxFile::SaveDataViaPath("1.txt", my_sp, true);

        ZQF::ZxFile ofs{ "123.txt", ZQF::ZxFile::OpenMod::WriteSafe };
        ofs << 12;

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }

}
