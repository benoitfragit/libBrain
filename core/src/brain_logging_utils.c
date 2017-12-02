#include "brain_logging_utils.h"
#include "brain_core_types.h"
#include <string.h>

#define MESSAGE() va_list args;                                        \
                  va_start(args, format);                              \
                  char buffer[1000];                                   \
                  vsprintf(buffer, format, args);                      \
                  printf("%s\n", buffer);                              \
                  va_end(args);

typedef enum BrainLogLevel
{
    LOG_DEBUG    = 0,
    LOG_INFO     = 1,
    LOG_WARNING  = 2,
    LOG_CRITICAL = 3,
    LOG_NONE     = 4
} BrainLogLevel;

static BrainString _log_levels[] = {
    "debug",
    "info",
    "warning",
    "critical"
};

static BrainLogLevel
getBrainLogLevel()
{
    BrainLogLevel level = LOG_NONE;
    BrainString log_level = getenv("BRAIN_LOG_LEVEL");

    if (log_level)
    {
        BrainUint i = 0;

        for (i = 0; i <= 3; ++i)
        {
            if (!strcmp(_log_levels[i], log_level))
            {
                level = (BrainLogLevel)(i);
                break;
            }
        }
    }

    return level;
}

void
BRAIN_DEBUG(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    const BrainLogLevel level = getBrainLogLevel();

    if (level <= LOG_DEBUG)
    {
        MESSAGE()
    }
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_INFO(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    const BrainLogLevel level = getBrainLogLevel();

    if (level <= LOG_INFO)
    {
        MESSAGE()
    }
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_WARNING(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    const BrainLogLevel level = getBrainLogLevel();

    if (level <= LOG_WARNING)
    {
        MESSAGE()
    }
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_CRITICAL(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    const BrainLogLevel level = getBrainLogLevel();

    if (level <= LOG_CRITICAL)
    {
        MESSAGE()
    }
#endif /* BRAIN_ENABLE_LOGGING */
}
