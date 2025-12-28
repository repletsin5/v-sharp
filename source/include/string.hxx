#pragma once

#include <string_view>


[[nodiscard]]
inline constexpr std::string_view toString_Access(AccessType access) {
    switch (access)
    {
    case AccessType::Default:
		return "Default";
        break;
    case AccessType::Public:
		return "Public";
        break;
    case AccessType::Private:
		return "Private";
        break;
    default:
        return "Unknown";
        break;
    }
}

[[nodiscard]]
inline constexpr std::string_view toString_Modifier(ModifierType mod) {
    switch (mod)
    {
    case ModifierType::None:
		return "None";
        break;
    case ModifierType::Static:
		return "Static";
        break;
    case ModifierType::Virtual:
		return "Virtual";
        break;
    case ModifierType::Override:
		return "Override";
        break;
    default:
        return "Unknown";
        break;
    }
}

[[nodiscard]]
inline constexpr std::string_view typeToString(Type t) noexcept
{
    switch (t)
    {
    case Type::Void:
        return "void";
    case Type::Boolean:
        return "boolean";
    case Type::Byte:
        return "byte";
    case Type::String:
        return "string";

    case Type::Int8:
        return "int8";
    case Type::Int16:
        return "int16";
    case Type::Int32:
        return "int32";
    case Type::Int64:
        return "int64";

    case Type::Uint8:
        return "uint8";
    case Type::Uint16:
        return "uint16";
    case Type::Uint32:
        return "uint32";
    case Type::Uint64:
        return "uint64";

    case Type::Float32:
        return "float32";
    case Type::Float64:
        return "float64";
    }

    return "unknown";
}

[[nodiscard]]
inline std::string_view keywordToString(const Keyword &kw) noexcept
{
    return kw.name;
}

inline std::ostream &operator<<(std::ostream &os, Type t)
{
    return os << typeToString(t);
}

inline std::ostream &operator<<(std::ostream &os, const Keyword &kw)
{
    return os << kw.name;
}
inline std::ostream& operator<<(std::ostream& os, const AccessType& ac)
{
    return os << toString_Access(ac);
}
inline std::ostream& operator<<(std::ostream& os, const ModifierType& mod)
{
    return os << toString_Modifier(mod);
}