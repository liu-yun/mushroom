#include "stdafx.h"
#include "graphics.h"

int main() {
	int graphdrive = VGA, graphmode = VGAHI;
	initgraph(&graphdrive, &graphmode, "");
	getchar();
	closegraph();
    return 0;
}

