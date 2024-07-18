#pragma once
#include <format>
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
        ZxFilePrivate::FILE_HANLDE_TYPE m_hFile{};

    public:
        ZxFile(const std::string_view msPath, const OpenMod eMode)
        {
            if (const auto file_id_opt = ZxFilePrivate::Open(msPath, eMode))
            {
                m_hFile = *file_id_opt;
            }
            else
            {
                throw std::runtime_error(std::format("ZxFile: open file failed! -> msPath: {}", msPath));
            }
        }

        ~ZxFile()
        {
            this->Close();
        }

        auto Flush() const -> bool
        {
            return ZxFilePrivate::Flush(m_hFile);
        }

        auto Close() const -> bool
        {
            return ZxFilePrivate::Close(m_hFile);
        }

        template <class T, size_t S>
        auto Read(const std::span<T, S> spBuffer) const -> std::optional<size_t>
        {
            return ZxFilePrivate::Read(m_hFile, { reinterpret_cast<uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
        }

        template <class T, size_t S>
        auto Write(const std::span<T, S> spData) const -> std::optional<size_t>
        {
            return ZxFilePrivate::Write(m_hFile, { reinterpret_cast<const uint8_t*>(spData.data()), spData.size_bytes() });
        }

        auto SetPtr(const uint64_t nOffset, const MoveWay eWay) const -> std::optional<uint64_t>
        {
            return ZxFilePrivate::SetPtr(m_hFile, nOffset, eWay);
        }

        auto GetSize() const -> std::optional<uint64_t>
        {
            return ZxFilePrivate::GetSize(m_hFile);
        }

        template<class T>
        ZxFile& operator>>(T& rfData)
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
        ZxFile& operator<<(T&& rfData)
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
        auto Get() -> T
        {
            T tmp;
            this->operator>>(tmp);
            return tmp;
        }

        template <class T>
        auto Put(const T& rfData) -> ZxFile&
        {
            return this->operator<<(rfData);
        }

        template <class T, size_t S>
        static auto SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, const bool isForceSave = true, const bool isCreateDires = true) -> void
        {
           bool status = ZxFilePrivate::SaveDataViaPathImp(msPath, { reinterpret_cast<const std::uint8_t*>(spData.data()), spData.size_bytes() }, isForceSave, isCreateDires);
           if (status == false) { throw std::runtime_error(std::format("ZxFile::SaveDataViaPath<>(): save data error! -> msPath: {}", msPath)); }
        }
    };

}
