#ifndef BOSHY_RUNTIME_PATHS_HPP
#define BOSHY_RUNTIME_PATHS_HPP

#include <filesystem>

// PTSD resolves config and shader paths relative to the process directory.
// Select the deployed executable directory before constructing Core::Context.
void UseRuntimeDirectory(const std::filesystem::path& directory);

#endif
