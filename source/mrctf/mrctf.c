#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/menus/text_helpers.h"
#include "source/mrctf/mrctf.h"
#include <string.h>


ZEROPAGE_ARRAY_DEF(unsigned char, flagMRCTF, 17);
ZEROPAGE_DEF(unsigned char, flagIndex);
ZEROPAGE_ARRAY_DEF(unsigned char, textMRCTF, 8);
ZEROPAGE_DEF(unsigned char, killcnt);
ZEROPAGE_DEF(unsigned char, found_key);
ZEROPAGE_DEF(unsigned char, cipher);
ZEROPAGE_DEF(unsigned char, correct);

unsigned char enc_key[16];
unsigned char enc_out[16];
unsigned char enc_flag[16];
unsigned char sum;
unsigned char tmp;

CODE_BANK(PRG_BANK_MENU_INPUT_TEXT);
void wait_for_text(void){
    while(1){
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        if(killcnt!=44||!found_key)continue;
        
        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            if(flagMRCTF[flagIndex] == 126 || flagMRCTF[flagIndex] == 0)
                flagMRCTF[flagIndex] = 32;
            flagMRCTF[flagIndex] += 1;
            break;
        }
        else if (controllerState & PAD_B && !(lastControllerState & PAD_B)) {
            flagIndex += 1;
            break;
        }
    }
}

CODE_BANK(PRG_BANK_MENU_DRAW_TEXT);
void draw_mrctf_text(void){
    set_vram_update(NULL);
    i = 0;
    screenBuffer[i++] = MSB(NTADR_A(FLAG_POS_X+flagIndex,FLAG_POS_Y))|NT_UPD_HORZ; // where to write, repeat horizontally
    screenBuffer[i++] = LSB(NTADR_A(FLAG_POS_X+flagIndex,FLAG_POS_Y));
    screenBuffer[i++] = 1;
    screenBuffer[i++] = flagMRCTF[flagIndex]-0x20; //-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
    screenBuffer[i++] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    ppu_wait_nmi();
}

CODE_BANK(PRG_BANK_MENU_CHECK_FLAG);
void check_flag(void){
    #ifdef MRCTF_DEBUG
        strcpy(flagMRCTF,"fUn0LdX0r65O2aSM");
        flagIndex = MAX_FLAG_LENGHT;
        cipher = 0x6e;
    #endif
    if(flagIndex == MAX_FLAG_LENGHT){
        memcpy(enc_key,"\x6d\x72\x63\x74\x66\x5f\x66\x61\x6b\x65\x5f\x66\x6c\x61\x67\x21",16);
        memcpy(enc_flag,"\x5C\xEB\xE8\x9E\x99\xB3\x5F\x47\x16\xB7\xD7\xBB\x35\x20\x0D\x62",16);
        for(i=0;i<16;i++){
            sum += 0x29;
            enc_out[i]=(flagMRCTF[i]^enc_key[i]^sum)+0x10;
        }

        strcpy(textMRCTF,"success");
        for(i=0;i<16;i++){
            enc_out[i] ^= cipher;
            if(enc_out[i]!=enc_flag[i]){
                strcpy(textMRCTF,"fail   ");
                break;
            }
        }
        //flag: fUn0LdX0r65O2aSM

        set_vram_update(NULL);
        i = 0;
        screenBuffer[i++] = MSB(NTADR_A(SUCCESS_POS_X,SUCCESS_POX_Y))|NT_UPD_HORZ; // where to write, repeat horizontally
        screenBuffer[i++] = LSB(NTADR_A(SUCCESS_POS_X,SUCCESS_POX_Y));
        screenBuffer[i++] = 7;
        for(j=0;j<7;j++)screenBuffer[i++] = textMRCTF[j]-0x20;
        screenBuffer[i++] = NT_UPD_EOF;
        set_vram_update(screenBuffer);
        ppu_wait_nmi();
        delay(100);
    }
}