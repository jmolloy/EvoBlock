#include "EvoBlock.h"

Cell::Cell() {
}

Cell::~Cell() {
}

void Cell::Calc() {
    unsigned char x = 
        m_lut[ (m_links[Link_A]->GetOutput()) |
               (m_links[Link_B]->GetOutput() << 1) |
               (m_links[Link_C]->GetOutput() << 2) ];
    m_output = (x == 0) ? 0 : 1;
}

EvoBlock::EvoBlock() {
}

EvoBlock::~EvoBlock() {
}

void EvoBlock::Program(int col, int row, unsigned int x) {
    int link_a = (x >> LINK_A_S) & LINK_MASK;
    int link_b = (x >> LINK_B_S) & LINK_MASK;
    int link_c = (x >> LINK_C_S) & LINK_MASK;
    int lut    = (x >> LUT_S)    & LUT_MASK;

    Cell *cell_a, *cell_b, *cell_c;
    if(link_a >= 8) {
        if(col == 0) {
            cell_a = &m_array[0][link_a-8];
        } else {
            cell_a = &m_array[col-1][link_a-8];
        }
    }
    if(link_b >= 8) {
        if(col == 0) {
            cell_b = &m_array[0][link_b-8];
        } else {
            cell_b = &m_array[col-1][link_b-8];
        }
    }
    if(link_c >= 8) {
        if(col == 0) {
            cell_c = &m_array[0][link_c-8];
        } else {
            cell_c = &m_array[col-1][link_c-8];
        }
    }

    m_array[col][row].SetLink(Link_A, cell_a);
    m_array[col][row].SetLink(Link_B, cell_b);
    m_array[col][row].SetLink(Link_C, cell_c);
    m_array[col][row].SetLUT(lut);
}

unsigned char EvoBlock::Calc(unsigned int in) {
    // Calc column-at-a-time.
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 8; j++) {
            m_array[i][j].Calc();
        }
    }

    unsigned char result = (m_array[4][0].GetOutput() << 0) |
        (m_array[4][1].GetOutput() << 1) |
        (m_array[4][2].GetOutput() << 2) |
        (m_array[4][3].GetOutput() << 3) |
        (m_array[4][4].GetOutput() << 4) |
        (m_array[4][5].GetOutput() << 5) |
        (m_array[4][6].GetOutput() << 6) |
        (m_array[4][7].GetOutput() << 7);

    unsigned char match = ~ (result ^ m_truth_table[in]);

    return match & m_mask;
}
