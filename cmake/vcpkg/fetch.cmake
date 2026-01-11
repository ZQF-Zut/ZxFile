include_guard(GLOBAL)

function(zqf_vcpkg_fetch)
  set(zqf_vcpkg_root_dir "${CMAKE_SOURCE_DIR}/.vcpkg")

  if(EXISTS "${zqf_vcpkg_root_dir}/vcpkg")
    set(zqf_vcpkg_root_dir "${zqf_vcpkg_root_dir}" PARENT_SCOPE)
    return()
  endif()

  set(zqf_vcpkg_tool_release_url "https://github.com/microsoft/vcpkg-tool/releases/download/2025-12-05")

  if(WIN32)
    file(WRITE "${zqf_vcpkg_root_dir}/.vcpkg-root")
    file(DOWNLOAD "${zqf_vcpkg_tool_release_url}/vcpkg-init.cmd" "${zqf_vcpkg_root_dir}/vcpkg-init.cmd")
    execute_process(COMMAND cmd.exe /c "${zqf_vcpkg_root_dir}/vcpkg-init.cmd")
  else()
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
      file(DOWNLOAD "${zqf_vcpkg_tool_release_url}/vcpkg-glibc-arm64" "${zqf_vcpkg_root_dir}/vcpkg")
      execute_process(COMMAND chmod +x "${zqf_vcpkg_root_dir}/vcpkg")
      execute_process(COMMAND bash -c "export VCPKG_ROOT=\"${zqf_vcpkg_root_dir}\" && \"${zqf_vcpkg_root_dir}/vcpkg\" bootstrap-standalone")
    else()
      file(DOWNLOAD "${zqf_vcpkg_tool_release_url}/vcpkg-init" "${zqf_vcpkg_root_dir}/vcpkg-init")
      execute_process(COMMAND chmod +x "${zqf_vcpkg_root_dir}/vcpkg-init")
      execute_process(COMMAND bash -c "VCPKG_ROOT=\"${zqf_vcpkg_root_dir}\" && . \"${zqf_vcpkg_root_dir}/vcpkg-init\"")
    endif()
  endif()

  set(zqf_vcpkg_root_dir "${zqf_vcpkg_root_dir}" PARENT_SCOPE)
endfunction()

if(Z_VCPKG_ROOT_DIR)
  set(zqf_vcpkg_root_dir "${Z_VCPKG_ROOT_DIR}")
else()
  zqf_vcpkg_fetch()
  set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_LIST_DIR}")
endif()

include("${zqf_vcpkg_root_dir}/scripts/buildsystems/vcpkg.cmake")
