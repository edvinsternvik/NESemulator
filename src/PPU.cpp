#include "PPU.h"
#include "Buss.h"

void PPU::registerBuss(Buss* buss) {
    m_buss = buss;
}