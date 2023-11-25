#include "global.h"
#include "util.h"
#include "sprite.h"
#include "palette.h"
#include "constants/rgb.h"
#include "pokemon.h"

const u32 gBitTable[] =
{
    1 << 0,
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7,
    1 << 8,
    1 << 9,
    1 << 10,
    1 << 11,
    1 << 12,
    1 << 13,
    1 << 14,
    1 << 15,
    1 << 16,
    1 << 17,
    1 << 18,
    1 << 19,
    1 << 20,
    1 << 21,
    1 << 22,
    1 << 23,
    1 << 24,
    1 << 25,
    1 << 26,
    1 << 27,
    1 << 28,
    1 << 29,
    1 << 30,
    1 << 31,
};

static const struct SpriteTemplate sInvisibleSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const u8 sSpriteDimensions[3][4][2] =
{
    // square
    {
        {1, 1},
        {2, 2},
        {4, 4},
        {8, 8},
    },

    // horizontal rectangle
    {
        {2, 1},
        {4, 1},
        {4, 2},
        {8, 4},
    },

    // vertical rectangle
    {
        {1, 2},
        {1, 4},
        {2, 4},
        {4, 8},
    },
};

static const u16 sCrc16Table[] =
{
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78,
};

const u8 gMiscBlank_Gfx[] = INCBIN_U8("graphics/interface/blank.4bpp");

#define HUE_SHIFT_RANGE_NORMAL  40
#define HUE_SHITFT_RANGE_SHINY  30

#define NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE     -1
#define NORMAL_HUE_BOTH_WAYS_SHINY_HUE_BOTH_WAYS    0
#define NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE      1
#define NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS   2
#define NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION   3
#define NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION  4

#define SOURCE_PID          0
#define SOURCE_IVS          1
#define SOURCE_NICKNAME_OT  2

#define INDIVIDUALITY_SOURCE SOURCE_PID

static const s8 sColorVariationModes[NUM_SPECIES] =
{
    [SPECIES_CHARIZARD]     = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_PIKACHU]       = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_RAICHU]        = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_CLEFAIRY]      = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_CLEFABLE]      = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_VULPIX]        = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_NINETALES]     = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_JIGGLYPUFF]    = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_WIGGLYTUFF]    = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_PARAS]         = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_PARASECT]      = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_MEOWTH]        = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_PERSIAN]       = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_PSYDUCK]       = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_GOLDUCK]       = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_GROWLITHE]     = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_ARCANINE]      = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_POLIWAG]       = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_POLIWHIRL]     = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_ABRA]          = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_KADABRA]       = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_MACHOP]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_MACHOKE]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_MACHAMP]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_MAGNEMITE]     = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_MAGNETON]      = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_SEEL]          = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_DEWGONG]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_GRIMER]        = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_MUK]           = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SHELLDER]      = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_CLOYSTER]      = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_GASTLY]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_HAUNTER]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_GENGAR]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_ONIX]          = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_RHYHORN]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_RHYDON]        = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SCYTHER]       = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_ELECTABUZZ]    = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_MAGIKARP]      = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_LAPRAS]        = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_DITTO]         = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_EEVEE]         = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_FLAREON]       = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_AERODACTYL]    = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SNORLAX]       = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_ZAPDOS]        = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_MEWTWO]        = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_HOOTHOOT]      = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_NOCTOWL]       = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_LEDYBA]        = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_LEDIAN]        = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_PICHU]         = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_TOGEPI]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_TOGETIC]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_SUNKERN]       = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_UMBREON]       = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_MURKROW]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_MISDREAVUS]    = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_UNOWN]         = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_STEELIX]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_SHUCKLE]       = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_SNEASEL]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SWINUB]        = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_PILOSWINE]     = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_MANTINE]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_PHANPY]        = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_DONPHAN]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SMEARGLE]      = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_ELEKID]        = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_MILTANK]       = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_LARVITAR]      = NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE,
    [SPECIES_PUPITAR]       = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_LUGIA]         = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_POOCHYENA]     = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_MIGHTYENA]     = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_ZIGZAGOON]     = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_LINOONE]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_SABLEYE]       = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_ARON]          = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_LAIRON]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_AGGRON]        = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_SPOINK]        = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_GRUMPIG]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_TRAPINCH]      = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_CRAWDAUNT]     = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_FEEBAS]        = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION,
    [SPECIES_CASTFORM]      = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_SHUPPET]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_BANETTE]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_DUSKULL]       = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_DUSCLOPS]      = NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS,
    [SPECIES_ABSOL]         = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_REGISTEEL]     = NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION,
    [SPECIES_LATIAS]        = NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE,
    [SPECIES_RAYQUAZA]      = NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION
};

u8 CreateInvisibleSpriteWithCallback(void (*callback)(struct Sprite *))
{
    u8 sprite = CreateSprite(&sInvisibleSpriteTemplate, DISPLAY_WIDTH + 8, DISPLAY_HEIGHT + 8, 14);
    gSprites[sprite].invisible = TRUE;
    gSprites[sprite].callback = callback;
    return sprite;
}

void StoreWordInTwoHalfwords(u16 *h, u32 w)
{
    h[0] = (u16)(w);
    h[1] = (u16)(w >> 16);
}

void LoadWordFromTwoHalfwords(u16 *h, u32 *w)
{
    *w = h[0] | (s16)h[1] << 16;
}

void SetBgAffineStruct(struct BgAffineSrcData *src, u32 texX, u32 texY, s16 scrX, s16 scrY, s16 sx, s16 sy, u16 alpha)
{
    src->texX = texX;
    src->texY = texY;
    src->scrX = scrX;
    src->scrY = scrY;
    src->sx = sx;
    src->sy = sy;
    src->alpha = alpha;
}

void DoBgAffineSet(struct BgAffineDstData *dest, u32 texX, u32 texY, s16 scrX, s16 scrY, s16 sx, s16 sy, u16 alpha)
{
    struct BgAffineSrcData src;

    SetBgAffineStruct(&src, texX, texY, scrX, scrY, sx, sy, alpha);
    BgAffineSet(&src, dest, 1);
}

void CopySpriteTiles(u8 shape, u8 size, u8 *tiles, u16 *tilemap, u8 *output)
{
    u8 x, y;
    s8 i, j;
    u8 ALIGNED(4) xflip[32];
    u8 h = sSpriteDimensions[shape][size][1];
    u8 w = sSpriteDimensions[shape][size][0];

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            int tile = (*tilemap & 0x3ff) * 32;

            if ((*tilemap & 0xc00) == 0)
            {
                CpuCopy32(tiles + tile, output, 32);
            }
            else if ((*tilemap & 0xc00) == 0x800)  // yflip
            {
                for (i = 0; i < 8; i++)
                    CpuCopy32(tiles + (tile + (7 - i) * 4), output + i * 4, 4);
            }
            else  // xflip
            {
                for (i = 0; i < 8; i++)
                {
                    for (j = 0; j < 4; j++)
                    {
                        u8 i2 = i * 4;
                        xflip[i2 + (3-j)] = (tiles[tile + i2 + j] & 0xf) << 4;
                        xflip[i2 + (3-j)] |= tiles[tile + i2 + j] >> 4;
                    }
                }
                if (*tilemap & 0x800)  // yflip
                {
                    for (i = 0; i < 8; i++)
                        CpuCopy32(xflip + (7 - i) * 4, output + i * 4, 4);
                }
                else
                {
                    CpuCopy32(xflip, output, 32);
                }
            }
            tilemap++;
            output += 32;
        }
        tilemap += (32 - w);
    }
}

int CountTrailingZeroBits(u32 value)
{
    u8 i;

    for (i = 0; i < 32; i++)
    {
        if ((value & 1) == 0)
            value >>= 1;
        else
            return i;
    }
    return 0;
}

u16 CalcCRC16(const u8 *data, s32 length)
{
    u16 i, j;
    u16 crc = 0x1121;

    for (i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}

u16 CalcCRC16WithTable(const u8 *data, u32 length)
{
    u16 i;
    u16 crc = 0x1121;
    u8 byte;

    for (i = 0; i < length; i++)
    {
        byte = crc >> 8;
        crc ^= data[i];
        crc = byte ^ sCrc16Table[(u8)crc];
    }
    return ~crc;
}

u32 CalcByteArraySum(const u8 *data, u32 length)
{
    u32 sum, i;
    for (sum = 0, i = 0; i < length; i++)
        sum += data[i];
    return sum;
}

void BlendPalette(u16 palOffset, u16 numEntries, u8 coeff, u16 blendColor)
{
    u16 i;
    for (i = 0; i < numEntries; i++)
    {
        u16 index = i + palOffset;
        struct PlttData *data1 = (struct PlttData *)&gPlttBufferUnfaded[index];
        s8 r = data1->r;
        s8 g = data1->g;
        s8 b = data1->b;
        struct PlttData *data2 = (struct PlttData *)&blendColor;
        gPlttBufferFaded[index] = RGB(r + (((data2->r - r) * coeff) >> 4),
                                      g + (((data2->g - g) * coeff) >> 4),
                                      b + (((data2->b - b) * coeff) >> 4));
    }
}

void UniquePalette(u16 palOffset, struct BoxPokemon *boxMon)
{
    u32 i;
    u32 value;
    s32 shift;
    u8 otId[4];
    u8 otName[PLAYER_NAME_LENGTH + 1];
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    s32 variationMode = sColorVariationModes[GetBoxMonData(boxMon, MON_DATA_SPECIES)];
    bool32 isShiny = IsShinyOtIdPersonality(GetBoxMonData(boxMon, MON_DATA_OT_ID), GetBoxMonData(boxMon, MON_DATA_PERSONALITY));
    bool32 willHueShift = TRUE;

    #if INDIVIDUALITY_SOURCE == SOURCE_PID
    value = (GetBoxMonData(boxMon, MON_DATA_PERSONALITY) >> 8) & 0xFFFF;
    #elif INDIVIDUALITY_SOURCE == SOURCE_IVS
    value = GetBoxMonData(boxMon, MON_DATA_IVS);
    #elif INDIVIDUALITY_SOURCE == SOURCE_NICKNAME_OT
    (u32)*otId = GetBoxMonData(boxMon, MON_DATA_OT_ID);
    GetBoxMonData(boxMon, MON_DATA_OT_NAME, otName);
    GetBoxMonData(boxMon, MON_DATA_NICKNAME, nickname);
    value = 0;

    for (i = 0; i < 4; i++)
        value += otId[i];

    for (i = 0; i < PLAYER_NAME_LENGTH + 1; i++)
    {
        if (otName[i] == 0xFF)
            break;
        value += otName[i];
    }

    for (i = 0; i < POKEMON_NAME_LENGTH + 1; i++)
    {
        if (nickname[i] == 0xFF)
            break;
        value += nickname[i];
    }
    #endif

    if (isShiny)
    {
        switch (variationMode)
        {
            case NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE:
                shift = value % (HUE_SHITFT_RANGE_SHINY + 1);
                break;
            case NORMAL_HUE_BOTH_WAYS_SHINY_HUE_BOTH_WAYS:
            case NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS:
            default:
                shift = (value % (HUE_SHITFT_RANGE_SHINY * 2 + 1)) - HUE_SHITFT_RANGE_SHINY;
                break;
            case NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE:
                shift = (value % (HUE_SHITFT_RANGE_SHINY + 1)) - HUE_SHITFT_RANGE_SHINY;
                break;
            case NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION:
            case NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION:
                willHueShift = FALSE;
                break;
        }
    }
    else
    {
        switch (variationMode)
        {
            case NORMAL_HUE_NEGATIVE_SHINY_HUE_POSITIVE:
                shift = (value % (HUE_SHIFT_RANGE_NORMAL + 1)) - HUE_SHIFT_RANGE_NORMAL;
                break;
            case NORMAL_HUE_BOTH_WAYS_SHINY_HUE_BOTH_WAYS:
            case NORMAL_HUE_BOTH_WAYS_SHINY_RGB_MODULATION:
            default:
                shift = (value % (HUE_SHIFT_RANGE_NORMAL * 2 + 1)) - HUE_SHIFT_RANGE_NORMAL;
                break;
            case NORMAL_HUE_POSITIVE_SHINY_HUE_NEGATIVE:
                shift = value % (HUE_SHIFT_RANGE_NORMAL + 1);
                break;
            case NORMAL_RGB_MODULATION_SHINY_HUE_BOTH_WAYS:
            case NORMAL_RGB_MODULATION_SHINY_RGB_MODULATION:
                willHueShift = FALSE;
                break;
        }
    }

    if (willHueShift == FALSE)
    {
        s8 dr = ((value >> 8) & 0xF) % 5;
        s8 dg = ((value >> 4) & 0xF) % 5;
        s8 db = (value & 0xF) % 5;

        for (i = 0; i < 16; i++)
        {
            u32 index = i + palOffset;
            struct PlttData *data1 = (struct PlttData *)&gPlttBufferUnfaded[index];
            s8 r = data1->r + dr - 2;
            s8 g = data1->g + dg - 2;
            s8 b = data1->b + db - 2;

            if (r > 31)
                r = 31 - dr / 2;
            if (g > 31)
                g = 31 - dg / 2;
            if (b > 31)
                b = 31 - db / 2;
            if (r < 0)
                r = dr / 2;
            if (g < 0)
                g = dg / 2;
            if (b < 0)
                b = db / 2;

            gPlttBufferFaded[index] = RGB(r, g, b);
        }
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            u32 index = i + palOffset;
            struct PlttData *data1 = (struct PlttData *)&gPlttBufferUnfaded[index];
            s32 r = (data1->r * 1000) / 31;
            s32 g = (data1->g * 1000) / 31;
            s32 b = (data1->b * 1000) / 31;
            s32 maxv, minv, d, h, s, l, o, p, q;

            if (r > g)
                maxv = r;
            else
                maxv = g;
            if (b > maxv)
                maxv = b;
            if (r < g)
                minv = r;
            else
                minv = g;
            if (b < minv)
                minv = b;

            d = maxv - minv;
            h = 0;
            s = 0;
            l = (maxv + minv) / 2;

            if  (maxv != minv)
            {
                if (l > 500)
                    s = 1000 * d / (2000 - maxv - minv);
                else
                    s = 1000 * d / (maxv + minv);
                if (maxv == r)
                {
                    if (g < b)
                        h = 1000 * (g - b) / d + 6000;
                    else
                        h = 1000 * (g - b) / d;
                }
                else if (maxv == g)
                {
                    h = 1000 * (b - r) / d + 2000;
                }
                else
                {
                    h = 1000 * (r - g) / d + 4000;
                }
                h /= 6;
            }

            h = (h + shift + 1000) % 1000;

            if (s != 0)
            {
                o = (h + 333) % 1000;

                if (l < 500)
                    p = l * (s + 1000) / 1000;
                else
                    p = l + s - l * s / 1000;

                q = l * 2 - p;

                if (o < 167)
                    r = q + (p - q) * o * 6 / 1000;
                else if (o < 500)
                    r = p;
                else if (o < 667)
                    r = q + (p - q) * (667 - o) * 6 / 1000;
                else
                    r = q;

                o = h;

                if (o < 167)
                    g = q + (p - q) * o * 6 / 1000;
                else if (o < 500)
                    g = p;
                else if (o < 667)
                    g = q + (p - q) * (667 - o) * 6 / 1000;
                else
                    g = q;

                o = (h + 1000 - 333) % 1000;

                if (o < 167)
                    b = q + (p - q) * o * 6 / 1000;
                else if (o < 500)
                    b = p;
                else if (o < 667)
                    b = q + (p - q) * (667 - o) * 6 / 1000;
                else
                    b = q;
            }
            else
            {
                r = l;
                g = l;
                b = l;
            }

            gPlttBufferFaded[index] = RGB((u8)(r * 31 / 1000), (u8)(g * 31 / 1000), (u8)(b * 31 / 1000));
        }
    }
}
