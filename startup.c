
#include "memory-map.h"
#include "drivers/dbgu.h"
#include "printf.h"

// needed to prevent gcc from optimizing c_entry out.
#pragma GCC push_options
#pragma GCC optimize ("O0")

/* crucial use of memory*/
void logo() {
printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOdooodOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMMMMMMMMNOo;'     ';lONMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMMMMMMMKc             cKMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMWWMMMMK:               :KMMMMWWMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMWKdc::lxXMO'               'OMXxl::cdKWMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMO, ,ll' cX0,               ,0Xl 'll, ,OMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMKdkXWNo ,ONo               oNO, oNWXkdKMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMXc ;KMXc             lXMK; cXMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMX0kkOKWMNd  xWMMXl           lXMMWx  dNMWKOkk0NMMMMMMMMMMM\n");
printf("MMMMMMMMMXo,    'lKk' cXMMMWx           xMMMMXc 'kKl'    ,dXMMMMMMMMM\n");
printf("MMMMMMMMNl :k0kc  ,,  dWMMMWk'         'kMMMMWd  ,,  cO0k: lNMMMMMMMM\n");
printf("MMMMMMMMX: c0WMNo     xWMMMWO;         ;OWMMMMx     oNMW0c :XMMMMMMMM\n");
printf("MMMMMMMMWO:',OMMO,    ;x0Oxc'           'cxOOx;    ,OMMO,'cOWMMMMMMMM\n");
printf("MMMMMMMMMMWK0XMMK;                                 ;KMMX0KWMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMWx'                               'xWMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMNOdooo;              cdc              ;ooodONMMMMMMMMMMMM\n");
printf("MMMMMMMMMMW0:             ;,    cXMXc    ,;             :0WMMMMMMMMMM\n");
printf("MMMMMMMMMMK; ,dO0Okxolllddc   ,dXMMMXd,   cddllloxkO0Od, ;KMMMMMMMMMM\n");
printf("MMMMMMMMMMO' lNMW0ddKMMXd'  ;dXWMMMMMWKd;  'dXMMKdo0WMNl ,OMMMMMMMMMM\n");
printf("MMMMMMMMMMXl 'xKd' ;KWO; 'ckNMMMMMMMMMMMNkc' ;OWK; 'dKx' lNMMMMMMMMMM\n");
printf("MMMMMMMMMMMNk:,, 'c0M0; c0WMKddXMMMMMXddKMW0c ;0W0c' ,,:kNMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMWKOOKWMMk  kMMXl  kMMMMMk  lNMWk' kMMWKO0KWMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMMXc ;x0x, :KMMMMMK: ,x0x; cXMMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMMMXx:,,,;lKWMMMMMMKl;,,,:xXMMMMMMMMMMMMMMMMMMMMMM\n");
printf("MMMMMMMMMMMMMMMMMMMMMMMMWK00XWMMMMMMMMMWX00KWMMMMMMMMMMMMMMMMMMMMMMMM\n");
printf("                             8\"\"\"88 8\"\"\"\"8 \n");
printf("eeeee eeee eeeee eeeee eeeee 8    8 8      \n");
printf("8  88 8  8   8   8  88 8   8 8    8 8eeeee \n");
printf("8   8 8e     8e  8   8 8eee8 8    8     88 \n");
printf("8   8 88     88  8   8 88    8    8 e   88 \n");
printf("8eee8 88e8   88  8eee8 88    8eeee8 8eee88 \n");
printf("                                      (c) Paul Weiss & Fabian Jezuita\n");
printf("*********************************************************************\n");
}

void printfdemo() {
	printf("printf demo mode -- PRESS ANY KEY\n");
	int ret = dbgu_put_byte('#');
	int c = dbgu_get_byte();	
	if (c <= 0) {
		printf("Error %x reading that!\n", c);
		printf("Here are some other nicely formatted things: %p, %x, %c, %s\n", &printfdemo, 0x12, '3', "45");
	} else {
		printf("\nWell done!\nYou entered '%c', acsii index %x \nand it's currently stored at %p in my memory.\n", c, c, &c);
	}	
	printf("And heres what happnes with unknown formats:\n %a %b %% %f\n", 1, 2, 3, 4);	
	printf("done. (press ^a then x to quit)");
}

/* MAIN */
void c_entry(void) {
	dbgu_init();
	logo();
	printfdemo();
}

#pragma GCC pop_options