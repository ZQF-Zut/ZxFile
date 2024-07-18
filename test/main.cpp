#include <print>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <ZxFile/ZxFile.h>


auto main() -> int
{
    try
    {
        constexpr auto my_str = "1231313";
        std::span<const char> my_sp{ my_str,7 };
        ZQF::ZxFile::SaveDataViaPath("1.txt", my_sp, true);

        {
            ZQF::ZxFile ofs{ "123.txt", ZQF::ZxFile::OpenMod::WriteForce };
            ofs << std::uint32_t(12);
            ofs << std::uint64_t(0x112233);
        }

        try
        {
            ZQF::ZxFile ofs{ "123.txt", ZQF::ZxFile::OpenMod::WriteSafe };
            assert(false);
        }
        catch ([[maybe_unused]] const std::exception& err)
        {

        }


        ZQF::ZxFile ofs{ "123.txt", ZQF::ZxFile::OpenMod::ReadSafe };
        assert(ofs.GetSize().value() == 12);

        ofs.SetPtr(4, ZQF::ZxFile::MoveWay::Beg);
        assert(ofs.Get<uint64_t>() == 0x112233);

        ofs.SetPtr(0, ZQF::ZxFile::MoveWay::Beg);
        assert(ofs.Get<uint32_t>() == 12);

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }

}
