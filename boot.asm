bits 32
MULTIBOOT_ALIGN	equ 1<<0
MEMINFO		equ 1<<1             
FLAGS		equ MULTIBOOT_ALIGN | MEMINFO  
MAGIC		equ 0x1BADB002       
CHECKSUM	equ -(MAGIC + FLAGS) 

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
align 16
stack_bottom:
	resb 16384 
stack_top:

section .rodata
align 8
	gdt_start:
		dd 0, 0
		dw 0xffff, 0x0000, 0x9a00, 0x00cf
		dw 0xffff, 0x0000, 0x9200, 0x00cf
	gdt_end:

	gdt_descriptor:
		dw gdt_end - gdt_start - 1
		dd gdt_start

section .text
global _start
extern kernel_main
_start:
	cli
	mov esp, stack_top

	lgdt [gdt_descriptor]

	jmp 0x08:protected_mode_start

protected_mode_start:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov gs, ax
	mov fs, ax

call kernel_main

.loop:
	hlt
	jmp .loop


