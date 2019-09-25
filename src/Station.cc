#include "Station.hh"
#include <chrono>

static const uint16_t CONTENTION_WINDOW_MAX { 1024 };
static const uint16_t CONTENTION_WINDOW_0 { 4 };

static const std::chrono::microseconds SIFS { 10 };
static const std::chrono::microseconds DIFS { 40 };

static const size_t ACK_BYTES { 30 };
static const size_t RTS_BYTES { 30 };
static const size_t CTS_BYTES { 30 };
