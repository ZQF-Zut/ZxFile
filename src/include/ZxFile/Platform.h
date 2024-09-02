#pragma once
#include <span>
#include <memory>
#include <cstdint>
#include <optional>
#include <string_view>


namespace ZQF::ZxFilePrivate
{
    using FILE_HANLDE_TYPE = std::uintptr_t;
    constexpr FILE_HANLDE_TYPE FILE_HANLDE_INVALID = UINTPTR_MAX;

    enum class MoveWay : std::size_t
    {
        Set = static_cast<std::size_t>(0),
        Cur = static_cast<std::size_t>(1),
        End = static_cast<std::size_t>(2)
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
    auto Bytes(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>;

    // get current file pointer
    auto Tell(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>;

    // set file pointer
    auto Seek(const FILE_HANLDE_TYPE hFile, const std::uint64_t nOffset, const MoveWay eWay) -> std::optional<std::uint64_t>;

    // read data from file
    auto Read(const FILE_HANLDE_TYPE hFile, void* pBuffer, const std::size_t nBytes) -> std::optional<std::size_t>;

    // write data to file
    auto Write(const FILE_HANLDE_TYPE hFile, const void* pData, const std::size_t nBytes) -> std::optional<std::size_t>;
} // namespace ZQF::ZxFilePrivate
