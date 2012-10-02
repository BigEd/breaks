//#define BREAKS6502_PACKED_BITS

typedef struct Context6502
{
    int         NoDecimalCorrection;        // disable BCD ALU correction if set

    // Inputs
    int         PHI0;
    int         NMI, IRQ, RDY, RES, SO;

    // Outputs
    int         RW, SYNC;
    char        PHI1, PHI2;         // Timing

#ifdef  BREAKS6502_PACKED_BITS

#else

    // Buses
    char        DATA[8], ADDR[16];

    // Top part internal state
    char        PD[8];              // Predecode register
    char        IR[8];              // Instruction register
    char        TR[6];              // T-state register
    char        PLAOUT[129];        // PLA outputs
    char        DRIVEREG[64];
    char        SOLatch[3];         // SO line latches
    char        FromSO;
    char        RESLatch;           // Reset latch
    char        BRLatch[2];         // Branch ready latches
    char        IRQP;               // IRQ input logic latch
    char        FromNMI;            // NMI logic output
    char        TRSync;             // Timereg latches
    char        TRin[4], TRout[4];
    int         Tcount;             // Packed T2-T5 counter
    // Random logic latches and internal variables
    int         sync, ready, TRES;
    int         clearIR, fetch;
    char        RWOut;              // R/W output latch

    // Bottom part internal state
    char        SB[8], DB[8];           // SBus, DBus
    char        X[8], Y[8], S[8];       // X, Y, S registers
    char        ABH[8], ABL[8];         // Address bus registers
    char        ADH[8], ADL[8];         // Internal address bus
    char        DL[8], DOR[8];          // Data latch, data output register
    char        AI[8], BI[8], ADD[8], AC[8];    // ALU regs
    // ALU latches
    char        PCL[8], PCLS[8], PCH[8], PCHS[8];   // Program Counter    

#endif  // BREAKS6502_PACKED_BITS

} Context6502;

void    Step6502 ( Context6502 * cpu );
void    Debug6502 ( Context6502 * cpu, char * cmd );
