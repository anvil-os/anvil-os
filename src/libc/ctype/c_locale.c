
#include <ctype.h>

/* Some local defines to make the table more readable */
#define CTL _Anvil_CtCtl
#define SPC _Anvil_CtSpc
#define BLN _Anvil_CtBln
#define PNC _Anvil_CtPnc
#define NUM _Anvil_CtNum
#define UPP _Anvil_CtUpp
#define LOW _Anvil_CtLow
#define HEX _Anvil_CtHex

const short _C_Ctype[257] =
{
    0,
    /* 0x00 to 0x07 are control chars */
    CTL, CTL, CTL, CTL, CTL, CTL, CTL, CTL,
    /* 0x09 to 0x0d are spaces (TAB, LF, VT, FF, CR). All are control chars */
    CTL, CTL|SPC|BLN, CTL|SPC, CTL|SPC, CTL|SPC, CTL|SPC, CTL, CTL,
    /* 0x10 to 0x17 are control chars */
    CTL, CTL, CTL, CTL, CTL, CTL, CTL, CTL,
    /* 0x18 to 0x1f are control chars */
    CTL, CTL, CTL, CTL, CTL, CTL, CTL, CTL,
    /* 0x20 is space, other punct. chars - !"#$%&' */
    SPC|BLN, PNC, PNC, PNC, PNC, PNC, PNC, PNC,
    /* ()*+,-./ */
    PNC, PNC, PNC, PNC, PNC, PNC, PNC, PNC,
    /* 01234567 */
    NUM, NUM, NUM, NUM, NUM, NUM, NUM, NUM,
    /* 89:;<=>? */
    NUM, NUM, PNC, PNC, PNC, PNC, PNC, PNC,
    /* @ABCDEFG */
    PNC, UPP|HEX, UPP|HEX, UPP|HEX, UPP|HEX, UPP|HEX, UPP|HEX, UPP,
    /* HIJKLMNO */
    UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP,
    /* PQRSTUVW */
    UPP, UPP, UPP, UPP, UPP, UPP, UPP, UPP,
    /* XYZ[\]^_ */
    UPP, UPP, UPP, PNC, PNC, PNC, PNC, PNC,
    /* `abcdefg */
    PNC, LOW|HEX, LOW|HEX, LOW|HEX, LOW|HEX, LOW|HEX, LOW|HEX, LOW,
    /* hijklmno */
    LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW,
    /* pqrstuvw */
    LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW,
    /* xyz{|}~ and DEL */
    LOW, LOW, LOW, PNC, PNC, PNC, PNC, CTL

    /* We just leave the rest (128 to 255) as zeroes for now
     * entry[256] is for EOF
     */

    /* EOF */
    // 0,
};

const unsigned char _C_toupper[257] =
{
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const unsigned char _C_tolower[257] =
{
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
