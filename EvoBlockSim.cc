#include "EvoBlockSim.h"

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

EvoBlockSim::EvoBlockSim() {
}

EvoBlockSim::~EvoBlockSim() {
}

void EvoBlockSim::Program(int col, int row, unsigned int x) {
    int link_a = (x >> LINK_A_S) & LINK_MASK;
    int link_b = (x >> LINK_B_S) & LINK_MASK;
    int link_c = (x >> LINK_C_S) & LINK_MASK;
    int lut    = (x >> LUT_S)    & LUT_MASK;

    Cell *cell_a = 0, *cell_b = 0, *cell_c = 0;
    if(link_a >= 8) {
        if(col == 0) {
            cell_a = &m_inputs_notted[link_a-8];
        } else if(col == 1) {
            cell_a = &m_inputs[link_a-8];
        } else {
            cell_a = &m_array[col-2][link_a-8];
        }
    } else {
        if(col == 0) {
            cell_a = &m_inputs[link_a];
        } else {
            cell_a = &m_array[col-1][link_a];
        }
    }

    if(link_b >= 8) {
        if(col == 0) {
            cell_b = &m_inputs_notted[link_b-8];
        } else if(col == 1) {
            cell_b = &m_inputs[link_b-8];
        } else {
            cell_b = &m_array[col-2][link_b-8];
        }
    } else {
        if(col == 0) {
            cell_b = &m_inputs[link_b];
        } else {
            cell_b = &m_array[col-1][link_b];
        }
    }

    if(link_c >= 8) {
        if(col == 0) {
            cell_c = &m_inputs_notted[link_c-8];
        } else if(col == 1) {
            cell_c = &m_inputs[link_c-8];
        } else {
            cell_c = &m_array[col-2][link_c-8];
        }
    } else {
        if(col == 0) {
            cell_c = &m_inputs[link_c];
        } else {
            cell_c = &m_array[col-1][link_c];
        }
    }

    m_array[col][row].SetLink(Link_A, cell_a);
    m_array[col][row].SetLink(Link_B, cell_b);
    m_array[col][row].SetLink(Link_C, cell_c);
    m_array[col][row].SetLUT(lut);
}

unsigned char EvoBlockSim::Calc(unsigned int in) {
    m_inputs[0]        .SetValue(  ((in >> 0) & 0x01));
    m_inputs_notted[0] .SetValue( !((in >> 0) & 0x01));
    m_inputs[1]        .SetValue(  ((in >> 1) & 0x01));
    m_inputs_notted[1] .SetValue( !((in >> 1) & 0x01));
    m_inputs[2]        .SetValue(  ((in >> 2) & 0x01));
    m_inputs_notted[2] .SetValue( !((in >> 2) & 0x01));
    m_inputs[3]        .SetValue(  ((in >> 3) & 0x01));
    m_inputs_notted[3] .SetValue( !((in >> 3) & 0x01));
    m_inputs[4]        .SetValue(  ((in >> 4) & 0x01));
    m_inputs_notted[4] .SetValue( !((in >> 4) & 0x01));
    m_inputs[5]        .SetValue(  ((in >> 5) & 0x01));
    m_inputs_notted[5] .SetValue( !((in >> 5) & 0x01));
    m_inputs[6]        .SetValue(  ((in >> 6) & 0x01));
    m_inputs_notted[6] .SetValue( !((in >> 6) & 0x01));
    m_inputs[7]        .SetValue(  ((in >> 7) & 0x01));
    m_inputs_notted[7] .SetValue( !((in >> 7) & 0x01));


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
