#include <ZxFile/Platform.h>
#include <format>

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
#ifdef _WIN32
    static auto PathUTF8ToWide(const std::string_view msPath) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
    {
        const auto buffer_max_chars = ((msPath.size() * sizeof(char)) + 1) * 2;
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
        const auto char_count_real = static_cast<std::size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(buffer_max_chars)));
        buffer[char_count_real] = {};
        const std::wstring_view cvt_sv{ buffer.get(), char_count_real };
        return { cvt_sv, std::unique_ptr<wchar_t[]>{ std::move(buffer) } };
    }

    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool
    {
        auto fn_create_directories = [](const std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>& rfWidePath) -> void
            {
                auto& [path_wide_sv, path_wide_buffer] = rfWidePath;

                const auto pos = path_wide_sv.rfind(L'/');
                if ((pos == std::wstring_view::npos) || (pos == 1)) { return; }

                const auto file_name_mask_char_tmp = path_wide_buffer.get()[pos + 1];
                path_wide_buffer.get()[pos + 1] = {};
                {
                    wchar_t* cur_path_cstr = path_wide_buffer.get();
                    const wchar_t* org_path_cstr = path_wide_buffer.get();

                    while (*cur_path_cstr++ != L'\0')
                    {
                        if (*cur_path_cstr != L'/') { continue; }

                        const wchar_t slash_char_tmp = *cur_path_cstr;
                        *cur_path_cstr = {};
                        {
                            if (::GetFileAttributesW(org_path_cstr) == INVALID_FILE_ATTRIBUTES)
                            {
                                ::CreateDirectoryW(org_path_cstr, nullptr);
                            }
                        }
                        *cur_path_cstr = slash_char_tmp;
                        cur_path_cstr++;
                    }
                }
                path_wide_buffer.get()[pos + 1] = file_name_mask_char_tmp;
            };

        const auto wide_path = PathUTF8ToWide(msPath);
        if (isCreateDirectories) { fn_create_directories(wide_path); }
        const auto hfile = ::CreateFileW(wide_path.first.data(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, isCoverExists ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) { return false; }
        DWORD write{};
        (void)::WriteFile(hfile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &write, nullptr);
        (void)::CloseHandle(hfile);
        return true;
    }


    auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>
    {
        DWORD access{}, attributes{};
        switch (eMode)
        {
        case OpenMod::ReadSafe:
            access = GENERIC_READ;
            attributes = OPEN_EXISTING;
            break;
        case OpenMod::WriteSafe:
            access = GENERIC_WRITE;
            attributes = CREATE_NEW;
            break;
        case OpenMod::WriteForce:
            access = GENERIC_WRITE;
            attributes = CREATE_ALWAYS;
            break;
        }

        const auto handle = ::CreateFileW(PathUTF8ToWide(msPath).first.data(), access, FILE_SHARE_READ, nullptr, attributes, FILE_ATTRIBUTE_NORMAL, nullptr);
        return (handle == INVALID_HANDLE_VALUE) ? std::nullopt : std::optional{ reinterpret_cast<std::uintptr_t>(handle) };
    }

    auto Close(const FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::CloseHandle(reinterpret_cast<HANDLE>(hFile)) != FALSE;
    }

    auto Flush(const FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::FlushFileBuffers(reinterpret_cast<const HANDLE>(hFile)) != FALSE;
    }

    auto Bytes(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        LARGE_INTEGER file_size{};
        return ::GetFileSizeEx(reinterpret_cast<const HANDLE>(hFile), &file_size) ? std::optional{ static_cast<std::uint64_t>(file_size.QuadPart) } : std::nullopt;
    }

    auto Tell(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        LARGE_INTEGER new_pos;
        const LARGE_INTEGER move_distance{ .QuadPart = 0 };
        const auto status = (::SetFilePointerEx(reinterpret_cast<const HANDLE>(hFile), move_distance, &new_pos, FILE_CURRENT) != FALSE);
        return status ? std::optional<std::uint64_t>{ static_cast<std::uint64_t>(new_pos.QuadPart) } : std::nullopt;
    }

    auto Seek(const FILE_HANLDE_TYPE hFile, const std::uint64_t nOffset, const MoveWay eWay) -> std::optional<std::uint64_t>
    {
        LARGE_INTEGER new_pos;
        const LARGE_INTEGER move_distance = { .QuadPart = static_cast<LONGLONG>((nOffset)) };
        const auto status = (::SetFilePointerEx(reinterpret_cast<const HANDLE>(hFile), move_distance, &new_pos, static_cast<DWORD>(eWay)) != FALSE);
        return status ? std::optional<std::uint64_t>{ static_cast<std::uint64_t>(new_pos.QuadPart) } : std::nullopt;
    }

    auto Read(const FILE_HANLDE_TYPE hFile, const std::span<std::uint8_t> spBuffer) -> std::optional<std::size_t>
    {
        DWORD read{};
        const auto status = (::ReadFile(reinterpret_cast<const HANDLE>(hFile), spBuffer.data(), static_cast<DWORD>(spBuffer.size_bytes()), &read, nullptr) != FALSE);
        return status ? std::optional<std::size_t>{ static_cast<std::size_t>(read) } : std::nullopt;
    }

    auto Write(const FILE_HANLDE_TYPE hFile, const std::span<const std::uint8_t> spData) -> std::optional<std::size_t>
    {
        DWORD written{};
        const auto status = (::WriteFile(reinterpret_cast<const HANDLE>(hFile), spData.data(), static_cast<DWORD>(spData.size_bytes()), &written, nullptr) != FALSE);
        return status ? std::optional<std::size_t>{ static_cast<std::size_t>(written) } : std::nullopt;
    }
#else
    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const std::uint8_t> spData, const bool isCoverExists, const bool isCreateDirectories) -> bool
    {
        auto fn_create_directories = [](const std::string_view msPath) -> void
        {
            const auto pos = msPath.rfind('/');
            if ((pos == std::string_view::npos) || (pos == 1)) { return; }

            auto path_buffer = std::make_unique_for_overwrite<char[]>(msPath.size());
            std::memcpy(path_buffer.get(), msPath.data(), msPath.size());

            path_buffer.get()[pos + 1] = {}; // rm file_name

            char* cur_path_cstr = path_buffer.get();
            const char* org_path_cstr = path_buffer.get();

            while (*cur_path_cstr++ != '\0')
            {
                if (*cur_path_cstr != '/') { continue; }

                const char slash_char_tmp = *cur_path_cstr;
                *cur_path_cstr = {};
                {
                    if (::access(org_path_cstr, X_OK) == -1)
                    {
                        ::mkdir(org_path_cstr, 0777);
                    }
                }
                *cur_path_cstr = slash_char_tmp;
                cur_path_cstr++;
            }
        };

        if (isCreateDirectories) { fn_create_directories(msPath); }
        constexpr auto create_always = O_CREAT | O_WRONLY | O_TRUNC;
        constexpr auto create_new = O_CREAT | O_WRONLY | O_EXCL;
        const auto file_handle = ::open(msPath.data(), isCoverExists ? create_always : create_new, 0666);  // NOLINT
        if (file_handle == -1) { return false; }
        ::write(file_handle, spData.data(), spData.size_bytes());
        ::close(file_handle);
        return true;
    }

    auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>
    {
        int open_mode{};
        switch (eMode)
        {
        case OpenMod::ReadSafe: open_mode = O_RDONLY; break;
        case OpenMod::WriteSafe: open_mode = O_CREAT | O_WRONLY | O_EXCL; break;
        case OpenMod::WriteForce: open_mode = O_CREAT | O_WRONLY | O_TRUNC; break;
        }
        const auto file_handle = ::open(msPath.data(), open_mode, 0666);
        return (file_handle == -1) ? std::nullopt : std::optional{ static_cast<std::uintptr_t>(file_handle) };
    }

    auto Close(const FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::close(static_cast<int>(hFile)) ? true : false;
    }

    auto Flush(const FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::fsync(static_cast<int>(hFile)) ? true : false;
    }

    auto Bytes(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        struct ::stat s;
        return (::fstat(static_cast<int>(hFile), &s) == -1) ? std::nullopt : std::optional{ static_cast<std::uint64_t>(s.st_size) };
    }

    auto Tell(const FILE_HANLDE_TYPE hFile) -> std::optional<std::uint64_t>
    {
        const auto pos = ::lseek64(static_cast<int>(hFile), 0, SEEK_CUR);
        return (pos == -1) ? std::nullopt : std::optional{ static_cast<std::uint64_t>(pos) };
    }

    auto Seek(const FILE_HANLDE_TYPE hFile, const std::uint64_t nOffset, const MoveWay eWay) -> std::optional<std::uint64_t>
    {
        const auto pos = ::lseek64(static_cast<int>(hFile), static_cast<loff_t>(nOffset), static_cast<int>(eWay));
        return (pos == -1) ? std::nullopt : std::optional{ static_cast<std::uint64_t>(pos) };
    }

    auto Read(const FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spBuffer) -> std::optional<std::size_t>
    {
        const auto read_bytes = ::read(static_cast<int>(hFile), spBuffer.data(), spBuffer.size_bytes());
        return read_bytes != -1 ? std::optional{ static_cast<std::size_t>(read_bytes) } : std::nullopt;
    }

    auto Write(const FILE_HANLDE_TYPE hFile, const std::span<const std::uint8_t> spData) -> std::optional<std::size_t>
    {
        const auto written_bytes = ::write(static_cast<int>(hFile), spData.data(), spData.size_bytes());
        return written_bytes != -1 ? std::optional{ static_cast<std::size_t>(written_bytes) } : std::nullopt;
    }
#endif
} // namespace ZQF::ZxFilePrivate
