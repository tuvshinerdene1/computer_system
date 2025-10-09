	.file	"exercise4.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB23:
	.cfi_startproc
	endbr64
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE23:
	.size	main, .-main
	.globl	cond
	.type	cond, @function
cond:
.LFB24:
	.cfi_startproc
	endbr64
	testq	%rsi, %rsi
	je	.L2
	cmpq	%rdi, (%rsi)
	jge	.L2
	movq	%rdi, (%rsi)
.L2:
	ret
	.cfi_endproc
.LFE24:
	.size	cond, .-cond
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
