#include "VGAREGS.H"


void unProtectCRTC(void) {
    int v;
    outPortb(CRTC_ADDR, 0x11);           // Index = 11h
    v = inPortb(CRTC_ADDR + 1);          // Read from 3d5h
    v = v & 0x7F;                        // Set bit 7 to 0
    outPortb(CRTC_ADDR, 0x11);           // Index = 11h again
    outPortw(CRTC_ADDR, 0x11 | (v << 8)); // Write to unprotect
}

// Returns the value contained in the specified register.
unsigned char registerIn(int controller, char index) {
    unsigned char value;

    switch (controller) {
        case MISC_ADDR:
            value = inPortb(0x3cc);
            break;

        case ATTR_ADDR:
            inPortb(STATUS_ADDR);         // Reset flip-flop
            outPortb(ATTR_ADDR, index);
            value = inPortb(ATTR_ADDR + 1);
            break;

        case SEQU_ADDR:
        case GRAC_ADDR:
        case CRTC_ADDR:
        case CHIPSTECH_ADDR:
            outPortb(controller, index); // Set index
            value = inPortb(controller + 1);
            break;

        case VGAENABLE_ADDR:
        default:
            value = inPortb(controller); // Default case
            break;
    }

    outPortb(ATTR_ADDR, 0x20); // Allow screen writes
    return value;              // Return the value
}

// Writes the value to the specified register.
void registerOut(int controller, char index, unsigned char value) {
    switch (controller) {
        case ATTR_ADDR:
            inPortb(STATUS_ADDR);               // Reset flip-flop
            outPortb(ATTR_ADDR, index);
            outPortb(ATTR_ADDR, value);         // Write value
            break;

        case SEQU_ADDR:
            if (index == 1) {
                outPortw(SEQU_ADDR, 0x0100);            // Reset sequencer
                outPortw(SEQU_ADDR, (value << 8) | 1);  // Write value
                outPortw(SEQU_ADDR, 0x0300);            // Restore sequencer
                break;
            }
        case GRAC_ADDR:
        case CRTC_ADDR:
        case CHIPSTECH_ADDR:
            outPortw(controller, (index | (value << 8)));
            break;

        case MISC_ADDR:
        case VGAENABLE_ADDR:
        default:
            outPortb(controller, value);  // Write to port
            break;
    }

    outPortb(ATTR_ADDR, 0x20);  // Allow screen writes
}

