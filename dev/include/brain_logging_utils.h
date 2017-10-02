#ifndef BRAIN_LOGGING_UTILS_H
#define BRAIN_LOGGING_UTILS_H

#include "brain_builder_types.h"

void BRAIN_CRITICAL(BrainString format, ...);
void BRAIN_DEBUG   (BrainString format, ...);
void BRAIN_INFO    (BrainString format, ...);

#endif /* BRAIN_LOGGING_UTILS_H */