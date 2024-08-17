#include <ZxFile/ZxFile.h>


namespace ZQF
{
    ZxFile::ZxFile()
    {

    }

    ZxFile::ZxFile(const std::string_view msPath, const OpenMod eMode)
    {
        this->Open(msPath, eMode);
    }

    ZxFile::ZxFile(const std::u8string_view phPath, const OpenMod eMode)
    {
        this->Open({ reinterpret_cast<const char*>(phPath.data()), phPath.size() }, eMode);
    }

    ZxFile::~ZxFile()
    {
        this->Close();
    }

    auto ZxFile::Open(const std::string_view msPath, const OpenMod eMode) -> void
    {
        if (m_hFile != ZxFilePrivate::FILE_HANLDE_INVALID)
        {
            throw std::runtime_error(std::format("ZxFile: already holds a file handle -> msPath: {}", msPath));
        }

        if (const auto file_id_opt = ZxFilePrivate::Open(msPath, eMode))
        {
            m_hFile = *file_id_opt;
        }
        else
        {
            throw std::runtime_error(std::format("ZxFile: open file failed! -> msPath: {}", msPath));
        }
    }

    auto ZxFile::IsOpen() const -> bool
    {
        return m_hFile == ZxFilePrivate::FILE_HANLDE_INVALID ? false : true;
    }

    auto ZxFile::Close() -> bool
    {
        if (m_hFile == ZxFilePrivate::FILE_HANLDE_INVALID) { return true; }
        bool status = ZxFilePrivate::Close(m_hFile);
        m_hFile = ZxFilePrivate::FILE_HANLDE_INVALID;
        return status;
    }

    auto ZxFile::Flush() const -> bool
    {
        return ZxFilePrivate::Flush(m_hFile);
    }

    auto ZxFile::Bytes() const -> std::optional<std::uint64_t>
    {
        return ZxFilePrivate::Bytes(m_hFile);
    }

    auto ZxFile::Tell() const -> std::optional<std::uint64_t>
    {
        return ZxFilePrivate::Tell(m_hFile);
    }

    auto ZxFile::Seek(const std::uint64_t nOffset, const MoveWay eWay) const -> std::optional<std::uint64_t>
    {
        return ZxFilePrivate::Seek(m_hFile, nOffset, eWay);
    }

    ZxFile::operator bool() const
    {
        return this->IsOpen();
    }

} // namespace ZQF
