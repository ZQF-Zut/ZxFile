#include <print>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <array>
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
            if (ofs)
            {
                ofs << std::uint32_t(12);
                ofs << std::uint64_t(0x112233);
            }
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
        assert(ofs.Bytes().value() == 12);

        ofs.Seek(4, ZQF::ZxFile::MoveWay::Set);
        assert(ofs.Get<uint64_t>() == 0x112233);

        ofs.Seek(0, ZQF::ZxFile::MoveWay::Set);
        assert(ofs.Get<uint32_t>() == 12);

        {
            std::array<char8_t, 22> u8_buffer_raw = { u8"乱七八糟的内容" };

            ZQF::ZxFile ofs_u8{ u8"乱七八糟的文件名.txt", ZQF::ZxFile::OpenMod::WriteForce };
            ofs_u8 << std::span{ std::string_view("乱七八糟的内容") };
            ofs_u8.Close();

            ZQF::ZxFile ifs_u8{ u8"乱七八糟的文件名.txt", ZQF::ZxFile::OpenMod::ReadSafe };
            std::array<char8_t, 22> u8_buffer_read{};
            ifs_u8 >> u8_buffer_read;
            assert(u8_buffer_raw == u8_buffer_read);
        }

        ZQF::ZxFile noexcept_open;
        assert(noexcept_open.OpenNoThrow("1290u012nf812", ZQF::ZxFile::OpenMod::ReadSafe) == false);

    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }

}
