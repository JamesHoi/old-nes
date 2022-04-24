#define PRG_BANK_MENU_INPUT_TEXT 3
#define PRG_BANK_MENU_DRAW_TEXT 4   
#define PRG_BANK_MENU_CHECK_FLAG 5

#define FLAG_POS_X 4
#define FLAG_POS_Y 15
#define MAX_FLAG_LENGHT 16  // globals flagMRCTF length should be MAX_FLAG_LENGHT+1
#define SUCCESS_POS_X 4
#define SUCCESS_POX_Y 16

//#define MRCTF_DEBUG

ZEROPAGE_ARRAY_EXTERN(unsigned char, flagMRCTF, 17);
ZEROPAGE_EXTERN(unsigned char, flagIndex);
ZEROPAGE_ARRAY_EXTERN(unsigned char, textMRCTF, 8);
ZEROPAGE_EXTERN(unsigned char, killcnt);
ZEROPAGE_EXTERN(unsigned char, found_key);
ZEROPAGE_EXTERN(unsigned char, cipher);
ZEROPAGE_EXTERN(unsigned char, correct);

void wait_for_text(void);
void draw_mrctf_text(void);
void check_flag(void);
void mrctf_init(void);