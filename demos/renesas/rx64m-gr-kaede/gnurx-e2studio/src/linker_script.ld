MEMORY
{
	RAM : ORIGIN = 0x0, LENGTH = 524288
	ROM : ORIGIN = 0xFFC00000, LENGTH = 4194304
	OFS : ORIGIN = 0x120000, LENGTH = 256
}
SECTIONS
{
	.exvectors 0xFFFFFF80 : AT(0xFFFFFF80)
	{
		_exvectors_start = .;
		KEEP(*(.exvectors))
		_exvectors_end = .;
	} >ROM
	.fvectors 0xFFFFFFFC : AT(0xFFFFFFFC)
	{
		KEEP(*(.fvectors))
	} >ROM
	.text 0xFFC00000 : AT(0xFFC00000)
	{
		*(.text)
		*(.text.*)
		*(P)
		etext = .;
	} >ROM
	.rvectors :
	{
		_rvectors_start = .;
		INCLUDE ../src/linker_script_rvectors.ld_inc
		_rvectors_end = .;
	} >ROM
	.init :
	{
		KEEP(*(.init))
		__preinit_array_start = .;
		KEEP(*(.preinit_array))
		__preinit_array_end = .;
		__init_array_start = (. + 3) & ~ 3;
		KEEP(*(.init_array))
		KEEP(*(SORT(.init_array.*)))
		__init_array_end = .;
		__fini_array_start = .;
		KEEP(*(.fini_array))
		KEEP(*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >ROM
	.fini :
	{
		KEEP(*(.fini))
	} >ROM
	.got :
	{
		*(.got)
		*(.got.plt)
	} >ROM
	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_erodata = .;
	} >ROM
	.eh_frame_hdr :
	{
		*(.eh_frame_hdr)
	} >ROM
	.eh_frame :
	{
		*(.eh_frame)
	} >ROM
	.jcr :
	{
		*(.jcr)
	} >ROM
	.tors :
	{
		__CTOR_LIST__ = .;
		. = ALIGN(2);
		___ctors = .;
		*(.ctors)
		___ctors_end = .;
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		___dtors = .;
		*(.dtors)
		___dtors_end = .;
		__DTOR_END__ = .;
		. = ALIGN(2);
		_mdata = .;
	} >ROM
	.r_bsp_NULL 0 : AT(0)
	{
		. += 0x100;
		_r_bsp_NULL_end = .;
	} >RAM
	.r_bsp_ustack BLOCK(4) :
	{
		KEEP(*(.r_bsp_ustack))
	} >RAM
	.ustack :
	{
		_ustack = .;
	} >RAM
	.r_bsp_istack BLOCK(4) :
	{
		KEEP(*(.r_bsp_istack))
	} >RAM
	.istack :
	{
		_istack = .;
	} >RAM
	.data : AT(_mdata)
	{
		_data = .;
		*(.data)
		*(.data.*)
		*(D)
		*(D_1)
		*(D_2)
		_edata = .;
	} >RAM
	.gcc_exc :
	{
		*(.gcc_exc)
	} >RAM
	.bss :
	{
		_bss = .;
		*(.bss)
		*(.bss.**)
		*(COMMON)
		*(B)
		*(B_1)
		*(B_2)
		_ebss = .;
		_end = .;
	} >RAM
	B_ETHERNET_BUFFERS_1 BLOCK(16) :
	{
		_B_ETHERNET_BUFFERS_1_start = .;
		*(B_ETHERNET_BUFFERS_1)
		_B_ETHERNET_BUFFERS_1_end = .;
	} >RAM
	B_RX_DESC_1 :
	{
		_B_RX_DESC_1_start = .;
		*(B_RX_DESC_1)
		_B_RX_DESC_1_end = .;
	} >RAM
	B_TX_DESC_1 :
	{
		_B_TX_DESC_1_start = .;
		*(B_TX_DESC_1)
		_B_TX_DESC_1_end = .;
	} >RAM
	.ofs1 0x120040 : AT(0x120040)
	{
		KEEP(*(.ofs1))
	} >OFS
	.ofs2 0x120048 : AT(0x120048)
	{
		KEEP(*(.ofs2))
	} >OFS
	.ofs3 0x120050 : AT(0x120050)
	{
		KEEP(*(.ofs3))
	} >OFS
	.ofs4 0x120060 : AT(0x120060)
	{
		KEEP(*(.ofs4))
	} >OFS
}
