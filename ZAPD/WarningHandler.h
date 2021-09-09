#pragma once

#include <array>
#include <string>
#include <string_view>

#include "ZFile.h"
#include "Utils/vt.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// TODO: better names
#define HANDLE_ERROR(header, body) WarningHandler::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, header, body)
#define HANDLE_WARNING(warningType, header, body) WarningHandler::Warning(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header, body)
#define HANDLE_WARNING_RESOURCE(warningType, parent, offset, header, body) WarningHandler::Warning_Resource(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, parent, offset, header, body)

enum class WarningType {
    Everything,
    Always, // Warnings of this type is always printed, can't be disabled.
    Deprecated,
    Unaccounted,
    MissingOffsets,
    Intersection,

    Max,
};

class WarningHandler {
public:
    static std::array<bool, static_cast<size_t>(WarningType::Max)> enabledWarnings;

    static void Init(int argc, char* argv[]);

    [[ noreturn ]]
    static void Error(const char* filename, int32_t line, const char* function, const std::string& header, const std::string& body);
    //[[ noreturn ]]
    //static void Error_Resource(const std::string& filename, int32_t line, const char* function, const std::string& header, const std::string& body);

    // variadic?
    static void Warning(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body);
    static void Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, uint32_t offset, const std::string& header, const std::string& body);

protected:
    static bool Werror;
};
