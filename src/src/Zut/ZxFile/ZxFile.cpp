#include "ZxFile.h"


namespace ZQF::Zut
{
    ZxFile::ZxFile()
    {

    }

    ZxFile::ZxFile(const std::string_view msPath, const OpenMod eMode)
    {
        this->Open(msPath, eMode);
    }

    ZxFile::ZxFile(const std::u8string_view u8Path, const OpenMod eMode)
    {
        this->Open({ reinterpret_cast<const char*>(u8Path.data()), u8Path.size() }, eMode);
    }

    ZxFile::~ZxFile()
    {
        this->Close();
    }

    auto ZxFile::Open(const std::string_view msPath, const OpenMod eMode) -> void
    {
        if (this->IsOpen()) { throw std::runtime_error(std::string{ "ZxFile::Open(): already holds a file handle -> msPath: " }.append(msPath)); }
        const auto file_id_opt = ZxFilePlat::Open(msPath, eMode);
        if (file_id_opt.has_value() == false) { throw std::runtime_error(std::string{ "ZxFile::Open(): open file failed! -> msPath: " }.append(msPath)); }
        m_hFile = *file_id_opt;
    }

    auto ZxFile::OpenNoThrow(const std::string_view msPath, const OpenMod eMode) noexcept -> bool
    {
        if (this->IsOpen()) { return false; }
        const auto file_id_opt = ZxFilePlat::Open(msPath, eMode);
        if (file_id_opt.has_value() == false) { return false; }
        m_hFile = *file_id_opt;
        return true;
    }

    auto ZxFile::IsOpen() const -> bool
    {
        return m_hFile == ZxFilePlat::FILE_HANLDE_INVALID ? false : true;
    }

    auto ZxFile::Close() -> bool
    {
        if (this->IsOpen() == false) { return false; }
        const auto status = ZxFilePlat::Close(m_hFile);
        m_hFile = ZxFilePlat::FILE_HANLDE_INVALID;
        return status;
    }

    auto ZxFile::Flush() const -> bool
    {
        return ZxFilePlat::Flush(m_hFile);
    }

    auto ZxFile::Bytes() const -> std::optional<std::uint64_t>
    {
        return ZxFilePlat::Bytes(m_hFile);
    }

    auto ZxFile::Tell() const -> std::optional<std::uint64_t>
    {
        return ZxFilePlat::Tell(m_hFile);
    }

    auto ZxFile::Seek(const std::uint64_t nOffset, const MoveWay eWay) const -> std::optional<std::uint64_t>
    {
        return ZxFilePlat::Seek(m_hFile, nOffset, eWay);
    }

    ZxFile::operator bool() const
    {
        return this->IsOpen();
    }

    auto ZxFile::WriteBytes(const void* pData, const std::size_t nBytes) const -> std::optional<std::size_t>
    {
        return ZxFilePlat::Write(m_hFile, pData, nBytes);
    }

    auto ZxFile::ReadBytes(void* pBuffer, const std::size_t nBytes) const -> std::optional<std::size_t>
    {
        return ZxFilePlat::Read(m_hFile, pBuffer, nBytes);
    }

} // namespace ZQF::Zut
