OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
PROVIDE(__stack = 0xd0008000);

SECTIONS
{
	.rom 0x8c010000 : {
		crt0.o   (.text)
		. = ALIGN(0x80);
		crt0.o   (.exceptRam)
		. = ALIGN(0x100);
		. += 0x80;
		crt0.o   (.exceptRom)
		init.o   (.text)
		. = ALIGN(1024);
		_erom = .;
	}
	.imem . : AT ( LOADADDR(.rom) + SIZEOF(.rom) ) {
		_text = . ;
		*(.imem)
		. = ALIGN(16);
	}
	.text . : AT ( LOADADDR(.imem) + SIZEOF(.imem) ) {
 		*(.text)
		 _etext = . ; 	
		 PROVIDE (etext = .);
		 . = ALIGN(16);
		_dataStart = .;
	} 

	.data . : AT ( LOADADDR(.text) + SIZEOF(.text) ) {
		_data = . ;
		*(.data)
		_edata = . ;	
		 . = ALIGN(16);
	} 

	.eh_frame . : AT ( LOADADDR(.data) + SIZEOF(.data) ) {
		*(.eh_frame)
		 . = ALIGN(16);
	}

	.init    . : { *(.init) } = 0
	.fini    . : { *(.fini) } = 0
	.ctors   . : AT ( LOADADDR(.eh_frame) + SIZEOF(.eh_frame) ) {
		*(.ctors)
		 . = ALIGN(16);
	}
	.dtors   . : AT ( LOADADDR(.ctors) + SIZEOF(.ctors) ) {
		*(.dtors)
		 . = ALIGN(16);
	}
	_etext = . ;
	etext = . ;
 
	.rodata . : AT ( LOADADDR(.dtors) + SIZEOF(.dtors) ) {
		*(.rodata)
		. = ALIGN(16);
	}
	.rel.dyn . : AT ( LOADADDR(.rodata) + SIZEOF(.rodata) ) { *(.rel.dyn) } = 0
	_gp = . + 0x8000 ;
	__global = _gp ;

	.sdata . : AT ( LOADADDR(.rel.dyn) + SIZEOF(.rel.dyn) ) {
		*(.sdata)
		. = ALIGN(16);
	}
	.sbss . : AT ( LOADADDR(.sdata) + SIZEOF(.sdata) ) {
		_bssStart = . ;
		*(.scommon)
		*(.sbss)
		. = ALIGN(16);
	}
	.bss . : AT ( LOADADDR(.sbss) + SIZEOF(.sbss) ) {
		*(.bss)
		*(COMMON)
		 . = ALIGN(16);
		_bssEnd = . ;
	}

	.reginfo . : AT ( LOADADDR(.bss) + SIZEOF(.bss) ) {
		*(.reginfo)
	}
	.mdebug . : AT ( LOADADDR(.reginfo) + SIZEOF(.reginfo) ) {
		*(.mdebug)
	}
	_bottomOfHeap = . ;
}
