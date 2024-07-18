#include <ZxFile/Platform.h>
#include <span>
#include <format>


namespace ZQF::ZxFilePrivate
{
#ifdef _WIN32
    static auto PathUtf8ToWide(const std::string_view msPath) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
    {
        const auto char_count = static_cast<size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), nullptr, 0));
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(char_count + 1);
        const auto char_count_real = static_cast<size_t>(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(char_count)));
        buffer[char_count_real] = {};
        return { { buffer.get(), char_count }, std::move(buffer) };
    }

    static auto CreateDirectories(std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>& rfWidePath) -> void
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
    }

    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<const uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void
    {
        auto wide_path = PathUtf8ToWide(msPath);
        if (isCreateDirectories) { CreateDirectories(wide_path); }
        const HANDLE hfile = ::CreateFileW(wide_path.first.data(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, isCoverExists ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) { throw std::runtime_error(std::format("ZxFile::SaveDataViaPathImp(): create file error!. msPath: {}", msPath)); }
        DWORD write{};
        (void)::WriteFile(hfile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &write, nullptr);
        ::CloseHandle(hfile);
    }


    auto Open(const std::string_view msPath, OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>
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

        return ::CreateFileW(PathUtf8ToWide(msPath).first.data(), access, FILE_SHARE_READ, nullptr, attributes, FILE_ATTRIBUTE_NORMAL, nullptr);
    }

    auto Close(FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::CloseHandle(hFile) != FALSE;
    }

    auto Flush(FILE_HANLDE_TYPE hFile) -> bool
    {
        return ::FlushFileBuffers(hFile) != FALSE;
    }

    auto GetSize(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>
    {
        LARGE_INTEGER file_size{};
        return ::GetFileSizeEx(hFile, &file_size) ? std::optional{ static_cast<uint64_t>(file_size.QuadPart) } : std::nullopt;
    }

    auto GetPtr(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>
    {
        LARGE_INTEGER new_file_pointer;
        LARGE_INTEGER move_distance{ .QuadPart = 0 };
        bool status = (::SetFilePointerEx(hFile, move_distance, &new_file_pointer, FILE_CURRENT) != FALSE);
        return status ? std::optional<uint64_t>{ static_cast<uint64_t>(new_file_pointer.QuadPart) } : std::nullopt;
    }

    auto SetPtr(FILE_HANLDE_TYPE hFile, uint64_t nOffset, MoveWay eWay) -> std::optional<uint64_t>
    {
        LARGE_INTEGER new_file_pointer;
        LARGE_INTEGER move_distance = { .QuadPart = static_cast<LONGLONG>((nOffset)) };
        bool status = (::SetFilePointerEx(hFile, move_distance, &new_file_pointer, static_cast<DWORD>(eWay)) != FALSE);
        return status ? std::optional<uint64_t>{ static_cast<uint64_t>(new_file_pointer.QuadPart) } : std::nullopt;
    }

    auto Read(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spBuffer) -> std::optional<size_t>
    {
        DWORD read{};
        bool status = (::ReadFile(hFile, spBuffer.data(), static_cast<DWORD>(spBuffer.size_bytes()), &read, nullptr) != FALSE);
        return status ? std::optional<size_t>{ static_cast<size_t>(read) } : std::nullopt;
    }

    auto Write(FILE_HANLDE_TYPE hFile, const std::span<const uint8_t> spData) -> std::optional<size_t>
    {
        DWORD written{};
        bool status = (::WriteFile(hFile, spData.data(), static_cast<DWORD>(spData.size_bytes()), &written, nullptr) != FALSE);
        return status ? std::optional<size_t>{ static_cast<size_t>(written) } : std::nullopt;
    }
#else
    static auto CreateDirectories(const std::string_view msPath) -> void
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
    }

    auto SaveDataViaPathImp(const std::string_view msPath, const std::span<uint8_t> spData, bool isCoverExists, bool isCreateDirectories) -> void
    {
        if (isCreateDirectories) { ZxFilePrivate::CreateDirectories(msPath); }
        constexpr auto create_always = O_CREAT | O_WRONLY | O_TRUNC;
        constexpr auto create_new = O_CREAT | O_WRONLY | O_EXCL;
        const auto file_handle = ::open(msPath.data(), isCoverExists ? create_always : create_new, 0666);  // NOLINT
        if (file_handle == -1) { throw std::runtime_error(std::format("ZxMem::RreadAllBytes(): create file failed!", msPath)); }
        ::write(file_handle, spData.data(), spData.size_bytes());
        ::close(file_handle);
    }


    auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<FILE_HANLDE_TYPE>
    {
        return {};
    }

    auto Close(FILE_HANLDE_TYPE hFile) -> bool
    {
        ::close(hFile);
        return true;
    }

    auto Flush(FILE_HANLDE_TYPE hFile) -> bool
    {
        return true;
    }

    auto GetSize(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>
    {
        const auto file_size = ::lseek(hFile, 0, SEEK_END);
        ::lseek(hFile, 0, SEEK_SET);
        return static_cast<size_t>(file_size);
    }

    auto GetPtr(FILE_HANLDE_TYPE hFile) -> std::optional<uint64_t>
    {

    }

    auto SetPtr(FILE_HANLDE_TYPE hFile, uint64_t nOffset, MoveWay eWay) -> std::optional<uint64_t>
    {

    }

    auto Read(FILE_HANLDE_TYPE hFile, const std::span<uint8_t> spBuffer) -> std::optional<size_t>
    {
        ::read(hFile, spBuffer.data(), spBuffer.size_bytes());
    }

    auto Write(FILE_HANLDE_TYPE hFile, const std::span<const uint8_t> spData) -> std::optional<size_t>
    {

    }
#endif
} // namespace ZQF::ZxMem
