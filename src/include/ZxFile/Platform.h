#pragma once
#include <span>
#include <memory>
#include <cstdint>
#include <optional>
#include <string_view>


namespace ZQF::ZxFilePrivate
{
#ifdef _WIN32
    using FILE_HANLDE_TYPE = void*;
#elif __linux__
    using FILE_HANLDE_TYPE = int;
#endif

    enum class MoveWay : size_t
    {
        Beg = static_cast<size_t>(0),
        Cur = static_cast<size_t>(1),
        End = static_cast<size_t>(2)
    };

    enum class OpenMod
    {
        ReadSafe,
        WriteSafe,
        WriteForce
    };

    // save data
    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool;

    // open file via utf8 path
    auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>;

    // close file
    auto Close(const FILE_HANLDE_TYPE hFile) -> bool;

    // flush file
    auto Flush(const FILE_HANLDE_TYPE hFile) -> bool;

    // get file size
    auto GetSize(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>;

    // get current file pointer
    auto GetPtr(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>;

    // set file pointer
    auto SetPtr(const FILE_HANLDE_TYPE hFile, const std::uint64_t nOffset, const MoveWay eWay) -> std::optional<std::uint64_t>;

    // read data from file
    auto Read(const FILE_HANLDE_TYPE hFile, const std::span<std::uint8_t> spBuffer) -> std::optional<std::size_t>;

    // write data to file
    auto Write(const FILE_HANLDE_TYPE hFile, const std::span<const std::uint8_t> spData) -> std::optional<std::size_t>;
} // namespace ZQF::ZxFilePrivate
