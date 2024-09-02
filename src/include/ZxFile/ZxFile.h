#pragma once
#include <stdexcept>
#include <ZxFile/Platform.h>


namespace ZQF
{
    class ZxFile
    {
    public:
        using OpenMod = ZxFilePrivate::OpenMod;
        using MoveWay = ZxFilePrivate::MoveWay;

    private:
        ZxFilePrivate::FILE_HANLDE_TYPE m_hFile{ ZxFilePrivate::FILE_HANLDE_INVALID };

    public:
        ZxFile();
        ZxFile(const std::string_view msPath, const OpenMod eMode);
        ZxFile(const std::u8string_view u8Path, const OpenMod eMode);
        ~ZxFile();

    public:
        explicit operator bool() const;
        template <class T> auto operator>>(T&& rfData) -> ZxFile&;
        template <class T> auto operator<<(T&& rfData) -> ZxFile&;

    public:
        auto Open(const std::string_view msPath, const OpenMod eMode) -> void;
        auto OpenNoThrow(const std::string_view msPath, const OpenMod eMode) noexcept -> bool;
        auto IsOpen() const -> bool;
        auto Close() -> bool;
        auto Flush() const -> bool;
        auto Bytes() const->std::optional<std::uint64_t>;
        auto Tell() const->std::optional<std::uint64_t>;
        auto Seek(const std::uint64_t nOffset, const MoveWay eWay) const -> std::optional<std::uint64_t>;

    public:
        template <class T> auto Get() -> T;
        template <class T> auto Put(T&& rfData) -> ZxFile&;
        template <class T, std::size_t S> auto Write(const std::span<T, S> spData) const -> std::optional<std::size_t>;
        template <class T, std::size_t S> auto Read(const std::span<T, S> spBuffer) const -> std::optional<std::size_t>;
        auto WriteBytes(const void* pData, const std::size_t nBytes) const -> std::optional<std::size_t>;
        auto ReadBytes(void* pBuffer, const std::size_t nBytes) const ->std::optional<std::size_t>;

    public:
        template <class T, std::size_t S> static auto SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, const bool isForceSave = true, const bool isCreateDires = true) -> void;

    };

    template<class>
    struct is_std_span : std::false_type {};

    template<class T, std::size_t Extent>
    struct is_std_span<std::span<T, Extent>> : std::true_type {};

    template<class T>
    inline constexpr bool is_std_span_v = is_std_span<T>::value;

    template<class T>
    auto ZxFile::operator>>(T&& rfData) -> ZxFile&
    {
        if constexpr (is_std_span_v<std::decay_t<decltype(rfData)>>)
        {
            this->Read(rfData);
        }
        else
        {
            this->ReadBytes(&rfData, sizeof(rfData));
        }
        
        return *this;
    }

    template<class T>
    auto ZxFile::operator<<(T&& rfData) -> ZxFile&
    {
        if constexpr (is_std_span_v<std::decay_t<decltype(rfData)>>)
        {
            this->Write(rfData);
        }
        else
        {
            this->WriteBytes(&rfData, sizeof(rfData));
        }
        
        return *this;
    }

    template <class T>
    auto ZxFile::Get() -> T
    {
        T tmp;
        this->operator>>(tmp);
        return tmp;
    }

    template <class T>
    auto ZxFile::Put(T&& rfData) -> ZxFile&
    {
        return this->operator<<(std::forward<T>(rfData));
    }

    template <class T, std::size_t S>
    auto ZxFile::Write(const std::span<T, S> spData) const -> std::optional<std::size_t>
    {
        return this->WriteBytes(spData.data(), spData.size_bytes());
    }

    template <class T, std::size_t S>
    auto ZxFile::Read(const std::span<T, S> spBuffer) const -> std::optional<std::size_t>
    {
        return this->ReadBytes(spBuffer.data(), spBuffer.size_bytes());
    }

    template <class T, std::size_t S>
    auto ZxFile::SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, const bool isForceSave, const bool isCreateDires) -> void
    {
        const auto status = ZxFilePrivate::SaveDataViaPathImp(msPath, { reinterpret_cast<const std::uint8_t*>(spData.data()), spData.size_bytes() }, isForceSave, isCreateDires);
        if (status == false) { throw std::runtime_error(std::string{ "ZxFile::SaveDataViaPath(): save data error! -> msPath: " }.append(msPath)); }
    }
}
