#ifndef SEMANTICVERSION_H
#define SEMANTICVERSION_H

#include <cstdint>
#include <string>

struct SemanticVersion
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;

    SemanticVersion()
    {
        major = 0;
        minor = 0;
        patch = 0;
    }

    bool IsValid() const
    {
        return (major != 0);
    }

    std::string ToString() const
    {
        return std::to_string(major) + "." +
             std::to_string(minor) + "." +
             std::to_string(patch);
    }

    bool operator < (const SemanticVersion& v) const
    {
        if (major < v.major)
        {
            return true;
        }
        else if (major == v.major)
        {
            if (minor < v.minor)
            {
                return true;
            }
            else if (minor == v.minor)
            {
                if (patch < v.patch)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    bool operator == (const SemanticVersion& v) const
    {
        return (major == v.major) && (minor == v.minor) && (patch == v.patch);
    }

    bool operator > (const SemanticVersion& v) const
    {
        if (*this < v)
        {
            return false;
        }
        else if (*this == v)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};

#endif // SEMANTICVERSION_H
