OUTPUT_FORMAT("binary")
ENTRY(start)
SECTIONS
{
  .text  0x0 : {
    *(.text)
  }
  .data  : {
    *(.data)
  }
  .bss  :
  { 					
    *(.bss)
  }
}

