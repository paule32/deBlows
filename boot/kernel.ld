ENTRY(_start)

SECTIONS {
    . = 1M;

    .text :
    {
	*(.multiboot)
        *(.text)
    }

        . = ALIGN(16);
        .data           : { *(.data*) }

        .signature      : {
                setup_sig = .;
                LONG(0x5a5aaa55)
        }


        . = ALIGN(16);
        .bss :
        {
                *(COMMON)
                __bss_start = .;
                *(.bss)
                __bss_end = .;
        }
        . = ALIGN(16);
        _end = .;

        /DISCARD/ : { *(.note*) }
}
