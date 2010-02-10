#ifndef EVOBLOCK_H
#define EVOBLOCK_H

#define LINK_A_S  0x10
#define LINK_B_S  0x0C
#define LINK_C_S  0x08
#define LINK_MASK 0x0F

#define LUT_S     0x00
#define LUT_MASK  0xFF

enum Links {
    Link_A,
    Link_B,
    Link_C
};

class Cell {
public:
    Cell();
    ~Cell();

    void SetLink(Links l, Cell *x) {
        m_links[l] = x;
    }
    Cell *GetLink(Links l) {
        return m_links[l];
    }

    void SetLUT(unsigned char x) {
        m_lut[0] = x & 0x01;
        m_lut[1] = x & 0x02;
        m_lut[2] = x & 0x04;
        m_lut[3] = x & 0x08;
        m_lut[4] = x & 0x10;
        m_lut[5] = x & 0x20;
        m_lut[6] = x & 0x30;
        m_lut[7] = x & 0x40;
    }

    unsigned char GetOutput() {
        return m_output;
    }

    void Calc();

private:
    Cell *m_links[3];
    unsigned char m_lut[8];
    unsigned char m_output;
};

class EvoBlockSim {
public:
    EvoBlockSim();
    ~EvoBlockSim();

    void SetTruthTable(unsigned char in, unsigned char out) {
        m_truth_table[in] = out;
    }

    void Program(int col, int row, unsigned int x);

    void SetMask(unsigned char m) {
        m_mask = m;
    }

    unsigned char Calc(unsigned int in);

private:
    Cell m_array[5][8]; // Columns, rows.
    unsigned char m_mask;
    unsigned char m_truth_table[256];
};

#endif
