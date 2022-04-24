#include "source/menus/pause.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"
#include "source/library/itoa.h"
#include "source/mrctf/mrctf.h"

char buffer2[2];

CODE_BANK(PRG_BANK_PAUSE_MENU);
void draw_pause_screen(void) {
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    put_str(NTADR_A(11, 8), "- Paused -");

    put_str(NTADR_A(3, 12),"you have to kill 44 enemy");
    put_str(NTADR_A(3, 13),"now: you kill ");
    itoa(killcnt,buffer2);
    put_str(NTADR_A(17, 13),buffer2);

    put_str(NTADR_A(3, 15),"you have to find the key");
    if(found_key)put_str(NTADR_A(3, 16),"now: you found!");
    else put_str(NTADR_A(3, 16),"now: not found!");

    put_str(NTADR_A(3, 18),"Please touch the mystery");
    put_str(NTADR_A(3, 19),"box after finished the two");
    put_str(NTADR_A(3, 20),"things on top.");

    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_pause_input(void) {
    banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
    gameState = GAME_STATE_RUNNING;
}