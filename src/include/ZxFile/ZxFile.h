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
        template <class T> auto operator>>(T& rfData) -> ZxFile&;
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
        template <class T> auto Put(const T& rfData) -> ZxFile&;
        template <class T, std::size_t S> auto Write(const std::span<T, S> spData) const -> std::optional<std::size_t>;
        template <class T, std::size_t S> auto Read(const std::span<T, S> spBuffer) const -> std::optional<std::size_t>;

    public:
        template <class T, std::size_t S> static auto SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, const bool isForceSave = true, const bool isCreateDires = true) -> void;

    };

    template<class T>
    auto ZxFile::operator>>(T& rfData) -> ZxFile&
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
        {
            this->Read(std::span{ &rfData, 1 });
        }
        else
        {
            this->Read(std::span{ rfData });
        }

        return *this;
    }

    template<class T>
    auto ZxFile::operator<<(T&& rfData) -> ZxFile&
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
        {
            this->Write(std::span{ &rfData, 1 });
        }
        else
        {
            this->Write(std::span{ rfData });
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
    auto ZxFile::Put(const T& rfData) -> ZxFile&
    {
        return this->operator<<(rfData);
    }

    template <class T, std::size_t S>
    auto ZxFile::Write(const std::span<T, S> spData) const -> std::optional<std::size_t>
    {
        return ZxFilePrivate::Write(m_hFile, { reinterpret_cast<const std::uint8_t*>(spData.data()), spData.size_bytes() });
    }

    template <class T, std::size_t S>
    auto ZxFile::Read(const std::span<T, S> spBuffer) const -> std::optional<std::size_t>
    {
        return ZxFilePrivate::Read(m_hFile, { reinterpret_cast<std::uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
    }

    template <class T, std::size_t S>
    auto ZxFile::SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, const bool isForceSave, const bool isCreateDires) -> void
    {
        const auto status = ZxFilePrivate::SaveDataViaPathImp(msPath, { reinterpret_cast<const std::uint8_t*>(spData.data()), spData.size_bytes() }, isForceSave, isCreateDires);
        if (status == false) { throw std::runtime_error(std::string{ "ZxFile::SaveDataViaPath(): save data error! -> msPath: " }.append(msPath)); }
    }
}
