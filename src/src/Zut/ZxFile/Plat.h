#pragma once
#include <span>
#include <cstdint>
#include <optional>
#include <string_view>

namespace ZQF::Zut::ZxFilePlat
{
  using FILE_HANLDE_TYPE = std::uintptr_t;
  constexpr FILE_HANLDE_TYPE FILE_HANLDE_INVALID = UINTPTR_MAX; // NOLINT

  enum class MoveWay : std::uint8_t
  {
    Set = 0,
    Cur = 1,
    End = 2
  };

  enum class OpenMod : std::uint8_t
  {
    ReadExists,
    WriteExists,
    WriteNew,
    WriteForce,
    ReadWriteExists,
    ReadWriteNew,
    ReadWriteForce
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
  auto Seek(const FILE_HANLDE_TYPE hFile, const std::int64_t nOffset, const MoveWay eWay) -> std::optional<std::int64_t>;

  // read data from file
  auto Read(const FILE_HANLDE_TYPE hFile, void* pBuffer, const std::size_t nBytes) -> std::optional<std::size_t>;

  // write data to file
  auto Write(const FILE_HANLDE_TYPE hFile, const void* pData, const std::size_t nBytes) -> std::optional<std::size_t>;
} // namespace ZQF::Zut::ZxFilePlat
