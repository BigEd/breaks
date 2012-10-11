#include "Breaks6502.h"
#include "Breaks6502Private.h"

// Program counter use optimized carry chain, which is inverted every next stage.
// Details here: http://forum.6502.org/viewtopic.php?f=8&t=2208&start=30#p20371

// Increment PC when IPC = 0

void ProgramCounter (Context6502 * cpu)
{
    int i, PCLC = 1, PCHC, carry_in, carry_out;

    // LOW

    carry_out = cpu->DRIVEREG[DRIVE_IPC];

    for (i=0; i<8; i++) {
        if (cpu->DRIVEREG[DRIVE_ADL_PCL]) cpu->PCLS[i] = cpu->ADL[i];

        carry_in = carry_out;
        if ( i & 1 ) {
            carry_out = NAND( cpu->PCLS[i], carry_in );
            if (cpu->PHI2)
                cpu->PCL[i] = NOR(cpu->PCLS[i], carry_in) | NOT(carry_out);
            if (cpu->DRIVEREG[DRIVE_PCL_ADL]) cpu->ADL[i] = cpu->PCL[i];
            if (cpu->DRIVEREG[DRIVE_PCL_DB]) cpu->DB[i] = cpu->PCL[i];
            if (cpu->DRIVEREG[DRIVE_PCL_PCL]) cpu->PCLS[i] = cpu->PCL[i];
        }
        else {
            carry_out = NOR( NOT(cpu->PCLS[i]), carry_in );
            if (cpu->PHI2)
                cpu->PCL[i] = NOR( NOT(cpu->PCLS[i]) & carry_in, carry_out );
            if (cpu->DRIVEREG[DRIVE_PCL_ADL]) cpu->ADL[i] = NOT(cpu->PCL[i]);
            if (cpu->DRIVEREG[DRIVE_PCL_DB]) cpu->DB[i] = NOT(cpu->PCL[i]);
            if (cpu->DRIVEREG[DRIVE_PCL_PCL]) cpu->PCLS[i] = NOT(cpu->PCL[i]);
        }
        if (cpu->PCLS[i] == 0) PCLC = 0;
    }

    printf ( "PCLC = %i\n", PCLC );

    // HIGH

    for (i=0; i<8; i++) {
        if (cpu->DRIVEREG[DRIVE_ADH_PCH]) cpu->PCHS[i] = cpu->ADH[i];
        if (cpu->DRIVEREG[DRIVE_PCH_ADH]) cpu->ADH[i] = cpu->PCH[i];
        if (cpu->DRIVEREG[DRIVE_PCH_DB]) cpu->DB[i] = cpu->PCH[i];
        if (cpu->DRIVEREG[DRIVE_PCH_PCH]) cpu->PCHS[i] = cpu->PCH[i];
    }
}