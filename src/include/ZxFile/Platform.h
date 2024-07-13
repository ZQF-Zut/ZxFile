#pragma once
#include <span>
#include <memory>
#include <optional>
#include <string_view>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif __linux__
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif


namespace ZQF::ZxFilePrivate
{
    enum class OpenMod
    {
        ReadSafe,
        WriteSafe,
        WriteForce
    };

#ifdef _WIN32
    using FILE_HANLDE_TYPE = void*;
    enum class MoveWay : size_t
    {
        Beg = static_cast<size_t>(FILE_BEGIN),
        Cur = static_cast<size_t>(FILE_CURRENT),
        End = static_cast<size_t>(FILE_END)
    };
#elif __linux__
    enum class MoveWay : size_t
    {
        Beg = static_cast<size_t>(SEEK_SET),
        Cur = static_cast<size_t>(SEEK_CUR),
        End = static_cast<size_t>(SEEK_END)
    };
    using FILE_HANLDE_TYPE = int;
#endif


    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void;


    // open file via utf8 path
    auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>;

    // close file
    auto Close(FILE_HANLDE_TYPE hFile) -> bool;

    // flush file
    auto Flush(FILE_HANLDE_TYPE hFile) -> bool;

    // get file size
    auto GetSize(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>;

    // get current file pointer
    auto GetPtr(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>;

    // set file pointer
    auto SetPtr(FILE_HANLDE_TYPE hFile, uint64_t nOffset, MoveWay eWay) -> std::optional<uint64_t>;

    // read data from file
    auto Read(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spBuffer) -> std::optional<size_t>;

    // write data to file
    auto Write(FILE_HANLDE_TYPE hFile, const std::span<const uint8_t> spData) -> std::optional<size_t>;
} // namespace ZQF::ZxJson
